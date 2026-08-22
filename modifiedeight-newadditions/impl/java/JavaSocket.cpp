#include <java/JavaSocket.hpp>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
typedef int socklen_t_compat;
#define JSOCK_ERRNO      WSAGetLastError()
#define JSOCK_WOULDBLOCK WSAEWOULDBLOCK
#define JSOCK_INPROGRESS WSAEWOULDBLOCK
#define JSOCK_INVALID    ((int64_t)INVALID_SOCKET)
#define JSOCK_CLOSE(s)   closesocket((SOCKET)(s))
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>
typedef socklen_t socklen_t_compat;
#define JSOCK_ERRNO      errno
#define JSOCK_WOULDBLOCK EAGAIN
#define JSOCK_INPROGRESS EINPROGRESS
#define JSOCK_INVALID    ((int64_t)-1)
#define JSOCK_CLOSE(s)   ::close((int)(s))
#endif

static bool_t gJavaSocketInited = 0;

void JavaSocket::globalInit() {
	if(gJavaSocketInited) return;
	gJavaSocketInited = 1;
#ifdef _WIN32
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
}

JavaSocket::JavaSocket() {
	this->fd = JSOCK_INVALID;
	this->state = CLOSED;
}

JavaSocket::~JavaSocket() {
	this->close();
}

void JavaSocket::close() {
	if(this->fd != JSOCK_INVALID) {
		JSOCK_CLOSE(this->fd);
		this->fd = JSOCK_INVALID;
	}
	this->state = CLOSED;
}

bool_t JavaSocket::beginConnect(const std::string& ip, int32_t port) {
	JavaSocket::globalInit();
	this->close();
	this->lastError.clear();

	int64_t s = (int64_t)socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(s == JSOCK_INVALID) {
		this->lastError = "socket() failed";
		this->state = FAILED;
		return 0;
	}

#ifdef _WIN32
	u_long nb = 1;
	ioctlsocket((SOCKET)s, FIONBIO, &nb);
#else
	int flags = fcntl((int)s, F_GETFL, 0);
	fcntl((int)s, F_SETFL, flags | O_NONBLOCK);
#endif
	int one = 1;
	setsockopt((int)s, IPPROTO_TCP, TCP_NODELAY, (const char*)&one, sizeof(one));

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons((uint16_t)port);
	if(inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) != 1) {
		JSOCK_CLOSE(s);
		this->lastError = "Bad server address";
		this->state = FAILED;
		return 0;
	}

	int rc = ::connect((int)s, (struct sockaddr*)&addr, sizeof(addr));
	if(rc == 0) {
		this->fd = s;
		this->state = CONNECTED;
		return 1;
	}
	int err = JSOCK_ERRNO;
	if(err == JSOCK_INPROGRESS
#ifndef _WIN32
	   || err == EINTR
#endif
	) {
		this->fd = s;
		this->state = CONNECTING;
		return 1;
	}
	JSOCK_CLOSE(s);
	char buf[96];
	snprintf(buf, sizeof(buf), "connect() failed (%d)", err);
	this->lastError = buf;
	this->state = FAILED;
	return 0;
}

void JavaSocket::pumpConnect() {
	if(this->state != CONNECTING || this->fd == JSOCK_INVALID) return;

	fd_set wset, eset;
	FD_ZERO(&wset);
	FD_ZERO(&eset);
	FD_SET((int)this->fd, &wset);
	FD_SET((int)this->fd, &eset);
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	int rc = select((int)this->fd + 1, 0, &wset, &eset, &tv);
	if(rc <= 0) return; // still pending

	int soErr = 0;
	socklen_t_compat len = sizeof(soErr);
	if(getsockopt((int)this->fd, SOL_SOCKET, SO_ERROR, (char*)&soErr, &len) != 0) soErr = JSOCK_ERRNO;

	if(soErr != 0 || FD_ISSET((int)this->fd, &eset)) {
		char buf[96];
		snprintf(buf, sizeof(buf), "Could not reach server (%d)", soErr);
		this->lastError = buf;
		this->close();
		this->state = FAILED;
		return;
	}
	this->state = CONNECTED;
}

int32_t JavaSocket::recvSome(uint8_t* dst, int32_t cap) {
	if(this->state != CONNECTED || this->fd == JSOCK_INVALID) return -1;
	int n = (int)recv((int)this->fd, (char*)dst, cap, 0);
	if(n > 0) return n;
	if(n == 0) {
		this->lastError = "Server closed the connection";
		return -1;
	}
	int err = JSOCK_ERRNO;
	if(err == JSOCK_WOULDBLOCK
#ifndef _WIN32
	   || err == EWOULDBLOCK || err == EINTR
#endif
	) return 0;
	char buf[96];
	snprintf(buf, sizeof(buf), "recv() failed (%d)", err);
	this->lastError = buf;
	return -1;
}

int32_t JavaSocket::sendSome(const uint8_t* src, int32_t len) {
	if(this->state != CONNECTED || this->fd == JSOCK_INVALID) return -1;
	int flags = 0;
#if !defined(_WIN32) && defined(MSG_NOSIGNAL)
	flags = MSG_NOSIGNAL;
#endif
	int n = (int)send((int)this->fd, (const char*)src, len, flags);
	if(n >= 0) return n;
	int err = JSOCK_ERRNO;
	if(err == JSOCK_WOULDBLOCK
#ifndef _WIN32
	   || err == EWOULDBLOCK || err == EINTR
#endif
	) return 0;
	char buf[96];
	snprintf(buf, sizeof(buf), "send() failed (%d)", err);
	this->lastError = buf;
	return -1;
}

// ---------------------------------------------------------------------------
// DNS
// ---------------------------------------------------------------------------

bool_t JavaSocket::resolveAddress(const std::string& host, std::string* outIp) {
	JavaSocket::globalInit();

	// Already numeric?
	struct in_addr probe;
	if(inet_pton(AF_INET, host.c_str(), &probe) == 1) {
		if(outIp) *outIp = host;
		return 1;
	}

	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	struct addrinfo* res = 0;
	if(getaddrinfo(host.c_str(), 0, &hints, &res) != 0 || !res) return 0;

	bool_t ok = 0;
	for(struct addrinfo* it = res; it; it = it->ai_next) {
		if(it->ai_family != AF_INET || !it->ai_addr) continue;
		struct sockaddr_in* sin = (struct sockaddr_in*)it->ai_addr;
		char txt[INET_ADDRSTRLEN];
		if(inet_ntop(AF_INET, &sin->sin_addr, txt, sizeof(txt))) {
			if(outIp) *outIp = txt;
			ok = 1;
			break;
		}
	}
	freeaddrinfo(res);
	return ok;
}

/* Encode "a.b.c" as DNS labels: 1a1b1c0 */
static void javaDnsWriteName(std::vector<uint8_t>& out, const std::string& name) {
	size_t i = 0;
	while(i < name.size()) {
		size_t dot = name.find('.', i);
		if(dot == std::string::npos) dot = name.size();
		size_t len = dot - i;
		if(len > 63) len = 63;
		out.push_back((uint8_t)len);
		for(size_t k = 0; k < len; ++k) out.push_back((uint8_t)name[i + k]);
		i = dot + 1;
	}
	out.push_back(0);
}

/* Walks a (possibly compressed) name, returning the offset just past it. */
static size_t javaDnsSkipName(const uint8_t* buf, size_t len, size_t pos, std::string* out) {
	if(out) out->clear();
	int32_t guard = 0;
	size_t cursor = pos;
	size_t afterPointer = 0;
	while(cursor < len && ++guard < 128) {
		uint8_t l = buf[cursor];
		if(l == 0) {
			++cursor;
			break;
		}
		if((l & 0xC0) == 0xC0) {
			if(cursor + 1 >= len) return len;
			if(!afterPointer) afterPointer = cursor + 2;
			cursor = (size_t)(((l & 0x3F) << 8) | buf[cursor + 1]);
			continue;
		}
		if(cursor + 1 + l > len) return len;
		if(out) {
			if(out->size()) out->push_back('.');
			out->append((const char*)(buf + cursor + 1), l);
		}
		cursor += 1 + (size_t)l;
	}
	return afterPointer ? afterPointer : cursor;
}

/* Collects the nameservers we should ask. */
static void javaDnsNameservers(std::vector<std::string>& out) {
#ifndef _WIN32
	FILE* f = fopen("/etc/resolv.conf", "r");
	if(f) {
		char line[256];
		while(fgets(line, sizeof(line), f)) {
			char* p = line;
			while(*p == ' ' || *p == '\t') ++p;
			if(strncmp(p, "nameserver", 10) != 0) continue;
			p += 10;
			while(*p == ' ' || *p == '\t') ++p;
			char* e = p;
			while(*e && *e != '\n' && *e != '\r' && *e != ' ' && *e != '\t' && *e != '#') ++e;
			*e = 0;
			if(*p && strchr(p, ':') == 0) out.push_back(p); // IPv4 resolvers only
		}
		fclose(f);
	}
#endif
	// Public fallbacks, so an SRV lookup still works when resolv.conf is
	// unavailable (Android, Windows, sandboxed builds).
	out.push_back("1.1.1.1");
	out.push_back("8.8.8.8");
}

bool_t JavaSocket::resolveSrv(const std::string& fqdn, std::string* outTarget, int32_t* outPort) {
	JavaSocket::globalInit();

	std::vector<std::string> servers;
	javaDnsNameservers(servers);

	std::vector<uint8_t> query;
	query.push_back(0x4D); query.push_back(0x38); // transaction id "M8"
	query.push_back(0x01); query.push_back(0x00); // standard query, recursion desired
	query.push_back(0x00); query.push_back(0x01); // 1 question
	query.push_back(0x00); query.push_back(0x00);
	query.push_back(0x00); query.push_back(0x00);
	query.push_back(0x00); query.push_back(0x00);
	javaDnsWriteName(query, fqdn);
	query.push_back(0x00); query.push_back(0x21); // QTYPE SRV (33)
	query.push_back(0x00); query.push_back(0x01); // QCLASS IN

	for(size_t si = 0; si < servers.size(); ++si) {
		struct sockaddr_in ns;
		memset(&ns, 0, sizeof(ns));
		ns.sin_family = AF_INET;
		ns.sin_port = htons(53);
		if(inet_pton(AF_INET, servers[si].c_str(), &ns.sin_addr) != 1) continue;

		int64_t s = (int64_t)socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if(s == JSOCK_INVALID) continue;

		bool_t got = 0;
		if(sendto((int)s, (const char*)&query[0], (int)query.size(), 0,
		          (struct sockaddr*)&ns, sizeof(ns)) == (int)query.size()) {
			fd_set rset;
			FD_ZERO(&rset);
			FD_SET((int)s, &rset);
			struct timeval tv;
			tv.tv_sec = 1;
			tv.tv_usec = 500000;
			if(select((int)s + 1, &rset, 0, 0, &tv) > 0) {
				uint8_t reply[1500];
				int n = (int)recv((int)s, (char*)reply, sizeof(reply), 0);
				if(n > 12) {
					size_t len = (size_t)n;
					int32_t qdCount = (reply[4] << 8) | reply[5];
					int32_t anCount = (reply[6] << 8) | reply[7];
					size_t pos = 12;
					for(int32_t q = 0; q < qdCount && pos < len; ++q) {
						pos = javaDnsSkipName(reply, len, pos, 0);
						pos += 4; // QTYPE + QCLASS
					}
					int32_t bestPriority = 0x7FFFFFFF;
					for(int32_t a = 0; a < anCount && pos + 10 <= len; ++a) {
						pos = javaDnsSkipName(reply, len, pos, 0);
						if(pos + 10 > len) break;
						int32_t rtype = (reply[pos] << 8) | reply[pos + 1];
						int32_t rdLen = (reply[pos + 8] << 8) | reply[pos + 9];
						size_t rdata = pos + 10;
						if(rdata + (size_t)rdLen > len) break;
						if(rtype == 33 && rdLen >= 7) { // SRV
							int32_t prio = (reply[rdata] << 8) | reply[rdata + 1];
							int32_t port = (reply[rdata + 4] << 8) | reply[rdata + 5];
							std::string target;
							javaDnsSkipName(reply, len, rdata + 6, &target);
							if(target.size() && prio < bestPriority) {
								bestPriority = prio;
								if(outTarget) *outTarget = target;
								if(outPort) *outPort = port;
								got = 1;
							}
						}
						pos = rdata + (size_t)rdLen;
					}
				}
			}
		}
		JSOCK_CLOSE(s);
		if(got) return 1;
	}
	return 0;
}

bool_t JavaSocket::resolveMinecraftHost(const std::string& host, int32_t typedPort,
                                        std::string* outIp, int32_t* outPort,
                                        std::string* outConnectHost) {
	std::string cleanHost = host;
	// Allow "host:port" to be typed straight into the address box.
	size_t colon = cleanHost.find(':');
	int32_t port = typedPort;
	if(colon != std::string::npos) {
		std::string tail = cleanHost.substr(colon + 1);
		long p = strtol(tail.c_str(), 0, 10);
		if(p > 0 && p < 65536) port = (int32_t)p;
		cleanHost = cleanHost.substr(0, colon);
	}

	if(outConnectHost) *outConnectHost = cleanHost;
	if(outPort) *outPort = port;

	// A numeric address means the operator told us exactly where to go.
	struct in_addr probe;
	if(inet_pton(AF_INET, cleanHost.c_str(), &probe) == 1) {
		if(outIp) *outIp = cleanHost;
		return 1;
	}

	// SRV first. Only trust it when we can also resolve its target.
	std::string srvTarget;
	int32_t srvPort = 0;
	if(JavaSocket::resolveSrv("_minecraft._tcp." + cleanHost, &srvTarget, &srvPort)) {
		std::string ip;
		if(JavaSocket::resolveAddress(srvTarget, &ip)) {
			if(outIp) *outIp = ip;
			if(outPort) *outPort = srvPort;
			// The handshake must still carry the name the player typed;
			// virtual-host aware proxies (BungeeCord, aternos) key on it.
			if(outConnectHost) *outConnectHost = cleanHost;
			return 1;
		}
	}

	std::string ip;
	if(JavaSocket::resolveAddress(cleanHost, &ip)) {
		if(outIp) *outIp = ip;
		return 1;
	}
	return 0;
}
