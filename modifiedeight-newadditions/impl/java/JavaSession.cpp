#include <java/JavaSession.hpp>
#include <level/LevelHeight.hpp>
#include <java/JavaChunkData.hpp>
#include <java/JavaLog.hpp>
#include <java/JavaChunkSource.hpp>
#include <java/JavaIdMap.hpp>
#include <java/JavaY.hpp>

#include <Minecraft.hpp>
#include <entity/player/User.hpp>
#include <gui/Gui.hpp>
#include <gui/screens/DisconnectionScreen.hpp>
#include <entity/LocalPlayer.hpp>
#include <level/MultiPlayerLevel.hpp>
#include <level/LevelSettings.hpp>
#include <level/chunk/LevelChunk.hpp>
#include <level/dimension/Dimension.hpp>
#include <level/storage/LevelStorageSource.hpp>
#include <tile/Tile.hpp>
#include <tile/entity/SignTileEntity.hpp>
#include <item/Item.hpp>
#include <item/ItemInstance.hpp>
#include <inventory/Inventory.hpp>
#include <entity/Mob.hpp>
#include <entity/MobFactory.hpp>
#include <entity/EntityFactory.hpp>
#include <entity/RemotePlayer.hpp>
#include <entity/ItemEntity.hpp>
#include <entity/FallingTile.hpp>
#include <entity/Painting.hpp>
#include <entity/player/gamemode/GameMode.hpp>
#include <util/Util.hpp>
#include <utils.h>

#include <json/json.h>
#include <zlib.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

/*
 * Connection milestones go to stdout unconditionally - there are only a handful
 * per session and they are the first thing anyone needs when a join goes wrong.
 * Per packet noise is behind M8_JAVA_DEBUG=1 in the environment.
 */
void javaLog(const char_t* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	fputs("[java] ", stdout);
	vprintf(fmt, ap);
	fputc('\n', stdout);
	fflush(stdout);
	va_end(ap);
}

bool_t javaDebugEnabled() {
	static int32_t cached = -1;
	if(cached < 0) {
		const char_t* v = getenv("M8_JAVA_DEBUG");
		cached = (v && v[0] && v[0] != '0') ? 1 : 0;
	}
	return (bool_t)cached;
}

/*
 * M8_JAVA_DEBUG=2 additionally names every packet that crosses the wire in
 * either direction.  A verification lobby is a conversation held entirely in
 * packets nothing on screen ever mentions, and this is the only way to see it.
 */
bool_t javaTraceEnabled() {
	static int32_t cached = -1;
	if(cached < 0) {
		const char_t* v = getenv("M8_JAVA_DEBUG");
		cached = (v && v[0] && v[0] != '0' && v[0] != '1') ? 1 : 0;
	}
	return (bool_t)cached;
}

void javaDebug(const char_t* fmt, ...) {
	if(!javaDebugEnabled()) {
		return;
	}
	va_list ap;
	va_start(ap, fmt);
	fputs("[java] ", stdout);
	vprintf(fmt, ap);
	fputc('\n', stdout);
	fflush(stdout);
	va_end(ap);
}

/*
 * Java sends chat as a JSON text component tree. Flatten it into the plain text
 * m8's chat line renderer can take: concatenate "text" and "extra", and for a
 * "translate" component fall back to the key plus its arguments rather than
 * dropping the message entirely (that is what carries join/leave/death notices).
 */
static void javaChatFlatten(const Json::Value& node, std::string* out) {
	if(node.isString()) {
		*out += node.asString();
		return;
	}
	if(node.isArray()) {
		for(Json::ArrayIndex i = 0; i < node.size(); ++i) {
			javaChatFlatten(node[i], out);
		}
		return;
	}
	if(!node.isObject()) {
		return;
	}
	if(node.isMember("text") && node["text"].isString()) {
		*out += node["text"].asString();
	}
	if(node.isMember("translate") && node["translate"].isString()) {
		const Json::Value& with = node["with"];
		if(with.isArray() && with.size()) {
			for(Json::ArrayIndex i = 0; i < with.size(); ++i) {
				if(i) *out += " ";
				javaChatFlatten(with[i], out);
			}
		} else {
			*out += node["translate"].asString();
		}
	}
	if(node.isMember("extra")) {
		javaChatFlatten(node["extra"], out);
	}
}

static std::string javaChatToText(const std::string& json) {
	Json::Value root;
	Json::Reader reader;
	if(!reader.parse(json, root, false)) {
		// Some servers send a bare string for disconnect reasons.
		return json;
	}
	std::string out;
	javaChatFlatten(root, &out);
	// Strip the section-sign colour codes m8's font does not understand.
	std::string clean;
	clean.reserve(out.size());
	for(size_t i = 0; i < out.size(); ++i) {
		if((uint8_t)out[i] == 0xC2 && i + 2 < out.size() && (uint8_t)out[i + 1] == 0xA7) {
			i += 2;
			continue;
		}
		clean += out[i];
	}
	return clean;
}

JavaSession::JavaSession() {
	this->inOff = 0;
	this->outOff = 0;
	this->compressionThreshold = -1;
	this->phase = JavaSession::IDLE;
	this->port = 0;
	this->reportedFailure = 0;
	this->minecraft = 0;
	this->level = 0;
	this->chunkSource = 0;
	this->player = 0;
	this->levelRequested = 0;
	this->levelLive = 0;
	this->serverEid = 0;
	this->gamemode = 0;
	this->dimension = 0;
	this->hasSkyLight = 1;
	this->posX = 0.0;
	this->posY = 64.0;
	this->posZ = 0.0;
	this->rotYaw = 0.0f;
	this->rotPitch = 0.0f;
	this->havePosition = 0;
	this->deferredBytes = 0;
	this->deferredOverflowed = 0;
	this->tickCounter = 0;
	this->sentX = 0.0;
	this->sentY = 0.0;
	this->sentZ = 0.0;
	this->sentYaw = 0.0f;
	this->sentPitch = 0.0f;
	this->sentGround = 0;
	this->everSentPosition = 0;
	this->positionUpdateTicks = 0;
	this->sentSprint = 0;
	this->sentSneak = 0;
	this->lastMoveSentMs = 0;
	this->moveWindowMs = 0;
	memset(this->moveWindow, 0, sizeof(this->moveWindow));
	this->selectedSlot = 0;
	this->nextParkedEid = -2;
	this->respawnHold = 0;
	this->abilityFlags = 0;
	this->hasAbilityFlags = 0;
}

JavaSession::~JavaSession() {
	this->socket.close();
}

bool_t JavaSession::isActive() const {
	return this->phase != JavaSession::IDLE && this->phase != JavaSession::CLOSED;
}

bool_t JavaSession::begin(Minecraft* mc, const std::string& name, const std::string& host,
                          int32_t typedPort, const std::string& user) {
	JavaSocket::globalInit();

	this->minecraft = mc;
	this->displayName = name;
	this->username = user;
	this->phase = JavaSession::IDLE;
	this->failure = "";
	this->reportedFailure = 0;

	std::string connectHost;
	if(!JavaSocket::resolveMinecraftHost(host, typedPort, &this->ip, &this->port, &connectHost)) {
		this->fail("Could not resolve " + host);
		return 0;
	}
	this->handshakeHost = connectHost;
	javaLog("connecting to %s:%d (handshake host \"%s\") as \"%s\"", this->ip.c_str(),
	        (int)this->port, this->handshakeHost.c_str(), this->username.c_str());

	if(!this->socket.beginConnect(this->ip, this->port)) {
		this->fail("Could not connect to " + this->ip + ": " + this->socket.lastError);
		return 0;
	}
	this->phase = JavaSession::CONNECTING;
	return 1;
}

void JavaSession::fail(const std::string& why) {
	if(this->failure.size()) {
		return;
	}
	this->failure = why;
	this->phase = JavaSession::CLOSED;
	this->socket.close();
	javaLog("session ended: %s", why.c_str());
}

void JavaSession::shutdown() {
	this->socket.close();
	this->phase = JavaSession::CLOSED;
	this->inBuf.clear();
	this->outBuf.clear();
	this->inOff = 0;
	this->outOff = 0;
	this->deferred.clear();
	this->deferredBytes = 0;
	this->level = 0;
	this->chunkSource = 0;
	this->player = 0;
	this->levelRequested = 0;
	this->levelLive = 0;
	this->knownEntities.clear();
	this->pendingItems.clear();
	this->playerNames.clear();
	this->nextParkedEid = -2;
	this->respawnHold = 0;
	this->abilityFlags = 0;
	this->hasAbilityFlags = 0;
}

// ---------------------------------------------------------------- wire ------

void JavaSession::compactIn() {
	if(this->inOff == 0) {
		return;
	}
	if(this->inOff >= this->inBuf.size()) {
		this->inBuf.clear();
		this->inOff = 0;
		return;
	}
	// Only pay for the move once the dead prefix is worth reclaiming.
	if(this->inOff < 65536 && this->inOff * 2 < this->inBuf.size()) {
		return;
	}
	this->inBuf.erase(this->inBuf.begin(), this->inBuf.begin() + (long)this->inOff);
	this->inOff = 0;
}

void JavaSession::flushOut() {
	while(this->outOff < this->outBuf.size()) {
		int32_t chunk = (int32_t)(this->outBuf.size() - this->outOff);
		if(chunk > 32768) chunk = 32768;
		int32_t n = this->socket.sendSome(&this->outBuf[this->outOff], chunk);
		if(n < 0) {
			this->fail("Connection lost: " + this->socket.lastError);
			return;
		}
		if(n == 0) {
			break; // socket buffer full, try again next frame
		}
		this->outOff += (size_t)n;
	}
	if(this->outOff >= this->outBuf.size()) {
		this->outBuf.clear();
		this->outOff = 0;
	} else if(this->outOff >= 65536) {
		this->outBuf.erase(this->outBuf.begin(), this->outBuf.begin() + (long)this->outOff);
		this->outOff = 0;
	}
}

void JavaSession::beginPacket(JavaByteBuf* out, int32_t id) {
	out->clear();
	out->writeVarInt(id);
}

void JavaSession::send(const JavaByteBuf& payload) {
	if(this->phase == JavaSession::CLOSED || this->phase == JavaSession::IDLE) {
		return;
	}
	if(javaTraceEnabled() && payload.size()) {
		int32_t id = payload.data()[0];
		// The four movement ids go out twenty times a second and the air trace
		// already accounts for them.
		if(id < 0x03 || id > 0x06) {
			javaDebug("-> 0x%02X (%d bytes)", (int)id, (int)payload.size());
		}
	}
	JavaByteBuf frame;
	if(this->compressionThreshold < 0) {
		frame.writeVarInt((int32_t)payload.size());
		frame.writeBytes(payload.data(), payload.size());
	} else if((int32_t)payload.size() >= this->compressionThreshold) {
		uLongf cap = compressBound((uLong)payload.size());
		std::vector<uint8_t> comp((size_t)cap);
		if(compress2(&comp[0], &cap, payload.data(), (uLong)payload.size(), Z_DEFAULT_COMPRESSION) != Z_OK) {
			this->fail("Could not compress an outgoing packet");
			return;
		}
		JavaByteBuf inner;
		inner.writeVarInt((int32_t)payload.size());
		inner.writeBytes(&comp[0], (size_t)cap);
		frame.writeVarInt((int32_t)inner.size());
		frame.writeBytes(inner.data(), inner.size());
	} else {
		// Under the threshold: the length prefix stays, the data does not get
		// deflated, and a zero uncompressed-length says so.
		frame.writeVarInt((int32_t)payload.size() + 1);
		frame.writeVarInt(0);
		frame.writeBytes(payload.data(), payload.size());
	}
	this->outBuf.insert(this->outBuf.end(), frame.bytes.begin(), frame.bytes.end());
}

bool_t JavaSession::decodeFrame(const uint8_t* raw, size_t rawLen, JavaByteBuf* out) {
	if(this->compressionThreshold < 0) {
		out->bytes.assign(raw, raw + rawLen);
		out->reset();
		return 1;
	}

	JavaByteBuf head(raw, rawLen);
	int32_t uncompressedLen = head.readVarInt();
	if(head.failed()) {
		return 0;
	}
	size_t consumed = head.readPos;
	if(uncompressedLen == 0) {
		out->bytes.assign(raw + consumed, raw + rawLen);
		out->reset();
		return 1;
	}
	if(uncompressedLen < 0 || uncompressedLen > JavaSession::MAX_FRAME) {
		return 0;
	}
	out->bytes.resize((size_t)uncompressedLen);
	uLongf destLen = (uLongf)uncompressedLen;
	int rc = uncompress(&out->bytes[0], &destLen, raw + consumed, (uLong)(rawLen - consumed));
	if(rc != Z_OK || destLen != (uLongf)uncompressedLen) {
		return 0;
	}
	out->reset();
	return 1;
}

void JavaSession::pumpSocket() {
	uint8_t buf[16384];
	for(int32_t guard = 0; guard < 256; ++guard) {
		int32_t n = this->socket.recvSome(buf, (int32_t)sizeof(buf));
		if(n < 0) {
			this->fail(this->socket.lastError.size() ? ("Connection lost: " + this->socket.lastError)
			                                        : std::string("Server closed the connection"));
			return;
		}
		if(n == 0) {
			return;
		}
		this->inBuf.insert(this->inBuf.end(), buf, buf + n);
		if(n < (int32_t)sizeof(buf)) {
			return;
		}
	}
}

void JavaSession::dispatchFrames() {
	while(1) {
		size_t avail = this->inBuf.size() - this->inOff;
		if(avail == 0) {
			break;
		}
		const uint8_t* p = &this->inBuf[this->inOff];

		// Frame length, as a VarInt that may not have arrived in full yet.
		int32_t length = 0;
		size_t lenBytes = 0;
		bool_t complete = 0;
		while(lenBytes < avail && lenBytes < 5) {
			uint8_t b = p[lenBytes];
			length |= (int32_t)(b & 0x7F) << (int32_t)(7 * lenBytes);
			++lenBytes;
			if((b & 0x80) == 0) {
				complete = 1;
				break;
			}
		}
		if(!complete) {
			if(lenBytes >= 5) {
				this->fail("Malformed packet length from server");
			}
			break;
		}
		if(length < 0 || length > JavaSession::MAX_FRAME) {
			this->fail("Server sent an implausible packet length");
			break;
		}
		if(avail < lenBytes + (size_t)length) {
			break; // rest of the frame still in flight
		}

		JavaByteBuf frame;
		bool_t ok = this->decodeFrame(p + lenBytes, (size_t)length, &frame);
		this->inOff += lenBytes + (size_t)length;
		if(!ok) {
			this->fail("Could not decompress a packet from the server");
			break;
		}

		int32_t id = frame.readVarInt();
		if(frame.failed()) {
			continue;
		}
		if(this->phase == JavaSession::LOGIN) {
			this->handleLoginPacket(&frame, id);
		} else if(this->phase == JavaSession::PLAY) {
			this->handlePlayPacket(&frame, id);
		}
		if(this->phase == JavaSession::CLOSED) {
			break;
		}
	}
	this->compactIn();
}

void JavaSession::sendHandshakeAndLogin() {
	JavaByteBuf pk;
	this->beginPacket(&pk, 0x00);
	pk.writeVarInt(JavaSession::PROTOCOL);
	pk.writeString(this->handshakeHost);
	pk.writeUShort((uint16_t)this->port);
	pk.writeVarInt(2); // next state: login
	this->send(pk);
	javaLog("tcp up, sending handshake (protocol %d) + login start", (int)JavaSession::PROTOCOL);

	this->beginPacket(&pk, 0x00);
	pk.writeString(this->username);
	this->send(pk);

	this->phase = JavaSession::LOGIN;
}

void JavaSession::pump() {
	if(this->phase == JavaSession::IDLE) {
		return;
	}

	if(this->phase == JavaSession::CLOSED) {
		if(this->failure.size() && !this->reportedFailure && this->minecraft) {
			this->reportedFailure = 1;
			if(this->levelLive) {
				this->minecraft->gui.addMessage("", this->failure, 2400);
				this->minecraft->setScreen(new DisconnectionScreen(this->failure));
				this->minecraft->setLeaveGame();
			} else {
				this->minecraft->setScreen(new DisconnectionScreen(this->failure));
			}
		}
		return;
	}

	if(this->phase == JavaSession::CONNECTING) {
		this->socket.pumpConnect();
		if(this->socket.state == JavaSocket::FAILED) {
			this->fail("Could not connect to " + this->ip + ": " + this->socket.lastError);
			return;
		}
		if(this->socket.state != JavaSocket::CONNECTED) {
			return;
		}
		this->sendHandshakeAndLogin();
	}

	this->pumpSocket();
	if(this->phase == JavaSession::CLOSED) {
		return;
	}
	this->dispatchFrames();
	if(this->phase == JavaSession::CLOSED) {
		return;
	}

	// The level finished building on the worker thread; everything held back
	// while that was happening can be applied now.
	if(this->levelRequested && !this->levelLive && this->minecraft &&
	   this->minecraft->levelGenerated && !this->minecraft->field_CF4) {
		this->levelLive = 1;
		javaLog("level live, world is now server driven");
		this->player = this->minecraft->player;
		/*
		 * Both of these describe a player that did not exist when the server
		 * described it.  Gamemode first, because it is the default the abilities
		 * packet is allowed to override.
		 */
		this->applyGameMode();
		this->flushDeferred();
		this->pushCreativeInventory();
	}

	if(this->levelLive) {
		++this->tickCounter;
		this->holdPlayerForTerrain();
		/*
		 * Movement is reported from playerTick(), once per game tick, exactly
		 * like a Java client.  This is only the safety net for the stretches
		 * where the player is not being ticked at all - a screen open over the
		 * world, terrain still loading, the death screen - so that the server
		 * still hears where we are about once a second.
		 */
		if((int32_t)(getTimeMs() - this->lastMoveSentMs) > 1000) {
			this->sendPositionNow(1);
		}
		this->syncHeldSlot();
		if(javaDebugEnabled() && (this->tickCounter % 200) == 0) {
			double px = 0, py = 0, pz = 0;
			if(this->player) {
				px = this->player->posX;
				py = JavaY::toJavad(this->player->boundingBox.minY);
				pz = this->player->posZ;
			}
			javaDebug("t=%d pos %.1f %.1f %.1f entities=%d chunks=%d",
			          (int)this->tickCounter, px, py, pz,
			          (int)this->knownEntities.size(),
			          this->chunkSource ? (int)this->chunkSource->residentCount : 0);
		}
	}

	this->flushOut();
}

void JavaSession::deferFrame(const JavaByteBuf& frame, int32_t id) {
	if(this->deferredBytes > JavaSession::MAX_DEFERRED) {
		if(!this->deferredOverflowed) {
			this->deferredOverflowed = 1;
			printf("[java] deferred packet buffer full, dropping world data\n");
		}
		return;
	}
	std::vector<uint8_t> keep;
	keep.reserve(frame.bytes.size() + 5);
	JavaByteBuf head;
	head.writeVarInt(id);
	keep.insert(keep.end(), head.bytes.begin(), head.bytes.end());
	keep.insert(keep.end(), frame.bytes.begin() + (long)frame.readPos, frame.bytes.end());
	this->deferredBytes += keep.size();
	this->deferred.push_back(keep);
}

void JavaSession::flushDeferred() {
	// Swap first: a handler could in principle defer again, and it must not
	// append into the container being walked.
	std::vector<std::vector<uint8_t> > pending;
	pending.swap(this->deferred);
	this->deferredBytes = 0;
	for(size_t i = 0; i < pending.size(); ++i) {
		JavaByteBuf frame(&pending[i][0], pending[i].size());
		int32_t id = frame.readVarInt();
		if(frame.failed()) continue;
		this->handlePlayPacket(&frame, id);
		if(this->phase == JavaSession::CLOSED) {
			return;
		}
	}
}

// --------------------------------------------------------------- login ------

void JavaSession::handleLoginPacket(JavaByteBuf* in, int32_t id) {
	switch(id) {
	case 0x00: { // Disconnect
		std::string json = in->readString();
		std::string why = javaChatToText(json);
		this->fail(why.size() ? why : std::string("Server refused the connection"));
		break;
	}
	case 0x01: // Encryption Request
		this->fail("This server is in online mode. m8 can only join offline-mode (cracked) Java servers.");
		break;
	case 0x02: { // Login Success
		in->readString(); // uuid
		std::string name = in->readString(16);
		if(name.size()) {
			this->username = name;
		}
		this->phase = JavaSession::PLAY;
		javaLog("login accepted as \"%s\"", this->username.c_str());
		break;
	}
	case 0x03: { // Set Compression
		int32_t threshold = in->readVarInt();
		if(!in->failed()) {
			this->compressionThreshold = threshold;
			javaLog("compression enabled, threshold %d", (int)threshold);
		}
		break;
	}
	default:
		break;
	}
}

// ---------------------------------------------------------------- play ------

void JavaSession::handlePlayPacket(JavaByteBuf* in, int32_t id) {
	if(javaTraceEnabled()) {
		javaDebug("<- 0x%02X (%d bytes)", (int)id, (int)(in->bytes.size() - in->readPos));
	}
	// A handful of packets must run whatever state the level is in: the
	// keepalive keeps us connected, the disconnect reason has to be shown, the
	// compression switch changes how every later frame is framed, and the join
	// plus first position are what build the level in the first place.
	switch(id) {
	case 0x00: { // Keep Alive
		int32_t token = in->readVarInt();
		if(!in->failed()) this->sendKeepAlive(token);
		return;
	}
	case 0x40: { // Disconnect
		std::string why = javaChatToText(in->readString());
		this->fail(why.size() ? why : std::string("Disconnected by server"));
		return;
	}
	case 0x46: { // Set Compression (play state)
		int32_t threshold = in->readVarInt();
		if(!in->failed()) this->compressionThreshold = threshold;
		return;
	}
	case 0x01: { // Join Game
		this->serverEid = in->readInt();
		this->gamemode = in->readByte() & 0x7;
		this->dimension = in->readSByte();
		in->readByte();   // difficulty
		in->readByte();   // max players
		in->readString(); // level type
		this->hasSkyLight = this->dimension != -1;
		javaLog("join game: eid=%d gamemode=%d dimension=%d skylight=%d",
		        (int)this->serverEid, (int)this->gamemode, (int)this->dimension,
		        (int)this->hasSkyLight);
		/*
		 * NetHandlerPlayClient.handleJoinGame ends with exactly these two
		 * packets - settings first, then the brand - and it sends them there and
		 * not later, before a single chunk has arrived.  The order and the
		 * timing both matter: the anti-bot front ends public servers put in
		 * front of the real server (Sonar and the like) hold every new
		 * connection in a limbo world and wait for that pair before they will
		 * hand it on, and a client that never sends them is dropped as a bot.
		 * That is why joining used to work only after a real Java client had
		 * been on the same address - the check remembers verified addresses for
		 * a while, and m8 was riding on that rather than passing.
		 */
		this->sendClientSettings();
		this->sendClientBrand();
		return;
	}
	case 0x08: { // Player Position And Look
		double x = in->readDouble();
		double y = in->readDouble();
		double z = in->readDouble();
		float yaw = in->readFloat();
		float pitch = in->readFloat();
		int32_t flags = in->readByte();
		if(in->failed()) return;

		// Relative bits: 0x01 X, 0x02 Y, 0x04 Z, 0x08 yaw, 0x10 pitch.
		if(flags & 0x01) x += this->posX;
		if(flags & 0x02) y += this->posY;
		if(flags & 0x04) z += this->posZ;
		if(flags & 0x08) yaw += this->rotYaw;
		if(flags & 0x10) pitch += this->rotPitch;

		this->posX = x;
		this->posY = y;
		this->posZ = z;
		this->rotYaw = yaw;
		this->rotPitch = pitch;
		this->havePosition = 1;

		if(!this->levelRequested) {
			this->createLevel();
			return;
		}
		javaDebug("in teleport %.2f %.2f %.2f (was %.2f %.2f %.2f, d=%.3f) flags=%d",
		          x, y, z, this->sentX, this->sentY, this->sentZ,
		          sqrt((x - this->sentX) * (x - this->sentX)
		             + (y - this->sentY) * (y - this->sentY)
		             + (z - this->sentZ) * (z - this->sentZ)),
		          (int)flags);

		if(this->levelLive && this->player) {
			// setPos() offsets the box by ySize, so a step still in progress would
			// shift a server teleport up by as much as half a block.  The server
			// has just told us exactly where we are; there is nothing left to
			// smooth over.
			this->player->ySize = 0.0f;
			this->player->moveTo((float)x, (float)JavaY::toM8d(y), (float)z, yaw, pitch);
			this->player->motionX = 0.0;
			this->player->motionY = 0.0;
			this->player->motionZ = 0.0;
			this->sentX = x;
			this->sentY = y;
			this->sentZ = z;
			this->sentYaw = yaw;
			this->sentPitch = pitch;
			this->everSentPosition = 1;
		}
		// 1.8 wants the exact same values echoed back to confirm the teleport.
		JavaByteBuf pk;
		this->beginPacket(&pk, 0x06);
		pk.writeDouble(x);
		pk.writeDouble(y);
		pk.writeDouble(z);
		pk.writeFloat(yaw);
		pk.writeFloat(pitch);
		pk.writeBool(1);
		this->send(pk);
		return;
	}
	case 0x3F: { // Plugin Message
		/*
		 * Nothing here speaks any of these channels, but which channel a server
		 * opens says a lot about what it is running, and that is the first thing
		 * worth knowing when a login is being refused.
		 */
		std::string channel = in->readString();
		if(!in->failed()) {
			javaDebug("in plugin message '%s' (%d bytes)", channel.c_str(),
			          (int)(in->bytes.size() - in->readPos));
		}
		return;
	}
	case 0x48: { // Resource Pack Send
		std::string url = in->readString();
		std::string hash = in->readString();
		if(in->failed()) return;
		javaLog("server offers resource pack %s", url.c_str());
		/*
		 * m8 cannot load a Java resource pack, but the reply is not about the
		 * pack: a server that sends this and hears nothing back is entitled to
		 * assume the client is not a client at all, and some verification plugins
		 * use exactly that.  Vanilla answers ACCEPTED and then, once the download
		 * finishes, SUCCESSFULLY_LOADED, so answer both.
		 */
		this->sendResourcePackStatus(hash, 3); // ACCEPTED
		this->sendResourcePackStatus(hash, 0); // SUCCESSFULLY_LOADED
		return;
	}
	case 0x41: // Server Difficulty
	case 0x37: // Statistics
	case 0x44: // World Border
	case 0x45: // Title
	case 0x47: // Player List Header/Footer
		return;
	default:
		break;
	}

	if(!this->levelLive) {
		this->deferFrame(*in, id);
		return;
	}
	if(!this->level) {
		return;
	}

	switch(id) {
	case 0x02: { // Chat Message
		std::string json = in->readString(32767);
		if(in->failed()) return;
		javaDebug("in chat json: %s", json.c_str());
		in->readByte(); // position: 0 chat, 1 system, 2 action bar
		std::string text = javaChatToText(json);
		if(text.size() && this->minecraft) {
			this->minecraft->gui.addMessage("", text, 200);
		}
		return;
	}
	case 0x03: { // Time Update
		int64_t worldAge = in->readLong();
		int64_t timeOfDay = in->readLong();
		if(in->failed()) return;
		(void)worldAge;
		/*
		 * Two things had to be said in m8's own terms before the sun would move.
		 *
		 * A day is 24000 ticks in Java and 19200 in 0.8.1, and
		 * NormalDayCycleDimension::getTimeOfDay divides by its own 19200, so the
		 * raw server clock put the sun in the wrong place and slid it backwards
		 * every time the server spoke.  Scale it.
		 *
		 * A negative time is how the server spells doDaylightCycle=false; the
		 * magnitude is still the time of day.  m8 freezes its sky through
		 * levelData.stopTime - Level::getTimeOfDay prefers stopTime over the
		 * clock whenever it is not negative - so mirror the rule there instead
		 * of dropping the sign on the floor.
		 */
		bool_t frozen = timeOfDay < 0;
		if(frozen) timeOfDay = -timeOfDay;
		int32_t m8Time = (int32_t)((timeOfDay % 24000) * 19200 / 24000);
		this->level->setTime(m8Time);
		this->level->levelData.setStopTime(frozen ? m8Time : -1);
		this->level->adventureSettings.daylightCycle = frozen ? 0 : 1;
		return;
	}
	case 0x04: { // Entity Equipment
		int32_t eid = in->readVarInt();
		int32_t slot = in->readShort();
		int32_t itemId = 0, count = 0, damage = 0;
		if(!this->readSlot(in, &itemId, &count, &damage, 0)) return;
		if(slot != 0) return; // only the held item is renderable here
		Entity* e = this->findEntity(eid);
		if(e && e->isPlayer()) {
			Player* p = (Player*)e;
			if(p->inventory) {
				if(itemId > 0) {
					ItemInstance ii(JavaIdMap::javaItemToM8(itemId), count > 0 ? count : 1, damage);
					p->inventory->replaceSlot(9, &ii);
				} else {
					p->inventory->clearSlot(9);
				}
				p->inventory->moveToSelectedSlot(9);
			}
		}
		return;
	}
	case 0x05: { // Spawn Position
		int32_t x = 0, y = 0, z = 0;
		in->readPosition(&x, &y, &z);
		if(in->failed()) return;
		Pos spawn;
		spawn.x = x;
		spawn.y = JavaY::clampM8(JavaY::toM8(y));
		spawn.z = z;
		this->level->setSpawnPos(spawn);
		return;
	}
	case 0x06: { // Update Health
		float health = in->readFloat();
		in->readVarInt();  // food
		in->readFloat();   // saturation
		if(in->failed()) return;
		if(this->player) {
			int32_t hp = (int32_t)(health + 0.5f);
			if(hp < 0) hp = 0;
			if(hp > 20) hp = 20;
			this->player->hurtTo(hp);
		}
		return;
	}
	case 0x07: { // Respawn
		this->dimension = in->readInt();
		in->readByte();   // difficulty
		this->gamemode = in->readByte() & 0x7;
		in->readString(); // level type
		if(in->failed()) return;
		this->hasSkyLight = this->dimension != -1;
		this->resetForRespawn();
		if(this->minecraft) {
			this->applyGameMode();
			if(this->minecraft->currentScreen) this->minecraft->setScreen(0);
		}
		return;
	}
	case 0x09: { // Held Item Change
		int32_t slot = in->readSByte();
		if(in->failed() || slot < 0 || slot > 8) return;
		this->selectedSlot = slot;
		if(this->player && this->player->inventory) {
			this->player->inventory->selectSlot(slot);
		}
		return;
	}
	case 0x0B: { // Animation
		int32_t eid = in->readVarInt();
		int32_t action = in->readByte();
		if(in->failed()) return;
		Entity* e = this->findEntity(eid);
		if(e && action == 0 && e->isPlayer() && e != (Entity*)this->player) {
			((Player*)e)->swing();
		}
		return;
	}
	case 0x0C:
		this->spawnPlayer(in);
		return;
	case 0x0D: { // Collect Item
		int32_t collected = in->readVarInt();
		int32_t collector = in->readVarInt();
		if(in->failed()) return;
		Entity* e = this->findEntity(collected);
		if(e) {
			this->level->playSound(e, "random.pop", 0.2f, 2.0f);
			this->destroyEntity(collected);
		}
		(void)collector;
		return;
	}
	case 0x0E:
		this->spawnObject(in);
		return;
	case 0x0F:
		this->spawnMob(in);
		return;
	case 0x10: { // Spawn Painting
		int32_t eid = in->readVarInt();
		std::string title = in->readString(32);
		int32_t x = 0, y = 0, z = 0;
		in->readPosition(&x, &y, &z);
		// EnumFacing's horizontal index runs S-W-N-E, which is exactly the
		// order HangingEntity::setDir() expects, so the byte passes straight
		// through.  MCPE inherited Java's motive names too, so the title is a
		// direct lookup - anything m8 does not have (1.8 added "Wither") falls
		// back to Kebab inside getMotiveByName rather than to a null motive.
		int32_t facing = in->readByte() & 3;
		if(in->failed() || !this->level || !JavaY::inRange(y)) return;
		Painting* art = new Painting(this->level, x, JavaY::toM8(y), z, facing, title);
		this->attachEntity(eid, art);
		this->knownEntities[eid] = 1;
		return;
	}
	case 0x11: { // Spawn Experience Orb - m8 has no orbs, but eat the bytes
		in->readVarInt();
		in->readInt();
		in->readInt();
		in->readInt();
		in->readShort();
		return;
	}
	case 0x12: { // Entity Velocity
		int32_t eid = in->readVarInt();
		int32_t vx = in->readShort();
		int32_t vy = in->readShort();
		int32_t vz = in->readShort();
		if(in->failed()) return;
		Entity* e = this->findEntity(eid);
		if(e) {
			e->lerpMotion((float)vx / 8000.0f, (float)vy / 8000.0f, (float)vz / 8000.0f);
		}
		return;
	}
	case 0x13: { // Destroy Entities
		int32_t count = in->readVarInt();
		if(in->failed() || count < 0) return;
		for(int32_t i = 0; i < count; ++i) {
			int32_t eid = in->readVarInt();
			if(in->failed()) return;
			this->destroyEntity(eid);
		}
		return;
	}
	case 0x14: // Entity (no movement, just a heartbeat)
		in->readVarInt();
		return;
	case 0x15: { // Entity Relative Move
		int32_t eid = in->readVarInt();
		int32_t dx = in->readSByte();
		int32_t dy = in->readSByte();
		int32_t dz = in->readSByte();
		in->readBool(); // on ground
		if(in->failed()) return;
		this->moveEntity(eid, (double)dx / 32.0, (double)dy / 32.0, (double)dz / 32.0, 0, 0.0f, 0.0f, 0);
		return;
	}
	case 0x16: { // Entity Look
		int32_t eid = in->readVarInt();
		float yaw = (float)in->readSByte() * 360.0f / 256.0f;
		float pitch = (float)in->readSByte() * 360.0f / 256.0f;
		in->readBool();
		if(in->failed()) return;
		this->moveEntity(eid, 0.0, 0.0, 0.0, 1, yaw, pitch, 0);
		return;
	}
	case 0x17: { // Entity Look And Relative Move
		int32_t eid = in->readVarInt();
		int32_t dx = in->readSByte();
		int32_t dy = in->readSByte();
		int32_t dz = in->readSByte();
		float yaw = (float)in->readSByte() * 360.0f / 256.0f;
		float pitch = (float)in->readSByte() * 360.0f / 256.0f;
		in->readBool();
		if(in->failed()) return;
		this->moveEntity(eid, (double)dx / 32.0, (double)dy / 32.0, (double)dz / 32.0, 1, yaw, pitch, 0);
		return;
	}
	case 0x18: { // Entity Teleport
		int32_t eid = in->readVarInt();
		double x = (double)in->readInt() / 32.0;
		double y = (double)in->readInt() / 32.0;
		double z = (double)in->readInt() / 32.0;
		float yaw = (float)in->readSByte() * 360.0f / 256.0f;
		float pitch = (float)in->readSByte() * 360.0f / 256.0f;
		in->readBool();
		if(in->failed()) return;
		this->moveEntity(eid, x, y, z, 1, yaw, pitch, 1);
		return;
	}
	case 0x19: { // Entity Head Look
		int32_t eid = in->readVarInt();
		float headYaw = (float)in->readSByte() * 360.0f / 256.0f;
		if(in->failed()) return;
		Entity* e = this->findEntity(eid);
		if(e && e->isMob()) {
			((Mob*)e)->setYHeadRot(headYaw);
		}
		return;
	}
	case 0x1A: { // Entity Status
		int32_t eid = in->readInt();
		int32_t status = in->readByte();
		if(in->failed()) return;
		Entity* e = this->findEntity(eid);
		if(!e) return;
		if(status == 2) {
			// hurt
			this->level->broadcastEntityEvent(e, 2);
		} else if(status == 3) {
			this->level->broadcastEntityEvent(e, 3);
		}
		return;
	}
	case 0x1C: { // Entity Metadata
		int32_t eid = in->readVarInt();
		if(in->failed()) return;
		int32_t itemId = 0, count = 0, damage = 0;
		if(!this->readMetadata(in, &itemId, &count, &damage)) return;
		if(itemId > 0 && this->pendingItems.count(eid)) {
			Entity* e = this->findEntity(eid);
			if(e && e->isItemEntity()) {
				((ItemEntity*)e)->itemInstance =
					ItemInstance(JavaIdMap::javaItemToM8(itemId), count > 0 ? count : 1, damage);
				this->pendingItems.erase(eid);
			}
		}
		return;
	}
	case 0x1D: // Entity Effect
		in->readVarInt(); in->readByte(); in->readByte(); in->readVarInt(); in->readBool();
		return;
	case 0x1E: // Remove Entity Effect
		in->readVarInt(); in->readByte();
		return;
	case 0x21: { // Chunk Data
		int32_t cx = in->readInt();
		int32_t cz = in->readInt();
		bool_t groundUp = in->readBool();
		int32_t mask = in->readUShort();
		int32_t len = in->readVarInt();
		if(in->failed() || len < 0 || (size_t)len > in->remaining()) return;
		const uint8_t* payload = in->data() + in->readPos;
		if(mask == 0 && groundUp) {
			// 1.8's chunk unload signal.
			if(this->chunkSource) this->chunkSource->drop(cx, cz);
		} else {
			this->applyChunk(cx, cz, mask, groundUp, this->hasSkyLight, payload, (size_t)len);
		}
		in->skip((size_t)len);
		return;
	}
	case 0x22: { // Multi Block Change
		int32_t cx = in->readInt();
		int32_t cz = in->readInt();
		int32_t count = in->readVarInt();
		if(in->failed() || count < 0) return;
		for(int32_t i = 0; i < count; ++i) {
			int32_t packed = in->readUShort();
			int32_t state = in->readVarInt();
			if(in->failed()) return;
			int32_t lx = (packed >> 12) & 0xF;
			int32_t lz = (packed >> 8) & 0xF;
			int32_t jy = packed & 0xFF;
			this->setJavaBlock((cx << 4) + lx, jy, (cz << 4) + lz, state);
		}
		return;
	}
	case 0x23: { // Block Change
		int32_t x = 0, y = 0, z = 0;
		in->readPosition(&x, &y, &z);
		int32_t state = in->readVarInt();
		if(in->failed()) return;
		javaDebug("in block change %d %d %d -> java %d:%d", x, y, z, state >> 4, state & 0xF);
		this->setJavaBlock(x, y, z, state);
		return;
	}
	case 0x24: { // Block Action
		int32_t bax = 0, bay = 0, baz = 0;
		in->readPosition(&bax, &bay, &baz);
		in->readByte(); in->readByte(); in->readVarInt();
		return;
	}
	case 0x25: { // Block Break Animation
		in->readVarInt();
		int32_t x = 0, y = 0, z = 0;
		in->readPosition(&x, &y, &z);
		in->readByte();
		return;
	}
	case 0x26: { // Map Chunk Bulk
		bool_t sky = in->readBool();
		int32_t count = in->readVarInt();
		if(in->failed() || count < 0 || count > 4096) return;
		std::vector<int32_t> cxs((size_t)count), czs((size_t)count), masks((size_t)count);
		for(int32_t i = 0; i < count; ++i) {
			cxs[(size_t)i] = in->readInt();
			czs[(size_t)i] = in->readInt();
			masks[(size_t)i] = in->readUShort();
			if(in->failed()) return;
		}
		this->hasSkyLight = sky;
		for(int32_t i = 0; i < count; ++i) {
			size_t need = JavaChunkData::payloadSize(masks[(size_t)i], sky, 1);
			if(need > in->remaining()) return;
			const uint8_t* payload = in->data() + in->readPos;
			this->applyChunk(cxs[(size_t)i], czs[(size_t)i], masks[(size_t)i], 1, sky, payload, need);
			in->skip(need);
		}
		return;
	}
	case 0x27: { // Explosion
		float x = in->readFloat();
		float y = in->readFloat();
		float z = in->readFloat();
		float radius = in->readFloat();
		int32_t count = in->readInt();
		if(in->failed() || count < 0) return;
		for(int32_t i = 0; i < count; ++i) {
			int32_t ox = in->readSByte();
			int32_t oy = in->readSByte();
			int32_t oz = in->readSByte();
			if(in->failed()) return;
			// The server has already removed these; mirror it locally.
			int32_t bx = (int32_t)x + ox;
			int32_t by = JavaY::toM8((int32_t)y + oy);
			int32_t bz = (int32_t)z + oz;
			if(LevelHeight::inRange(by)) {
				this->level->setTileAndData(bx, by, bz, 0, 0, 3);
			}
		}
		in->readFloat(); in->readFloat(); in->readFloat(); // player motion
		this->level->levelEvent(2001, (int32_t)x, (int32_t)JavaY::toM8d(y), (int32_t)z, (int32_t)radius);
		return;
	}
	case 0x28: { // Effect
		int32_t effectId = in->readInt();
		int32_t x = 0, y = 0, z = 0;
		in->readPosition(&x, &y, &z);
		in->readInt();
		in->readBool();
		if(in->failed()) return;
		(void)effectId;
		return;
	}
	case 0x29: { // Sound Effect
		std::string name = in->readString();
		int32_t x = in->readInt();
		int32_t y = in->readInt();
		int32_t z = in->readInt();
		float volume = in->readFloat();
		int32_t pitch = in->readByte();
		if(in->failed()) return;
		this->level->playSound((float)x / 8.0f, (float)JavaY::toM8d((double)y / 8.0), (float)z / 8.0f,
		                       name, volume, (float)pitch / 63.0f);
		return;
	}
	case 0x2B: { // Change Game State
		int32_t reason = in->readByte();
		float value = in->readFloat();
		if(in->failed()) return;
		if(reason == 3) {
			this->gamemode = (int32_t)value;
			javaLog("server changed gamemode to %d", (int)this->gamemode);
			this->applyGameMode();
		} else if(reason == 4) {
			/*
			 * "Enter credits", sent by EntityPlayerMP::travelToDimension when you
			 * step into the exit portal after the dragon is dead.  The server has
			 * already taken the player entity out of the world at this point and
			 * is waiting for a Client Status before it will put one back, so
			 * ignoring this leaves us floating in a level nobody is ticking.
			 *
			 * Java answers with GuiWinGame, and closing that screen sends exactly
			 * the packet below.  m8 has no credits screen and therefore nothing to
			 * press to leave one, so skip straight to the part that matters.
			 */
			javaLog("server sent enter-credits, auto-skipping");
			JavaByteBuf status;
			this->beginPacket(&status, 0x16); // Client Status
			status.writeVarInt(0);            // perform respawn
			this->send(status);
			if(this->minecraft) {
				this->minecraft->gui.addMessage("", "You beat the dragon. Credits skipped.", 200);
			}
		}
		return;
	}
	case 0x2F: { // Set Slot
		int32_t window = in->readSByte();
		int32_t slot = in->readShort();
		std::vector<uint8_t> raw;
		int32_t itemId = 0, count = 0, damage = 0;
		if(!this->readSlot(in, &itemId, &count, &damage, &raw)) return;
		if(window != 0) return;
		javaDebug("in set slot %d -> java item %d x%d dmg %d", slot, itemId, count, damage);
		this->applyInventorySlot(slot, itemId, count, damage, raw);
		return;
	}
	case 0x30: { // Window Items
		int32_t window = in->readByte();
		int32_t count = in->readShort();
		if(in->failed() || count < 0) return;
		for(int32_t i = 0; i < count; ++i) {
			std::vector<uint8_t> raw;
			int32_t itemId = 0, cnt = 0, damage = 0;
			if(!this->readSlot(in, &itemId, &cnt, &damage, &raw)) return;
			if(window == 0) this->applyInventorySlot(i, itemId, cnt, damage, raw);
		}
		return;
	}
	case 0x33: { // Update Sign
		int32_t x = 0, y = 0, z = 0;
		in->readPosition(&x, &y, &z);
		std::string lines[4];
		for(int32_t i = 0; i < 4; ++i) {
			lines[i] = javaChatToText(in->readString(384));
		}
		if(in->failed()) return;
		this->applySignText(x, y, z, lines);
		return;
	}
	case 0x35: { // Update Block Entity
		int32_t x = 0, y = 0, z = 0;
		int32_t action = 0;
		in->readPosition(&x, &y, &z);
		action = in->readByte();
		/*
		 * Action 9 is a sign, and a few server implementations answer a chunk
		 * load with this instead of Update Sign.  The payload is a TileEntitySign
		 * tag whose Text1..Text4 are chat JSON, exactly what 0x33 carries.
		 */
		if(action == 9) {
			std::string lines[4];
			if(this->readSignNbt(in, lines)) {
				for(int32_t i = 0; i < 4; ++i) {
					lines[i] = javaChatToText(lines[i]);
				}
				this->applySignText(x, y, z, lines);
			}
			return;
		}
		this->skipNbt(in);
		return;
	}
	case 0x38: { // Player List Item
		int32_t action = in->readVarInt();
		int32_t count = in->readVarInt();
		if(in->failed() || count < 0) return;
		for(int32_t i = 0; i < count; ++i) {
			uint64_t hi = 0, lo = 0;
			in->readUUID(&hi, &lo);
			if(in->failed()) return;
			if(action == 0) { // add player
				std::string name = in->readString(16);
				int32_t props = in->readVarInt();
				if(in->failed() || props < 0) return;
				for(int32_t p = 0; p < props; ++p) {
					in->readString();
					in->readString();
					if(in->readBool()) in->readString();
					if(in->failed()) return;
				}
				in->readVarInt(); // gamemode
				in->readVarInt(); // ping
				if(in->readBool()) in->readString(); // display name
				this->playerNames[lo] = name;
			} else if(action == 1) {
				in->readVarInt();
			} else if(action == 2) {
				in->readVarInt();
			} else if(action == 3) {
				if(in->readBool()) in->readString();
			} else if(action == 4) {
				this->playerNames.erase(lo);
			}
			if(in->failed()) return;
		}
		return;
	}
	case 0x39: { // Player Abilities
		int32_t flags = in->readByte();
		in->readFloat();
		in->readFloat();
		if(in->failed()) return;
		this->abilityFlags = flags;
		this->hasAbilityFlags = 1;
		javaDebug("in abilities flags=0x%02X (invuln=%d fly=%d mayfly=%d instabuild=%d)",
		          flags, (int)((flags & 1) != 0), (int)((flags & 2) != 0),
		          (int)((flags & 4) != 0), (int)((flags & 8) != 0));
		this->applyAbilities();
		return;
	}
	default:
		// Anything else is either cosmetic (scoreboards, particles, maps) or
		// something m8 has no equivalent for.  Dropping it is safe because the
		// frame boundary came from the length prefix, not from parsing.  Say so
		// under M8_JAVA_DEBUG though: when a server is waiting on an answer we
		// do not know how to give, this line is the only clue.
		javaDebug("in unhandled packet 0x%02X (%d bytes)", (int)id,
		          (int)(in->bytes.size() - in->readPos));
		return;
	}
}

// --------------------------------------------------------------- world ------

/*
 * Java's four game modes against m8's two.
 *
 * Survival (0) and adventure (2) both mean "no flying, no free blocks"; the
 * difference between them is what you are allowed to break, which the server
 * decides anyway.  Spectator (3) has no equivalent at all, and creative is the
 * closest thing on offer.  The old test - anything but survival is creative -
 * handed a fly button to every adventure lobby, and a lobby is exactly where a
 * server is most likely to be watching how the client falls.
 */
bool_t JavaSession::isCreativeGamemode() const {
	return this->gamemode == 1 || this->gamemode == 3;
}

void JavaSession::applyGameMode() {
	bool_t creative = this->isCreativeGamemode();
	if(this->minecraft) {
		this->minecraft->setIsCreativeMode(creative);
	}
	if(!this->player) {
		return;
	}
	/*
	 * setIsCreativeMode runs GameMode::initAbilities for us, but only when a
	 * player already exists, and CreativeMode::initAbilities deliberately leaves
	 * abilities.flying alone.  Spelling all four out here means a gamemode
	 * change can never leave a stale ability behind, whichever direction it
	 * goes in.
	 */
	Abilities& ab = this->player->abilities;
	ab.mayfly = creative;
	ab.instabuild = creative;
	ab.invulnerable = creative;
	ab.flying = this->gamemode == 3;
	// Whatever the server said last still wins over the defaults.
	this->applyAbilities();
}

void JavaSession::applyAbilities() {
	if(!this->player || !this->hasAbilityFlags) {
		return;
	}
	Abilities& ab = this->player->abilities;
	ab.invulnerable = (this->abilityFlags & 0x01) != 0;
	ab.flying = (this->abilityFlags & 0x02) != 0;
	ab.mayfly = (this->abilityFlags & 0x04) != 0;
	ab.instabuild = (this->abilityFlags & 0x08) != 0;
}

void JavaSession::createLevel() {
	if(this->levelRequested || !this->minecraft) {
		return;
	}
	this->levelRequested = 1;

	LevelStorageSource* source = this->minecraft->getLevelSource();
	source->deleteLevel(LevelStorageSource::TempLevelId);

	// generatorType/generatorVersion 1 == infinite, which is what a Java world
	// looks like from m8's side.
	LevelSettings settings;
	settings.seed = 0;
	settings.gameType = this->isCreativeGamemode();
	settings.generatorType = 1;

	/*
	 * The one place in the game that asks for a tall world.  This has to happen
	 * before the level exists, because every chunk it ever creates is allocated
	 * and indexed for the height that was set here, and it is safe here because
	 * Minecraft::leaveGame has already torn down whatever level came before.  The
	 * level is a temp one that is never written back, so no save is at risk.
	 */
	LevelHeight::set(JAVA_HEIGHT);

	MultiPlayerLevel* lvl = new MultiPlayerLevel(
		source->selectLevel(LevelStorageSource::TempLevelId, 1), "java", settings, 1, 1, 0);

	/*
	 * Level::_init runs from the base constructor, so the virtual call it makes
	 * lands on Level::createChunkSource and hands us a real terrain generating
	 * ChunkCache regardless of what MultiPlayerLevel overrides.  Swap it for the
	 * server authoritative source now, before Minecraft::setLevel starts the
	 * prepareLevel worker: that worker sweeps the origin with getTile, and with
	 * the cache still installed it would generate a whole local world we would
	 * then immediately overwrite.
	 */
	JavaChunkSource* jcs = new JavaChunkSource(lvl);
	ChunkSource* old = lvl->chunkSource;
	lvl->chunkSource = jcs;
	if(old) {
		delete old;
	}
	this->chunkSource = jcs;

	/*
	 * field_B64 is "this level had no saved data", and prepareLevel reads it as
	 * "generate a spawn point and flood the light engine".  Both are the local
	 * generator's job, and setInitialSpawn spins until Dimension::isValidSpawn
	 * likes a column - which it never will on a world the server has not sent
	 * yet.  The server tells us where we spawn, so clear it.
	 */
	lvl->field_B64 = 0;

	/*
	 * LevelData's constructor freezes the sky at tick 5000 for any gameType but
	 * survival - that is how 0.8.1 spells "creative worlds are always morning" -
	 * and Level::getTimeOfDay honours stopTime over the clock, so no Time Update
	 * could ever move the sun.  The server owns the clock on a Java world; the
	 * 0x03 handler puts stopTime back if the server says the cycle is off.
	 */
	lvl->levelData.setStopTime(-1);
	lvl->adventureSettings.daylightCycle = 1;

	LocalPlayer* p = new LocalPlayer(this->minecraft, lvl, this->minecraft->user,
	                                 lvl->dimensionPtr->id, this->isCreativeGamemode());
	p->entityId = this->serverEid;
	p->inventoryMenu.setListener(p);
	p->username = this->username;
	p->moveTo((float)this->posX, (float)JavaY::toM8d(this->posY), (float)this->posZ,
	          this->rotYaw, this->rotPitch);

	this->level = lvl;
	this->sentX = this->posX;
	this->sentY = this->posY;
	this->sentZ = this->posZ;
	this->sentYaw = this->rotYaw;
	this->sentPitch = this->rotPitch;
	this->everSentPosition = 1;

	this->minecraft->setIsCreativeMode(this->isCreativeGamemode());
	this->minecraft->setLevel(lvl, "JavaSession -> setLevel", p);

	// Player::resetPos, which setLevel runs, zeroes the pitch.
	p->pitch = this->rotPitch;
	p->prevPitch = this->rotPitch;
	javaLog("level built, spawning at %.2f %.2f %.2f (java y %.2f)", this->posX,
	        JavaY::toM8d(this->posY), this->posZ, this->posY);
}

/*
 * Undo a dimension, without leaving anything behind that used to live in it.
 *
 * Respawn (0x07) means "forget the world": every column and every entity the
 * server described is gone, and the ids it used for them are free again the
 * moment it says so.  Clearing knownEntities on its own is not enough - the
 * Entity objects would stay in the level ticking against terrain that no longer
 * exists, and the first mob the new dimension spawns would very likely arrive
 * on a recycled id whose old owner is still sitting in Level::eid2entity.  That
 * is exactly the collision retireEntity exists to prevent, so every entity goes
 * out through destroyEntity, which parks it on a private id first.
 *
 * destroyEntity erases from knownEntities as it goes, so the walk is over a
 * copy of the keys rather than the map itself.
 */
void JavaSession::resetForRespawn() {
	if(this->level) {
		std::vector<int32_t> stale;
		stale.reserve(this->knownEntities.size());
		for(std::map<int32_t, bool_t>::const_iterator it = this->knownEntities.begin();
		    it != this->knownEntities.end(); ++it) {
			stale.push_back(it->first);
		}
		for(size_t i = 0; i < stale.size(); ++i) {
			this->destroyEntity(stale[i]);
		}
	}
	this->knownEntities.clear();
	this->pendingItems.clear();

	// The columns are the server's to resend; dropping them also drops the
	// render chunks, so nothing from the old dimension is left on screen.
	if(this->chunkSource) {
		this->chunkSource->dropAll();
	}

	/*
	 * Two seconds of pinning at 20 frames a second.  It is a ceiling, not a
	 * wait: holdPlayerForTerrain lets go as soon as the column under the player
	 * has something in it, which on a local server is the very next frame.
	 */
	this->respawnHold = 40;
	javaLog("respawn: dimension %d, holding player until terrain arrives", (int)this->dimension);
}

/*
 * Keep the player where the server put them until there is a floor to stand on.
 *
 * pump() runs from RakNetInstance::runEvents, which Minecraft::update calls
 * before Level::tick, so this is the last word on the player's position each
 * frame and gravity gets at most one tick of it before being zeroed again.
 * Pinning rather than freezing matters because the player is still a live
 * entity: motion and fall distance both have to be cleared, or the fall damage
 * banked while the world was blank would land the moment the hold ends.
 */
void JavaSession::holdPlayerForTerrain() {
	if(this->respawnHold <= 0) {
		return;
	}
	LocalPlayer* p = this->player;
	if(!p) {
		this->respawnHold = 0;
		return;
	}

	--this->respawnHold;

	// Real terrain under our feet means the wait is over.  Mth::floor takes a
	// float and these are doubles, so the floor is done here.
	bool_t haveTerrain = 0;
	if(this->chunkSource) {
		LevelChunk* c = this->chunkSource->find(
			(int32_t)floor(this->posX) >> 4, (int32_t)floor(this->posZ) >> 4);
		if(c && c->topBlockY > 1) {
			this->respawnHold = 0;
			haveTerrain = 1;
		}
	}

	p->moveTo((float)this->posX, (float)JavaY::toM8d(this->posY), (float)this->posZ,
	          p->yaw, p->pitch);
	p->motionX = 0.0;
	p->motionY = 0.0;
	p->motionZ = 0.0;
	p->fallDistance = 0.0f;
	/*
	 * Standing on nothing is not standing.  What the hold is for is keeping the
	 * fall damage banked over a blank world from ever landing, and zeroing the
	 * motion and the fall distance is what does that; claiming onGround as well
	 * only changes what playerTick reports, and a client that says it is on the
	 * ground while hanging in mid air over a void is a bot as far as any
	 * verification lobby is concerned.
	 */
	p->onGround = haveTerrain;
}

void JavaSession::applyChunk(int32_t cx, int32_t cz, int32_t mask, bool_t groundUp,
                             bool_t skyLight, const uint8_t* data, size_t len) {
	if(!this->level || !this->chunkSource) {
		return;
	}
	LevelChunk* chunk = this->chunkSource->obtain(cx, cz);
	if(!chunk) {
		return;
	}
	if(!JavaChunkData::apply(this->level, chunk, data, len, mask, groundUp, skyLight)) {
		return;
	}
	// The column came from the server, so it is not ours to write back, and the
	// tile updates the bulk write skipped must reach the renderer in one go.
	chunk->unsaved = 0;
	chunk->clearUpdateMap();
	int32_t x0 = cx << 4;
	int32_t z0 = cz << 4;
	this->level->setTilesDirty(x0, 0, z0, x0 + 15, LevelHeight::maxY(), z0 + 15);
}

void JavaSession::setJavaBlock(int32_t jx, int32_t jy, int32_t jz, int32_t state) {
	if(!this->level) {
		return;
	}
	int32_t my = JavaY::toM8(jy);
	if(!LevelHeight::inRange(my)) {
		return;
	}
	int32_t id = 0;
	int32_t meta = 0;
	if(state > 0) {
		JavaIdMap::javaBlockToM8(state >> 4, state & 0xF, &id, &meta);
	}
	this->level->setTileAndData(jx, my, jz, id, meta, 3);
}

/*
 * A tile entity that has been marked removed is normally freed by Level::tick,
 * which sweeps `tileEntities` and erases the matching chunk entry.  A client
 * level never runs Level::tick - MultiPlayerLevel::tick only advances time and
 * the sky - so a corpse sits in the chunk map forever, and because
 * LevelChunk::getTileEntity answers 0 for a removed entry without replacing it,
 * the position can never hold a live sign again.  Re-placing a sign is exactly
 * the case that leaves one behind: LevelChunk::setTileAndData calls onRemove
 * whenever the metadata changes, even when the block id does not.
 */
static void javaBurySignCorpse(Level* level, int32_t x, int32_t y, int32_t z) {
	if(!level) {
		return;
	}
	for(size_t i = 0; i < level->tileEntities.size();) {
		TileEntity* te = level->tileEntities[i];
		if(te && te->isRemoved() && te->posX == x && te->posY == y && te->posZ == z) {
			level->tileEntities.erase(level->tileEntities.begin() + i);
			for(size_t j = 0; j < level->field_50.size();) {
				if(level->field_50[j] == te) {
					level->field_50.erase(level->field_50.begin() + j);
				} else {
					++j;
				}
			}
			LevelChunk* chunk = level->getChunk(x >> 4, z >> 4);
			if(chunk) {
				chunk->removeTileEntity(x & 0xF, y, z & 0xF);
			}
			delete te;
		} else {
			++i;
		}
	}
}

void JavaSession::applySignText(int32_t jx, int32_t jy, int32_t jz, const std::string* lines) {
	if(!this->level || !JavaY::inRange(jy)) {
		return;
	}
	int32_t my = JavaY::toM8(jy);
	if(!LevelHeight::inRange(my)) {
		return;
	}

	int32_t id = this->level->getTile(jx, my, jz);
	int32_t standing = Tile::sign ? Tile::sign->blockID : 63;
	int32_t wall = Tile::wallSign ? Tile::wallSign->blockID : 68;
	if(id != standing && id != wall) {
		// The block is not here (yet).  Nothing to hang the text on, and
		// inventing a tile entity for whatever *is* here would only leak.
		return;
	}

	javaBurySignCorpse(this->level, jx, my, jz);

	/*
	 * getTileEntity does the creation for us: LevelChunk::getTileEntity sees an
	 * entity tile with no entry, asks the tile for a fresh one and registers it
	 * through Level::setTileEntity, which is what puts it in the list
	 * LevelRenderer walks.  That last part is the whole point - a sign that is
	 * not in level->tileEntities is drawn by nobody.
	 */
	TileEntity* te = this->level->getTileEntity(jx, my, jz);
	if(!te || te->type != 4) {
		return;
	}
	SignTileEntity* sign = (SignTileEntity*)te;

	bool_t incomingEmpty = 1;
	bool_t localHasText = 0;
	for(int32_t i = 0; i < 4; ++i) {
		if(!lines[i].empty()) incomingEmpty = 0;
		if(!sign->textLines[i].empty()) localHasText = 1;
	}
	/*
	 * Placing a sign makes the server answer twice: Sign Editor Open, which is
	 * what makes the sign ours to type into, and an Update Sign carrying four
	 * empty lines because the board really is blank at that moment.  m8 has the
	 * edit screen up by then and the player may already be typing, so an empty
	 * update over text we hold locally is that echo, never an edit - 1.8 has no
	 * way to blank a sign that already has text.
	 */
	if(incomingEmpty && localHasText) {
		return;
	}

	for(int32_t i = 0; i < 4; ++i) {
		std::string line = lines[i];
		if(Util::utf8len(line) > 15) {
			line = Util::utf8substring(line, 0, 15);
		}
		sign->textLines[i] = line;
	}
	// Same as SignTileEntity::load: text that came from outside is not ours to
	// retype, which matches 1.8 - only the player who placed a sign may edit it.
	sign->editable = 0;
	this->level->setTileDirty(jx, my, jz);
}

/*
 * Java window 0 slot numbering: 0 crafting result, 1-4 crafting grid, 5-8 armor
 * (helmet first), 9-35 the backpack, 36-44 the hotbar.  m8 keeps its 36 storage
 * slots at container indices 9-44 and points the nine linkedSlots at whichever
 * of those the hotbar shows, so put the hotbar first (9-17) and link straight to
 * it, then the backpack behind it (18-44).
 */
void JavaSession::applyInventorySlot(int32_t javaSlot, int32_t itemId, int32_t count,
                                     int32_t damage, const std::vector<uint8_t>& raw) {
	if(!this->player) {
		return;
	}
	if(javaSlot >= 5 && javaSlot <= 8) {
		int32_t piece = javaSlot - 5;
		if(itemId > 0) {
			ItemInstance ii(JavaIdMap::javaItemToM8(itemId), count > 0 ? count : 1, damage);
			this->player->setArmor(piece, &ii);
		} else {
			this->player->setArmor(piece, 0);
		}
		return;
	}

	int32_t index;
	if(javaSlot >= 36 && javaSlot <= 44) {
		index = 9 + (javaSlot - 36);
		this->hotbar[javaSlot - 36] = raw;
	} else if(javaSlot >= 9 && javaSlot <= 35) {
		index = 18 + (javaSlot - 9);
	} else {
		return; // crafting grid and result have no m8 equivalent
	}

	Inventory* inv = this->player->inventory;
	if(!inv) {
		return;
	}
	if((int32_t)inv->items.size() < 45) {
		inv->items.resize(45, 0);
	}
	if(itemId > 0) {
		int32_t m8Id = JavaIdMap::javaItemToM8(itemId);
		if(m8Id > 0) {
			ItemInstance ii(m8Id, count > 0 ? count : 1, damage);
			inv->setItem(index, &ii);
		} else {
			inv->setItem(index, 0);
		}
	} else if(this->gamemode != 1) {
		inv->setItem(index, 0);
	}
	/*
	 * In creative the client is the author of its own inventory: m8 starts with
	 * a filled hotbar and the palette screen writes straight into it, while a
	 * Java server's creative window is empty until a Creative Inventory Action
	 * tells it otherwise.  Letting its empty slots through here would wipe the
	 * hotbar the moment Window Items arrives and leave nothing to place, eat or
	 * hold - so an empty creative slot is ignored and pushCreativeInventory()
	 * brings the server up to date instead.
	 */
	if(javaSlot >= 36 && javaSlot <= 44) {
		inv->linkSlot(javaSlot - 36, index);
	}
}

// ------------------------------------------------------------ entities ------

Entity* JavaSession::findEntity(int32_t javaEid) {
	if(!this->level) {
		return 0;
	}
	if(this->player && javaEid == this->player->entityId) {
		return 0; // the server never means "move yourself" with these
	}
	return this->level->getEntity(javaEid);
}

/*
 * Take an entity out of play without ever handing its id to the level twice.
 *
 * Level::tickEntities sweeps a dead entity with
 *
 *     eid2entity.erase(eid2entity.find(entity->entityId))
 *
 * one tick after it dies and deletes it the tick after that, and
 * std::map::erase(end()) is undefined - so the corpse's key has to stay in the
 * table until the sweep, which means it cannot be the key the replacement
 * entity needs.  Re-keying the corpse onto a private negative id satisfies
 * both: the sweep still finds something to erase, and the real id is free
 * immediately.
 */
void JavaSession::retireEntity(Entity* old) {
	MultiPlayerLevel* lvl = this->level;
	if(!lvl || !old || old == (Entity*)this->player) {
		return;
	}

	int32_t oldId = old->entityId;
	std::map<int32_t, Entity*>::iterator it = lvl->eid2entity.find(oldId);
	if(it != lvl->eid2entity.end() && it->second == old) {
		lvl->eid2entity.erase(it);
	}
	int32_t parkId = this->nextParkedEid;
	// Wrapping would collide with a real id; parking stops mattering long
	// before two billion entities have come and gone, so just stop moving.
	if(this->nextParkedEid > -2000000000) {
		--this->nextParkedEid;
	}
	old->entityId = parkId;
	lvl->eid2entity[parkId] = old;

	// A player corpse is only swept when this is set - see Level::tickEntities
	// and ClientSideNetworkHandler's own remove-player path.
	if(old->isPlayer()) {
		old->field_108 = 1;
	}
	lvl->removeEntity(old);
}

/*
 * Register a freshly spawned entity under the id the server gave it.
 *
 * This replaces MultiPlayerLevel::putEntity, which dereferences whatever
 * pointer its own id map happens to hold - fine against an MCPE server that
 * allocates ids monotonically, a use after free against a Java server that
 * recycles them.
 */
void JavaSession::attachEntity(int32_t javaEid, Entity* e) {
	MultiPlayerLevel* lvl = this->level;
	if(!lvl || !e) {
		return;
	}

	Entity* previous = 0;
	std::map<int32_t, Entity*>::iterator mapped = lvl->field_BAC.find(javaEid);
	if(mapped != lvl->field_BAC.end()) {
		Entity* old = mapped->second;
		lvl->field_BAC.erase(mapped);
		lvl->field_BC4.erase(old);
		lvl->field_BDC.erase(old);
		// The level's id table is the only thing that says whether that
		// pointer is still ours to touch: it loses the entry one tick before
		// the entity is deleted, so a mismatch means "already gone".
		if(lvl->getEntity(javaEid) == old) {
			previous = old;
		}
	} else {
		previous = lvl->getEntity(javaEid);
	}
	if(previous && previous != (Entity*)this->player) {
		this->retireEntity(previous);
	}

	e->entityId = javaEid;
	lvl->addEntity(e);
	// Level::addEntity inserts, and std::map::insert leaves an existing value
	// alone, so make the id point at the entity that actually owns it now.
	lvl->eid2entity[javaEid] = e;
	lvl->field_BAC[javaEid] = e;
	lvl->field_BC4.insert(e);
}

void JavaSession::spawnMob(JavaByteBuf* in) {
	int32_t eid = in->readVarInt();
	int32_t type = in->readByte();
	double x = (double)in->readInt() / 32.0;
	double y = (double)in->readInt() / 32.0;
	double z = (double)in->readInt() / 32.0;
	float yaw = (float)in->readSByte() * 360.0f / 256.0f;
	float pitch = (float)in->readSByte() * 360.0f / 256.0f;
	float headYaw = (float)in->readSByte() * 360.0f / 256.0f;
	int32_t vx = in->readShort();
	int32_t vy = in->readShort();
	int32_t vz = in->readShort();
	if(in->failed()) {
		return;
	}
	this->readMetadata(in, 0, 0, 0);

	int32_t m8Type = JavaIdMap::javaMobToM8(type);
	if(!m8Type || !this->level) {
		return;
	}
	Mob* mob = MobFactory::CreateMob(m8Type, this->level);
	if(!mob) {
		return;
	}
	// Mob::moveTo takes feet level Y, which is what Java sends.
	mob->moveTo((float)x, (float)JavaY::toM8d(y), (float)z, yaw, pitch);
	mob->headYaw = headYaw;
	mob->prevHeadYaw = headYaw;
	mob->lerpMotion((float)vx / 8000.0f, (float)vy / 8000.0f, (float)vz / 8000.0f);
	this->attachEntity(eid, mob);
	this->knownEntities[eid] = 1;
}

void JavaSession::spawnObject(JavaByteBuf* in) {
	int32_t eid = in->readVarInt();
	int32_t type = in->readByte();
	double x = (double)in->readInt() / 32.0;
	double y = (double)in->readInt() / 32.0;
	double z = (double)in->readInt() / 32.0;
	float pitch = (float)in->readSByte() * 360.0f / 256.0f;
	float yaw = (float)in->readSByte() * 360.0f / 256.0f;
	int32_t data = in->readInt();
	int32_t vx = 0, vy = 0, vz = 0;
	if(data > 0) {
		vx = in->readShort();
		vy = in->readShort();
		vz = in->readShort();
	}
	if(in->failed() || !this->level) {
		return;
	}

	if(type == 2) {
		// Dropped item.  The stack itself arrives in a later Entity Metadata,
		// so start from an empty instance and remember to fill it in.
		ItemEntity* item = new ItemEntity(this->level, (float)x, (float)JavaY::toM8d(y), (float)z,
		                                 ItemInstance(Item::stick ? Item::stick->itemID : 280, 1, 0));
		item->lerpMotion((float)vx / 8000.0f, (float)vy / 8000.0f, (float)vz / 8000.0f);
		this->attachEntity(eid, item);
		this->knownEntities[eid] = 1;
		this->pendingItems[eid] = 1;
		return;
	}

	int32_t m8Type = JavaIdMap::javaObjectToM8(type);
	if(!m8Type) {
		return;
	}
	Entity* e = EntityFactory::CreateEntity(m8Type, this->level);
	if(!e) {
		return;
	}
	if(e->isMob()) {
		((Mob*)e)->moveTo((float)x, (float)JavaY::toM8d(y), (float)z, yaw, pitch);
	} else {
		e->moveTo((float)x, (float)JavaY::toM8d(y), (float)z, yaw, pitch);
	}
	if(type == 70) {
		// Falling block: the data field is Block.getStateId(), which is
		// id | (meta << 12).  FallingTile::tick() removes itself outright when
		// blockID is 0, so an id m8 has no tile for falls back to sand rather
		// than winking out of existence the tick after it spawns.
		int32_t id = 0, meta = 0;
		if(data > 0) {
			JavaIdMap::javaBlockToM8(data & 0xFFF, (data >> 12) & 0xF, &id, &meta);
		}
		if(!id) {
			id = Tile::sand ? Tile::sand->blockID : 12;
			meta = 0;
		}
		((FallingTile*)e)->blockID = id;
		((FallingTile*)e)->blockMetaMaybe = meta;
	}
	e->lerpMotion((float)vx / 8000.0f, (float)vy / 8000.0f, (float)vz / 8000.0f);
	this->attachEntity(eid, e);
	this->knownEntities[eid] = 1;
}

void JavaSession::spawnPlayer(JavaByteBuf* in) {
	int32_t eid = in->readVarInt();
	uint64_t hi = 0, lo = 0;
	in->readUUID(&hi, &lo);
	double x = (double)in->readInt() / 32.0;
	double y = (double)in->readInt() / 32.0;
	double z = (double)in->readInt() / 32.0;
	float yaw = (float)in->readSByte() * 360.0f / 256.0f;
	float pitch = (float)in->readSByte() * 360.0f / 256.0f;
	int32_t heldItem = in->readShort();
	if(in->failed() || !this->level || !this->minecraft) {
		return;
	}
	this->readMetadata(in, 0, 0, 0);

	RemotePlayer* p = new RemotePlayer(this->level, this->minecraft->isCreativeMode());
	this->minecraft->gameMode->initAbilities(p->abilities);
	p->moveTo((float)x, (float)JavaY::toM8d(y), (float)z, yaw, pitch);
	p->headYaw = yaw;
	p->prevHeadYaw = yaw;

	std::string name;
	std::map<uint64_t, std::string>::iterator it = this->playerNames.find(lo);
	if(it != this->playerNames.end()) {
		name = it->second;
	}
	p->username = name;

	if(p->inventory) {
		if(heldItem > 0) {
			ItemInstance ii(JavaIdMap::javaItemToM8(heldItem), 1, 0);
			p->inventory->replaceSlot(9, &ii);
		} else {
			p->inventory->clearSlot(9);
		}
		p->inventory->moveToSelectedSlot(9);
	}
	this->attachEntity(eid, p);
	this->knownEntities[eid] = 1;
}

void JavaSession::moveEntity(int32_t javaEid, double dx, double dy, double dz,
                             bool_t hasRot, float yaw, float pitch, bool_t absolute) {
	Entity* e = this->findEntity(javaEid);
	if(!e) {
		return;
	}
	if(!hasRot) {
		yaw = e->yaw;
		pitch = e->pitch;
	}

	/*
	 * lerpTo's Y argument is not the same thing on both sides of the hierarchy:
	 * Mob::lerpTo adds ridingHeight itself (so it wants feet level) while
	 * Entity::lerpTo assigns posY straight through (so it wants eye/centre
	 * level).  Feed each the one it expects.
	 */
	bool_t isMob = e->isMob();
	double baseY = isMob ? (double)(e->posY - e->ridingHeight) : (double)e->posY;
	double targetY;
	if(absolute) {
		targetY = JavaY::toM8d(dy);
		if(!isMob) {
			targetY += (double)e->ridingHeight;
		}
	} else {
		targetY = baseY + dy;
	}
	double targetX = absolute ? dx : (double)e->posX + dx;
	double targetZ = absolute ? dz : (double)e->posZ + dz;

	e->lerpTo((float)targetX, (float)targetY, (float)targetZ, yaw, pitch, 3);
}

void JavaSession::destroyEntity(int32_t javaEid) {
	MultiPlayerLevel* lvl = this->level;
	if(!lvl) {
		return;
	}
	this->knownEntities.erase(javaEid);
	this->pendingItems.erase(javaEid);

	Entity* e = 0;
	std::map<int32_t, Entity*>::iterator mapped = lvl->field_BAC.find(javaEid);
	if(mapped != lvl->field_BAC.end()) {
		Entity* old = mapped->second;
		lvl->field_BAC.erase(mapped);
		lvl->field_BC4.erase(old);
		lvl->field_BDC.erase(old);
		if(lvl->getEntity(javaEid) == old) {
			e = old;
		}
	} else {
		e = lvl->getEntity(javaEid);
	}
	if(!e || e == (Entity*)this->player) {
		return;
	}
	this->retireEntity(e);
}

// -------------------------------------------------------- wire helpers ------

static bool_t javaSkipNbtPayload(JavaByteBuf* in, int32_t type, int32_t depth) {
	if(depth > 32 || in->failed()) {
		return 0;
	}
	switch(type) {
	case 0: return 1;
	case 1: in->skip(1); return !in->failed();
	case 2: in->skip(2); return !in->failed();
	case 3: case 5: in->skip(4); return !in->failed();
	case 4: case 6: in->skip(8); return !in->failed();
	case 7: { // byte array
		int32_t len = in->readInt();
		if(in->failed() || len < 0) return 0;
		in->skip((size_t)len);
		return !in->failed();
	}
	case 8: { // string
		int32_t len = in->readUShort();
		if(in->failed()) return 0;
		in->skip((size_t)len);
		return !in->failed();
	}
	case 9: { // list
		int32_t elem = in->readByte();
		int32_t len = in->readInt();
		if(in->failed() || len < 0) return 0;
		for(int32_t i = 0; i < len; ++i) {
			if(!javaSkipNbtPayload(in, elem, depth + 1)) return 0;
		}
		return 1;
	}
	case 10: { // compound
		while(1) {
			int32_t t = in->readByte();
			if(in->failed()) return 0;
			if(t == 0) return 1;
			int32_t nameLen = in->readUShort();
			if(in->failed()) return 0;
			in->skip((size_t)nameLen);
			if(!javaSkipNbtPayload(in, t, depth + 1)) return 0;
		}
	}
	case 11: { // int array
		int32_t len = in->readInt();
		if(in->failed() || len < 0) return 0;
		in->skip((size_t)len * 4);
		return !in->failed();
	}
	default:
		return 0;
	}
}

/*
 * Java's NBT is big endian and m8's is little endian, so CompoundTag cannot read
 * a tile entity blob off the wire.  Nothing here needs the whole tree either -
 * only the four strings - so this walks the tag the same way javaSkipNbtPayload
 * does and keeps the names it recognises.
 */
static bool_t javaCollectSignText(JavaByteBuf* in, int32_t type, int32_t depth, std::string* lines);

static std::string javaReadNbtString(JavaByteBuf* in) {
	int32_t len = in->readUShort();
	if(in->failed() || len < 0) {
		return std::string();
	}
	std::string out;
	out.resize((size_t)len);
	if(len > 0) {
		in->readBytes((uint8_t*)&out[0], (size_t)len);
	}
	if(in->failed()) {
		return std::string();
	}
	return out;
}

static bool_t javaCollectSignText(JavaByteBuf* in, int32_t type, int32_t depth, std::string* lines) {
	if(depth > 32 || in->failed()) {
		return 0;
	}
	if(type != 10) {
		return javaSkipNbtPayload(in, type, depth);
	}
	while(1) {
		int32_t t = in->readByte();
		if(in->failed()) return 0;
		if(t == 0) return 1;
		std::string name = javaReadNbtString(in);
		if(in->failed()) return 0;
		int32_t slot = -1;
		if(t == 8 && name.size() == 5 && name.compare(0, 4, "Text") == 0
		   && name[4] >= '1' && name[4] <= '4') {
			slot = name[4] - '1';
		}
		if(slot >= 0) {
			lines[slot] = javaReadNbtString(in);
			if(in->failed()) return 0;
		} else if(!javaCollectSignText(in, t, depth + 1, lines)) {
			return 0;
		}
	}
}

bool_t JavaSession::readSignNbt(JavaByteBuf* in, std::string* lines) {
	int32_t type = in->readByte();
	if(in->failed() || type != 10) {
		return 0;
	}
	int32_t nameLen = in->readUShort();
	if(in->failed()) {
		return 0;
	}
	in->skip((size_t)nameLen);
	if(in->failed()) {
		return 0;
	}
	return javaCollectSignText(in, type, 0, lines);
}

void JavaSession::skipNbt(JavaByteBuf* in) {
	int32_t type = in->readByte();
	if(in->failed() || type == 0) {
		return;
	}
	int32_t nameLen = in->readUShort();
	if(in->failed()) {
		return;
	}
	in->skip((size_t)nameLen);
	javaSkipNbtPayload(in, type, 0);
}

bool_t JavaSession::readSlot(JavaByteBuf* in, int32_t* outId, int32_t* outCount, int32_t* outDamage,
                             std::vector<uint8_t>* rawOut) {
	size_t start = in->readPos;
	int32_t id = in->readShort();
	if(in->failed()) {
		return 0;
	}
	int32_t count = 0;
	int32_t damage = 0;
	if(id >= 0) {
		count = in->readByte();
		damage = in->readShort();
		this->skipNbt(in);
		if(in->failed()) {
			return 0;
		}
	} else {
		id = -1;
	}
	if(outId) *outId = id;
	if(outCount) *outCount = count;
	if(outDamage) *outDamage = damage;
	if(rawOut) {
		rawOut->assign(in->data() + start, in->data() + in->readPos);
	}
	return 1;
}

/*
 * Walk a 1.8 DataWatcher blob.  We only actually want one field out of it - the
 * ItemStack a dropped item carries at index 10 - but the whole list has to be
 * consumed correctly because more packet fields can follow it.
 */
bool_t JavaSession::readMetadata(JavaByteBuf* in, int32_t* outItemId, int32_t* outCount,
                                 int32_t* outDamage) {
	while(1) {
		int32_t header = in->readByte();
		if(in->failed()) {
			return 0;
		}
		if(header == 127) {
			return 1;
		}
		int32_t type = (header & 0xE0) >> 5;
		int32_t index = header & 0x1F;
		switch(type) {
		case 0: in->skip(1); break;
		case 1: in->skip(2); break;
		case 2: case 3: in->skip(4); break;
		case 4: in->readString(); break;
		case 5: {
			int32_t id = 0, count = 0, damage = 0;
			if(!this->readSlot(in, &id, &count, &damage, 0)) return 0;
			if(index == 10 && outItemId) {
				*outItemId = id;
				if(outCount) *outCount = count;
				if(outDamage) *outDamage = damage;
			}
			break;
		}
		case 6: in->skip(12); break;
		case 7: in->skip(12); break;
		default:
			return 0;
		}
		if(in->failed()) {
			return 0;
		}
	}
}

// ------------------------------------------------------- outbound helpers ---

void JavaSession::sendKeepAlive(int32_t id) {
	JavaByteBuf out;
	this->beginPacket(&out, 0x00);
	out.writeVarInt(id);
	this->send(out);
}

void JavaSession::sendClientSettings() {
	JavaByteBuf out;
	this->beginPacket(&out, 0x15);
	out.writeString("en_US");
	int32_t view = 8;
	if(this->minecraft) {
		view = this->minecraft->options.renderDistance;
		// m8's option is an enum (0 = far .. 3 = tiny); turn it into chunks.
		switch(view) {
		case 0: view = 10; break;
		case 1: view = 8; break;
		case 2: view = 6; break;
		default: view = 4; break;
		}
	}
	out.writeSByte((int8_t)view);
	out.writeSByte(0);   // chat mode: enabled
	out.writeBool(1);    // chat colours
	out.writeByte(0x7F); // every skin layer on
	this->send(out);
	/*
	 * No Client Status here.  It looked like the "I have finished loading"
	 * handshake, but 1.8 has no such thing - the server starts ticking a player
	 * the moment it lets them in - and the only value it can carry at this point
	 * is PERFORM_RESPAWN, which a living player never sends.  A vanilla server
	 * ignores it; a verification front end reasonably treats a packet no client
	 * would send as proof that this is not one.
	 */
}

void JavaSession::sendClientBrand() {
	JavaByteBuf out;
	this->beginPacket(&out, 0x17); // Plugin Message
	out.writeString("MC|Brand");
	/*
	 * ClientBrandRetriever.getClientModName() is the literal "vanilla" in an
	 * unmodified 1.8 client, and on the wire the payload is a length prefixed
	 * string, not raw bytes (1.7 was the other way round).  The capitalised
	 * "Vanilla" some tools send is a known bot signature, so keep it lowercase.
	 */
	out.writeString("vanilla");
	this->send(out);
}

void JavaSession::sendResourcePackStatus(const std::string& hash, int32_t status) {
	JavaByteBuf out;
	this->beginPacket(&out, 0x19);
	out.writeString(hash.size() > 40 ? hash.substr(0, 40) : hash);
	out.writeVarInt(status);
	this->send(out);
}

void JavaSession::sendEntityAction(int32_t action) {
	JavaByteBuf out;
	this->beginPacket(&out, 0x0B);
	out.writeVarInt(this->serverEid);
	out.writeVarInt(action);
	out.writeVarInt(0); // aux data, only used by horse jumps
	this->send(out);
}

/*
 * EntityPlayerSP.onUpdateWalkingPlayer, packet for packet.
 *
 * A Java client reports itself once every tick and never more often than that,
 * and which of the four movement packets it picks depends on what actually
 * changed: position and rotation, position only, rotation only, or - standing
 * perfectly still - the bare "on the ground" flag.  Getting this right is not
 * cosmetic.  The server counts a player's ticks by these packets, so sending
 * thirty a second (which is what a frame driven sender does at sixty frames)
 * looks exactly like a speed hack, and sending none while standing still makes
 * the fall and gravity checks of a verification plugin see a player who is not
 * simulating physics.  Both get you thrown off a public server.
 */
void JavaSession::playerTick() {
	if(this->phase != JavaSession::PLAY || !this->levelLive) {
		return;
	}
	if(!this->player) {
		this->player = this->minecraft ? this->minecraft->player : 0;
		if(!this->player) {
			return;
		}
	}

	// Sneaking and sprinting are state rather than movement: they are reported
	// once, when they change.  Without them the server has us walking upright
	// forever, which costs the sprint speed it grants and the hitbox it shrinks.
	bool_t sprint = this->player->isSprinting ? 1 : 0;
	if(sprint != this->sentSprint) {
		this->sentSprint = sprint;
		this->sendEntityAction(sprint ? 3 : 4); // START_SPRINTING / STOP_SPRINTING
	}
	bool_t sneak = this->player->isSneaking() ? 1 : 0;
	if(sneak != this->sentSneak) {
		this->sentSneak = sneak;
		this->sendEntityAction(sneak ? 0 : 1); // START_SNEAKING / STOP_SNEAKING
	}

	double x = (double)this->player->posX;
	double feetY = (double)this->player->boundingBox.minY; // and not posY: see below
	double z = (double)this->player->posZ;
	float yaw = this->player->yaw;
	float pitch = this->player->pitch;
	bool_t ground = this->player->onGround;

	double dx = x - this->sentX;
	double dy = feetY - this->sentY;
	double dz = z - this->sentZ;
	bool_t moved = !this->everSentPosition
		|| dx * dx + dy * dy + dz * dz > 9.0e-4
		|| this->positionUpdateTicks >= 20;
	bool_t turned = yaw != this->sentYaw || pitch != this->sentPitch;

	int32_t kind = moved && turned ? 3 : (moved ? 1 : (turned ? 2 : 0));
	if(javaDebugEnabled()) {
		int32_t now = getTimeMs();
		++this->moveWindow[kind];
		if(this->moveWindowMs == 0) {
			this->moveWindowMs = now;
		} else if(now - this->moveWindowMs >= 1000) {
			javaDebug("moves/s: %d bare, %d pos, %d look, %d both (%d ms)",
			          (int)this->moveWindow[0], (int)this->moveWindow[1],
			          (int)this->moveWindow[2], (int)this->moveWindow[3],
			          (int)(now - this->moveWindowMs));
			memset(this->moveWindow, 0, sizeof(this->moveWindow));
			this->moveWindowMs = now;
		}
	}

	JavaByteBuf out;
	if(moved && turned) {
		this->beginPacket(&out, 0x06); // Player Position And Look
		out.writeDouble(x);
		out.writeDouble(JavaY::toJavad(feetY));
		out.writeDouble(z);
		out.writeFloat(yaw);
		out.writeFloat(pitch);
	} else if(moved) {
		this->beginPacket(&out, 0x04); // Player Position
		out.writeDouble(x);
		out.writeDouble(JavaY::toJavad(feetY));
		out.writeDouble(z);
	} else if(turned) {
		this->beginPacket(&out, 0x05); // Player Look
		out.writeFloat(yaw);
		out.writeFloat(pitch);
	} else {
		this->beginPacket(&out, 0x03); // Player
	}
	out.writeBool(ground);
	this->send(out);
	this->lastMoveSentMs = getTimeMs();

	/*
	 * How the client falls is the one thing a verification lobby always looks
	 * at, so while the debug log is on every airborne tick is written down.  A
	 * vanilla 1.8 client loses 0.08 blocks a tick to gravity and two percent of
	 * its speed to drag; anything else here is a physics bug worth seeing.
	 */
	if(javaDebugEnabled() && !ground) {
		javaDebug("air y=%.3f dy=%+.4f my=%+.4f fall=%.1f fly=%d mayfly=%d hold=%d",
		          JavaY::toJavad(feetY), dy, (double)this->player->motionY,
		          this->player->fallDistance, (int)this->player->abilities.flying,
		          (int)this->player->abilities.mayfly, (int)this->respawnHold);
	}

	++this->positionUpdateTicks;
	if(moved) {
		this->sentX = x;
		this->sentY = feetY;
		this->sentZ = z;
		this->positionUpdateTicks = 0;
	}
	if(turned) {
		this->sentYaw = yaw;
		this->sentPitch = pitch;
	}
	this->sentGround = ground;
	this->everSentPosition = 1;
}

/*
 * A full position, unconditionally.  playerTick() carries the normal traffic;
 * this is for the moments when the player is not being ticked at all - a screen
 * open over the world, terrain still arriving, the death screen - where the
 * server would otherwise hear nothing for as long as that lasts.
 */
void JavaSession::sendPositionNow(bool_t force) {
	if(this->phase != JavaSession::PLAY || !this->levelLive) {
		return;
	}
	if(!this->player) {
		this->player = this->minecraft ? this->minecraft->player : 0;
		if(!this->player) {
			return;
		}
	}

	/*
	 * Java's posY is the bottom of the collision box.  m8's posY is eye level and,
	 * crucially, it lags the box by ySize: Entity::move() raises boundingBox by
	 * stepHeight when you step onto something, then sets
	 *
	 *     posY = boundingBox.minY + ridingHeight - ySize
	 *
	 * with ySize bumped by 0.5 so the camera glides up over the next few ticks
	 * instead of snapping.  Subtracting only ridingHeight therefore reports the
	 * height we were at *before* the step for as long as ySize is decaying, which
	 * for a slab or a stair is a position squarely inside the block we just
	 * climbed onto.  NetHandlerPlayServer checks exactly that - if the box we
	 * claim collides with terrain and the box we came from did not, it answers
	 * with a teleport back to where we were - so every attempt to walk up a step
	 * was being undone a tick later.  Read the box directly and the two clients
	 * agree; jumps never showed the bug because a jump leaves ySize at zero.
	 */
	double feetY = (double)this->player->boundingBox.minY;
	double x = (double)this->player->posX;
	double z = (double)this->player->posZ;
	float yaw = this->player->yaw;
	float pitch = this->player->pitch;
	bool_t ground = this->player->onGround;

	if(!force && (int32_t)(getTimeMs() - this->lastMoveSentMs) < 50) {
		return;
	}

	JavaByteBuf out;
	this->beginPacket(&out, 0x06); // Player Position And Look
	out.writeDouble(x);
	out.writeDouble(JavaY::toJavad(feetY));
	out.writeDouble(z);
	out.writeFloat(yaw);
	out.writeFloat(pitch);
	out.writeBool(ground);
	this->send(out);
	this->lastMoveSentMs = getTimeMs();

	this->sentX = x;
	this->sentY = feetY;
	this->sentZ = z;
	this->sentYaw = yaw;
	this->sentPitch = pitch;
	this->sentGround = ground;
	this->everSentPosition = 1;
	this->positionUpdateTicks = 0;
}

void JavaSession::sendDigging(int32_t status, int32_t jx, int32_t jy, int32_t jz, int32_t face) {
	JavaByteBuf out;
	this->beginPacket(&out, 0x07);
	out.writeVarInt(status);
	out.writePosition(jx, jy, jz);
	out.writeByte((uint8_t)face);
	this->send(out);
}

void JavaSession::sendHeldSlot(int32_t slot) {
	if(slot < 0 || slot > 8) {
		return;
	}
	this->selectedSlot = slot;
	JavaByteBuf out;
	this->beginPacket(&out, 0x09);
	out.writeShort((int16_t)slot);
	this->send(out);
}

/*
 * Write the stack in the selected hotbar slot in Java's Slot encoding.  When the
 * server told us what is in that slot we echo its bytes back verbatim, so any
 * NBT (enchantments, custom names, ...) survives the round trip.  Otherwise fall
 * back to whatever m8 thinks it is holding.
 */
void JavaSession::writeHeldStack(JavaByteBuf* out) {
	int32_t slot = this->selectedSlot;
	// ... except in creative, where m8 may be holding something the server has
	// not been told about yet; see applyInventorySlot().
	if(this->gamemode != 1 && slot >= 0 && slot <= 8 && this->hotbar[slot].size() >= 2) {
		out->writeBytes(&this->hotbar[slot][0], this->hotbar[slot].size());
		return;
	}
	ItemInstance* held = 0;
	if(this->player) {
		held = this->player->getSelectedItem();
	}
	if(!held || !held->isValid || held->count <= 0) {
		out->writeShort(-1);
		return;
	}
	int32_t id = held->itemClass ? held->itemClass->itemID
	           : (held->tileClass ? held->tileClass->blockID : 0);
	int32_t javaId = JavaIdMap::m8ItemToJava(id);
	if(javaId <= 0) {
		out->writeShort(-1);
		return;
	}
	out->writeShort((int16_t)javaId);
	out->writeByte((uint8_t)held->count);
	out->writeShort((int16_t)held->metadata);
	out->writeByte(0); // no NBT
}
