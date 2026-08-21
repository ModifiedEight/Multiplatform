#pragma once
#include <_types.h>
#include <string>

struct Minecraft;
struct Packet;
struct JavaSession;

/*
 * The one global handle on the Java session.
 *
 * m8 can only be in one world at a time, so a single session is enough, and
 * routing everything through statics keeps the edits in the engine down to
 * one-line calls that read the same whether or not a Java server is involved.
 */
struct JavaBridge
{
	// Starts a session. host may carry a ":port" suffix; typedPort is the port
	// column from the server list and is used when it does not.
	static bool_t begin(Minecraft* minecraft, const std::string& displayName,
	                    const std::string& host, int32_t typedPort);

	// True from begin() until the session ends (cleanly or not).
	static bool_t isActive();

	// Called once per frame off RakNetInstance::runEvents. Safe to call always.
	static void pump();

	/*
	 * Called once per game tick from LocalPlayer::tick, in place of m8's own
	 * sendPosition().  A Java server wants to hear from the client every tick;
	 * m8 reports only after a tenth of a block or a whole degree, which is far
	 * too sparse for the movement checks on the other side.
	 */
	static void playerTick();

	// Tears the session down and hands the client back to RakNet.
	static void shutdown();

	// Returns 1 when the packet was consumed by the Java session, in which case
	// the caller must not touch RakNet. Returns 0 when no session is running.
	static bool_t interceptSend(Packet* pk);

	// Routes a typed chat line (commands included) to the Java server.
	// Returns 0 when there is no session, so the caller falls back to MCPE.
	static bool_t sendChat(const std::string& text);

	static JavaSession* session();
};
