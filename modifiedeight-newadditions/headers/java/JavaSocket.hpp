#pragma once
#include <_types.h>
#include <string>
#include <vector>

/*
 * JavaSocket - minimal non-blocking TCP client plus the DNS lookups the Java
 * Edition server list needs.
 *
 * Minecraft Java servers are addressed through an SRV record
 * (_minecraft._tcp.<host>) whenever the operator does not run on the default
 * port; aternos hosts in particular hand out a random high port that way. So a
 * plain A lookup is not enough - we resolve SRV first and only fall back to
 * (host, port-as-typed) when there is no SRV record.
 */
struct JavaSocket
{
	enum State
	{
		CLOSED = 0,
		CONNECTING,
		CONNECTED,
		FAILED
	};

	int64_t fd;
	State state;
	std::string lastError;

	JavaSocket();
	~JavaSocket();

	// Starts a non-blocking connect. Returns 0 and sets lastError on immediate failure.
	bool_t beginConnect(const std::string& ip, int32_t port);
	// Drives an in-progress connect. Call every frame while state == CONNECTING.
	void pumpConnect();
	// Returns bytes read (>0), 0 for "nothing available", -1 for closed/error.
	int32_t recvSome(uint8_t* dst, int32_t cap);
	// Returns bytes written (may be less than len), or -1 on error.
	int32_t sendSome(const uint8_t* src, int32_t len);
	void close();

	static void globalInit();

	/*
	 * Resolves a user-typed "host" (with the port they typed as fallback) into
	 * a numeric address and port. Consults _minecraft._tcp.<host> SRV first.
	 * Returns 0 if nothing could be resolved.
	 */
	static bool_t resolveMinecraftHost(const std::string& host, int32_t typedPort,
	                                   std::string* outIp, int32_t* outPort,
	                                   std::string* outConnectHost);
	// Raw A/AAAA lookup via the platform resolver.
	static bool_t resolveAddress(const std::string& host, std::string* outIp);
	// Raw SRV lookup, implemented as a UDP DNS query so it behaves the same everywhere.
	static bool_t resolveSrv(const std::string& fqdn, std::string* outTarget, int32_t* outPort);
};
