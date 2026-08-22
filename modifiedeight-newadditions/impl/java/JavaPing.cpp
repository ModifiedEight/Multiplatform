#include <java/JavaPing.hpp>
#include <java/JavaByteBuf.hpp>
#include <java/JavaChat.hpp>
#include <java/JavaLog.hpp>
#include <java/JavaSession.hpp>
#include <java/JavaSocket.hpp>
#include <utils.h>
#include <stdio.h>

#include <map>
#include <mutex>
#include <thread>

/*
 * How long an answer is trusted before get() asks again.  The vanilla server
 * list pings once when it opens and then only on demand; re-asking every few
 * seconds while the screen is up costs one short-lived socket and keeps the
 * player counts moving, which is nicer to look at.
 */
#define PING_TTL_MS      6000
#define PING_CONNECT_MS  5000
#define PING_READ_MS     5000
// A status blob is a MOTD plus, usually, a 64x64 base64 favicon. 512 KiB is
// several times more than any real server sends and stops a hostile or broken
// one from making us allocate for ever.
#define PING_MAX_FRAME   (512 * 1024)

JavaPingResult::JavaPingResult() {
	this->state = PENDING;
	this->protocol = 0;
	this->online = 0;
	this->max = 0;
	this->latencyMs = 0;
}

namespace {

struct Entry
{
	JavaPingResult result;
	// A thread is out asking right now; do not start a second one.
	bool_t inFlight;
	// getTimeMs() when the answer landed. Zero means "never answered".
	int32_t answeredMs;

	Entry() {
		this->inFlight = 0;
		this->answeredMs = 0;
	}
};

/*
 * Heap allocated and never freed on purpose.  The worker threads are detached,
 * so one can still be finishing while the process shuts down, and a cache that
 * is never destroyed cannot be written to after destruction.
 */
std::map<std::string, Entry>* cache() {
	static std::map<std::string, Entry>* c = new std::map<std::string, Entry>();
	return c;
}

std::mutex& cacheLock() {
	static std::mutex* m = new std::mutex();
	return *m;
}

// Length-prefixed, uncompressed: status never negotiates compression.
bool_t sendPacket(JavaSocket* sock, const JavaByteBuf& body) {
	JavaByteBuf frame;
	frame.writeVarInt((int32_t)body.size());
	frame.writeBuf(body);

	int32_t sent = 0;
	int32_t waited = 0;
	while(sent < (int32_t)frame.size()) {
		int32_t n = sock->sendSome(frame.data() + sent, (int32_t)frame.size() - sent);
		if(n < 0) return 0;
		if(n == 0) {
			if(waited >= PING_READ_MS) return 0;
			sleepMs(10);
			waited += 10;
			continue;
		}
		waited = 0;
		sent += n;
	}
	return 1;
}

/*
 * Reads one whole packet: a VarInt length, then that many bytes.  The length
 * itself arrives a byte at a time, so read into a buffer and re-try the decode
 * until it succeeds rather than assuming the first recv holds all of it.
 */
bool_t readPacket(JavaSocket* sock, std::vector<uint8_t>* out) {
	std::vector<uint8_t> buf;
	int32_t waited = 0;
	int32_t frameLen = -1;
	size_t headerLen = 0;

	for(;;) {
		if(frameLen < 0 && buf.size()) {
			JavaByteBuf probe(&buf[0], buf.size());
			int32_t len = probe.readVarInt();
			if(!probe.failed()) {
				if(len <= 0 || len > PING_MAX_FRAME) return 0;
				frameLen = len;
				headerLen = probe.readPos;
			} else if(buf.size() >= 5) {
				// Five bytes is the widest a VarInt gets; this is not one.
				return 0;
			}
		}
		if(frameLen >= 0 && buf.size() >= headerLen + (size_t)frameLen) {
			out->assign(buf.begin() + headerLen, buf.begin() + headerLen + frameLen);
			return 1;
		}

		uint8_t chunk[4096];
		int32_t n = sock->recvSome(chunk, (int32_t)sizeof(chunk));
		if(n < 0) return 0;
		if(n == 0) {
			if(waited >= PING_READ_MS) return 0;
			sleepMs(10);
			waited += 10;
			continue;
		}
		waited = 0;
		buf.insert(buf.end(), chunk, chunk + n);
	}
}

// The blocking half, run on its own thread. Fills in *out and returns 1 on success.
bool_t askServer(const std::string& host, int32_t typedPort, JavaPingResult* out) {
	char_t portTxt[24];
	sprintf(portTxt, ":%d", (int)typedPort);
	// What the player typed, which is also what keys the cache.
	std::string label = host + portTxt;

	std::string ip, handshakeHost;
	int32_t port = typedPort;
	if(!JavaSocket::resolveMinecraftHost(host, typedPort, &ip, &port, &handshakeHost)) {
		javaDebug("ping %s: could not resolve", label.c_str());
		return 0;
	}

	JavaSocket sock;
	if(!sock.beginConnect(ip, port)) {
		javaDebug("ping %s: %s", label.c_str(), sock.lastError.c_str());
		return 0;
	}
	int32_t waited = 0;
	while(sock.state == JavaSocket::CONNECTING) {
		if(waited >= PING_CONNECT_MS) {
			javaDebug("ping %s: connect timed out", label.c_str());
			return 0;
		}
		sock.pumpConnect();
		if(sock.state == JavaSocket::CONNECTING) {
			sleepMs(20);
			waited += 20;
		}
	}
	if(sock.state != JavaSocket::CONNECTED) {
		javaDebug("ping %s: %s", label.c_str(), sock.lastError.c_str());
		return 0;
	}

	int32_t sentAt = getTimeMs();

	// Handshake, next state 1 (status) rather than the 2 that joining uses.
	JavaByteBuf pk;
	pk.writeVarInt(0x00);
	pk.writeVarInt(JavaSession::PROTOCOL);
	pk.writeString(handshakeHost);
	pk.writeUShort((uint16_t)port);
	pk.writeVarInt(1);
	if(!sendPacket(&sock, pk)) return 0;

	// Status Request: an id and nothing else.
	pk.clear();
	pk.writeVarInt(0x00);
	if(!sendPacket(&sock, pk)) return 0;

	std::vector<uint8_t> body;
	if(!readPacket(&sock, &body)) {
		javaDebug("ping %s: no status response", label.c_str());
		return 0;
	}

	JavaByteBuf in(&body[0], body.size());
	int32_t id = in.readVarInt();
	std::string json = in.readString(PING_MAX_FRAME);
	if(in.failed() || id != 0x00) {
		javaDebug("ping %s: unexpected status packet 0x%02X", label.c_str(), (int)id);
		return 0;
	}

	Json::Value root;
	Json::Reader reader;
	if(!reader.parse(json, root, false) || !root.isObject()) {
		javaDebug("ping %s: status response is not JSON", label.c_str());
		return 0;
	}

	out->state = JavaPingResult::ONLINE;
	out->latencyMs = getTimeMs() - sentAt;
	out->motd = javaChatToText(root["description"]);
	// Only the first line fits in a list row.
	size_t nl = out->motd.find('\n');
	if(nl != std::string::npos) out->motd = out->motd.substr(0, nl);

	const Json::Value& players = root["players"];
	if(players.isObject()) {
		out->online = players["online"].isIntegral() ? players["online"].asInt() : 0;
		out->max = players["max"].isIntegral() ? players["max"].asInt() : 0;
	}
	const Json::Value& version = root["version"];
	if(version.isObject()) {
		// A version name is a plain string, but it is allowed to carry colour
		// codes, and aternos in particular does.
		if(version["name"].isString()) out->version = javaChatToText(version["name"]);
		if(version["protocol"].isIntegral()) out->protocol = version["protocol"].asInt();
	}

	/*
	 * A protocol of -1 (or 0) is how a front end says "there is no server behind
	 * me": aternos answers for a stopped server with protocol -1, version
	 * "\u00a7c\u25cf Offline" and 0/20 players, and a proxy in maintenance does
	 * much the same. It answered, but nothing can be joined, so the honest thing
	 * to show is Offline rather than a player count nobody is in.
	 */
	if(out->protocol <= 0) {
		javaDebug("ping %s: answered with protocol %d (\"%s\") - not a joinable server",
		          label.c_str(), (int)out->protocol, out->version.c_str());
		out->state = JavaPingResult::OFFLINE;
	}

	if(out->state == JavaPingResult::ONLINE) {
		javaDebug("ping %s: online %d/%d, %s (protocol %d), %d ms, motd \"%s\"",
		          label.c_str(), (int)out->online, (int)out->max, out->version.c_str(),
		          (int)out->protocol, (int)out->latencyMs, out->motd.c_str());
	}
	return 1;
}

void askAndStore(std::string key, std::string host, int32_t typedPort) {
	JavaPingResult r;
	if(!askServer(host, typedPort, &r)) {
		r = JavaPingResult();
		r.state = JavaPingResult::OFFLINE;
	}

	std::lock_guard<std::mutex> lock(cacheLock());
	std::map<std::string, Entry>::iterator it = cache()->find(key);
	if(it == cache()->end()) return;
	it->second.result = r;
	it->second.answeredMs = getTimeMs();
	// getTimeMs() is a millisecond count that starts at zero, so it really can
	// be zero here, and zero is the "never answered" marker.
	if(it->second.answeredMs == 0) it->second.answeredMs = 1;
	it->second.inFlight = 0;
}

} // namespace

JavaPingResult JavaPing::get(const std::string& host, int32_t typedPort) {
	char_t portTxt[24];
	sprintf(portTxt, ":%d", (int)typedPort);
	std::string key = host + portTxt;

	std::lock_guard<std::mutex> lock(cacheLock());
	Entry& e = (*cache())[key];

	bool_t stale = e.answeredMs != 0 && (int32_t)(getTimeMs() - e.answeredMs) >= PING_TTL_MS;
	if(!e.inFlight && (e.answeredMs == 0 || stale)) {
		e.inFlight = 1;
		std::thread(askAndStore, key, host, typedPort).detach();
	}
	return e.result;
}
