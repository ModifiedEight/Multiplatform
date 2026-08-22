#pragma once
#include <_types.h>
#include <string>

/*
 * Server List Ping for the Java Edition entries in m8's server list.
 *
 * m8 learns whether an MCPE server is up from RakNet's broadcast pings, and a
 * Java server answers none of those, so a Java entry used to sit on
 * "Loading..." for ever even though joining it worked perfectly.  Java has its
 * own status handshake - the same one the vanilla server list uses: connect,
 * handshake with next state 1, ask, and read back a single JSON blob carrying
 * the MOTD, the player counts and the version.
 *
 * The query blocks, so it runs on a detached thread and drops its answer into a
 * cache; get() only ever reads that cache and starts a thread when there is
 * nothing fresh in it.  Render code can therefore call get() every frame.
 */
struct JavaPingResult
{
	enum State
	{
		// No answer yet - either the first query is in flight or one is about to be.
		PENDING = 0,
		ONLINE,
		// Refused, timed out, unresolvable, or answered with something unreadable.
		OFFLINE
	};

	State state;
	// First line of the MOTD, colour codes stripped.
	std::string motd;
	// Whatever the server calls its version, e.g. "1.8.9" or "Spigot 1.8.8".
	std::string version;
	// Protocol number the server reports; 47 is 1.8.x, which is what m8 speaks.
	int32_t protocol;
	int32_t online, max;
	// Round trip of the status exchange, in milliseconds.
	int32_t latencyMs;

	JavaPingResult();
};

struct JavaPing
{
	/*
	 * The status of host:typedPort.  host may carry its own ":port" suffix and
	 * may resolve through an SRV record, exactly as when joining.  Returns
	 * PENDING until an answer lands; never blocks.
	 */
	static JavaPingResult get(const std::string& host, int32_t typedPort);
};
