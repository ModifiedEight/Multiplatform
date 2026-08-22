#pragma once
#include <_types.h>
#include <java/JavaByteBuf.hpp>
#include <java/JavaSocket.hpp>
#include <map>
#include <string>
#include <vector>

struct Minecraft;
struct MultiPlayerLevel;
struct LocalPlayer;
struct Entity;
struct JavaChunkSource;
struct Packet;

/*
 * One connection to a Minecraft Java Edition 1.8.x server, spoken natively.
 *
 * Everything happens on the main thread: pump() is driven from
 * RakNetInstance::runEvents, which Minecraft::update already calls once per
 * frame before ticking, so this sits exactly where the RakNet receive loop
 * would have been and inherits its threading guarantees.
 *
 * The socket is non blocking and nothing here ever waits, so a slow or dead
 * server costs one frame at worst.
 *
 * Only offline mode ("cracked") servers can be joined: implementing the
 * Yggdrasil handshake would mean shipping the player's Mojang credentials
 * through a client that has no account system.  A server that asks for
 * encryption is reported as such instead of failing obscurely.
 */
struct JavaSession {
	enum Phase {
		IDLE = 0,
		CONNECTING = 1,
		LOGIN = 2,
		PLAY = 3,
		CLOSED = 4
	};

	static const int32_t PROTOCOL = 47;
	// A 1.8 server never legitimately sends a frame anywhere near this big.
	static const int32_t MAX_FRAME = 8 * 1024 * 1024;
	// Chunk traffic while the level is still being built has to be kept, not
	// dropped: a Java server sends each column exactly once.
	static const size_t MAX_DEFERRED = 96u * 1024u * 1024u;

	// ---- wire ----
	JavaSocket socket;
	std::vector<uint8_t> inBuf;
	size_t inOff;
	std::vector<uint8_t> outBuf;
	size_t outOff;
	int32_t compressionThreshold; // -1 until the server enables compression
	Phase phase;

	// ---- target ----
	std::string displayName;
	std::string handshakeHost; // what the player typed, for virtual host routing
	std::string ip;            // what we actually connect to
	std::string username;
	int32_t port;

	// ---- error reporting ----
	std::string failure;
	bool_t reportedFailure;

	// ---- engine ----
	Minecraft* minecraft;
	MultiPlayerLevel* level;
	JavaChunkSource* chunkSource;
	LocalPlayer* player;
	bool_t levelRequested;
	bool_t levelLive;

	// ---- join info ----
	int32_t serverEid;
	int32_t gamemode;
	int32_t dimension;
	bool_t hasSkyLight;
	double posX, posY, posZ;
	float rotYaw, rotPitch;
	bool_t havePosition;

	// ---- inbound held back until the level exists ----
	std::vector<std::vector<uint8_t> > deferred;
	size_t deferredBytes;
	bool_t deferredOverflowed;

	// ---- outbound bookkeeping ----
	int32_t tickCounter;
	double sentX, sentY, sentZ;
	float sentYaw, sentPitch;
	bool_t sentGround;
	bool_t everSentPosition;
	/*
	 * Vanilla's EntityPlayerSP.positionUpdateTicks: how many ticks have gone by
	 * without a full position going out.  Twenty of them and one is sent whether
	 * anything moved or not, which is what keeps the server's idea of where we
	 * are from drifting.
	 */
	int32_t positionUpdateTicks;
	bool_t sentSprint;
	bool_t sentSneak;
	// getTimeMs() of the last movement packet, for the idle fallback in pump().
	int32_t lastMoveSentMs;
	/*
	 * A second's worth of movement packets, counted by kind, for the debug log.
	 * A Java client sends exactly twenty a second and a server that measures
	 * anything about how a player moves - a fall, a walk speed, a verification
	 * lobby - is really measuring against that number, so it is worth being able
	 * to see ours.
	 */
	int32_t moveWindowMs;
	int32_t moveWindow[4];

	/*
	 * Java identifies a held item by its own item id, and m8's item ids only
	 * partly line up.  Rather than round trip through the id map (which is lossy
	 * in the outbound direction) the raw Slot bytes the server sent for each
	 * hotbar index are kept verbatim and echoed back in Player Block Placement,
	 * which is what the server compares against.
	 */
	std::vector<uint8_t> hotbar[9];
	int32_t selectedSlot;

	// uuid low 64 bits -> name, filled from Player List Item, used by Spawn Player
	std::map<uint64_t, std::string> playerNames;
	// Java entity ids of dropped items whose ItemStack has not arrived yet
	std::map<int32_t, bool_t> pendingItems;
	// every Java entity id we have spawned locally
	std::map<int32_t, bool_t> knownEntities;
	/*
	 * A Java server hands an entity id straight back out again as soon as the
	 * mob wearing it despawns, while m8 keeps a removed entity alive for a
	 * tick or two before deleting it (Level::tickEntities).  Corpses are
	 * therefore re-keyed onto private ids counting down from -2 so the
	 * level's own id table never has two owners for one id.
	 */
	int32_t nextParkedEid;
	/*
	 * Frames left to hold the player still after a Respawn.
	 *
	 * A dimension change throws away every column, and the replacements arrive
	 * over the next second or so.  Until they do the player is standing over
	 * nothing but the bedrock floor JavaChunkSource lays into a column it knows
	 * nothing about yet, and left alone would free fall the whole way down and
	 * be dragged back up by the server - which is what made the nether portal
	 * look broken.  While this is counting down the player is pinned to the
	 * position the server last gave us; it is cleared early as soon as real
	 * terrain is standing underneath them.
	 */
	int32_t respawnHold;

	/*
	 * The last Player Abilities flags the server sent, kept because it always
	 * sends them before the level exists.  NetHandlerPlayClient can apply them
	 * straight away - its player is made the instant Join Game lands - while m8
	 * builds the level on a worker thread and has no player to write to for the
	 * best part of a second, so the packet used to be read and thrown away.
	 * That is how a survival player ended up able to fly: nothing ever told the
	 * client what the server actually allows.
	 */
	int32_t abilityFlags;
	bool_t hasAbilityFlags;

	JavaSession();
	~JavaSession();

	bool_t begin(Minecraft* mc, const std::string& name, const std::string& host,
	             int32_t typedPort, const std::string& user);
	void pump();
	void shutdown();
	bool_t isActive() const;

	// Returns 1 when the MCPE packet was consumed (translated or deliberately
	// dropped) and must not reach RakNet.
	bool_t interceptSend(Packet* pk);
	// Chat text straight from the chat screen, '/' commands included.
	void sendChatLine(const std::string& text);

	// ---- wire plumbing ----
	void fail(const std::string& why);
	void flushOut();
	void pumpSocket();
	void dispatchFrames();
	void send(const JavaByteBuf& payload);
	void beginPacket(JavaByteBuf* out, int32_t id);
	bool_t decodeFrame(const uint8_t* raw, size_t rawLen, JavaByteBuf* out);
	void compactIn();

	// ---- protocol ----
	void sendHandshakeAndLogin();
	void handleLoginPacket(JavaByteBuf* in, int32_t id);
	void handlePlayPacket(JavaByteBuf* in, int32_t id);
	void deferFrame(const JavaByteBuf& frame, int32_t id);
	void flushDeferred();

	// ---- world ----
	void createLevel();
	// Everything a Respawn (dimension change or death) has to unwind.
	void resetForRespawn();
	// See respawnHold.  Called once a frame, does nothing when not holding.
	void holdPlayerForTerrain();
	/*
	 * Java has four game modes and m8 has two.  Survival and adventure are both
	 * "not creative" as far as the abilities go, and spectator is the closest
	 * thing to creative that exists here.
	 */
	bool_t isCreativeGamemode() const;
	// Push the current gamemode, then the server's abilities, onto the player.
	void applyGameMode();
	void applyAbilities();
	void applyChunk(int32_t cx, int32_t cz, int32_t mask, bool_t groundUp,
	                bool_t skyLight, const uint8_t* data, size_t len);
	void setJavaBlock(int32_t jx, int32_t jy, int32_t jz, int32_t state);
	void applyInventorySlot(int32_t javaSlot, int32_t itemId, int32_t count, int32_t damage,
	                        const std::vector<uint8_t>& raw);
	/*
	 * Signs.  A sign is two things in 0.8.1: a block, which arrives with the
	 * chunk or as a Block Change, and a SignTileEntity, which is what actually
	 * draws the post, the board and the four lines.  Chunk data never carries
	 * tile entities, so without this the block is there and nothing is drawn -
	 * SignTile::getRenderShape() is -1, so the chunk mesher contributes nothing
	 * at all and the sign is invisible.  The server sends Update Sign for every
	 * sign in a chunk right after the chunk itself, which is where the text and
	 * the tile entity both come from.
	 */
	void applySignText(int32_t jx, int32_t jy, int32_t jz, const std::string* lines);
	// Sends the four lines of a sign the local player just finished editing.
	void sendSignText(int32_t mx, int32_t my, int32_t mz);

	// ---- entities ----
	Entity* findEntity(int32_t javaEid);
	void attachEntity(int32_t javaEid, Entity* e);
	void retireEntity(Entity* old);
	void spawnMob(JavaByteBuf* in);
	void spawnObject(JavaByteBuf* in);
	void spawnPlayer(JavaByteBuf* in);
	void moveEntity(int32_t javaEid, double dx, double dy, double dz,
	                bool_t hasRot, float yaw, float pitch, bool_t absolute);
	void destroyEntity(int32_t javaEid);
	// Consumes a DataWatcher blob. Returns the ItemStack at index 10 when the
	// blob carries one (that is how a dropped item announces what it is).
	bool_t readMetadata(JavaByteBuf* in, int32_t* outItemId, int32_t* outCount,
	                    int32_t* outDamage);
	bool_t readSlot(JavaByteBuf* in, int32_t* outId, int32_t* outCount, int32_t* outDamage,
	                std::vector<uint8_t>* rawOut);
	void skipNbt(JavaByteBuf* in);
	// Walks a Java (big endian) tile entity tag and picks out Text1..Text4.
	// Returns 0 when the tag was malformed; the buffer is consumed either way.
	bool_t readSignNbt(JavaByteBuf* in, std::string* lines);

	// ---- outbound helpers ----
	void sendKeepAlive(int32_t id);
	/*
	 * The equivalent of EntityPlayerSP.onUpdateWalkingPlayer, called once per
	 * game tick from LocalPlayer::tick.  A Java server expects to hear from the
	 * client every single tick - the packet may say nothing more than "still on
	 * the ground", but it is that steady beat the server counts ticks by, and
	 * the anti-bot checks public servers run reject a client that reports only
	 * when it feels like it.
	 */
	void playerTick();
	void sendPositionNow(bool_t force);
	// Sneak and sprint state changes (0x0B Entity Action).
	void sendEntityAction(int32_t action);
	void sendDigging(int32_t status, int32_t jx, int32_t jy, int32_t jz, int32_t face);
	void sendHeldSlot(int32_t slot);
	// Notices that the player picked a different hotbar slot and tells the server.
	void syncHeldSlot();
	void writeHeldStack(JavaByteBuf* out);
	// Creative only: hand the server everything m8 is already carrying.
	void pushCreativeInventory();
	void sendCreativeSlot(int32_t javaSlot, const struct ItemInstance* item);
	void sendClientSettings();
	// "MC|Brand": every real client announces what it is right after Join Game.
	void sendClientBrand();
	// Answers 0x48 Resource Pack Send, which some servers wait on before letting
	// the player through.
	void sendResourcePackStatus(const std::string& hash, int32_t status);
};
