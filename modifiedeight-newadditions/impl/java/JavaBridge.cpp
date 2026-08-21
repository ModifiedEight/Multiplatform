#include <java/JavaBridge.hpp>
#include <java/JavaSession.hpp>
#include <java/JavaLog.hpp>
#include <Minecraft.hpp>
#include <entity/player/User.hpp>

static JavaSession* gJavaSession = 0;

JavaSession* JavaBridge::session() {
	return gJavaSession;
}

bool_t JavaBridge::isActive() {
	return gJavaSession && gJavaSession->isActive();
}

bool_t JavaBridge::begin(Minecraft* minecraft, const std::string& displayName,
                         const std::string& host, int32_t typedPort) {
	JavaBridge::shutdown();
	if(!minecraft) {
		return 0;
	}
	std::string user;
	if(minecraft->user) {
		user = minecraft->user->username;
	}
	if(user.empty()) {
		user = "Steve";
	}
	// Java 1.8 caps names at 16 characters and rejects anything outside
	// [A-Za-z0-9_], which m8 does allow in its profile name.
	std::string clean;
	for(size_t i = 0; i < user.size() && clean.size() < 16; ++i) {
		char c = user[i];
		if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
		   || (c >= '0' && c <= '9') || c == '_') {
			clean += c;
		}
	}
	if(clean.empty()) {
		clean = "Steve";
	}

	if(clean != user) {
		javaLog("profile name \"%s\" is not valid on a Java server, joining as \"%s\"",
		        user.c_str(), clean.c_str());
	}

	gJavaSession = new JavaSession();
	/*
	 * Even a session that could not get off the ground is worth keeping: it
	 * holds the reason, and pump() turns that into a DisconnectionScreen on the
	 * next frame.  Reporting success here means the caller shows the progress
	 * screen and the player gets the real message instead of a generic one.
	 */
	gJavaSession->begin(minecraft, displayName, host, typedPort, clean);
	return 1;
}

void JavaBridge::pump() {
	if(!gJavaSession) {
		return;
	}
	gJavaSession->pump();
	// Do not reap until the session has had the chance to put its failure on
	// screen - fail() closes the phase, and the *next* pump is what reports it.
	if(!gJavaSession->isActive()
	   && (gJavaSession->failure.empty() || gJavaSession->reportedFailure)) {
		delete gJavaSession;
		gJavaSession = 0;
	}
}

void JavaBridge::playerTick() {
	if(!JavaBridge::isActive()) {
		return;
	}
	gJavaSession->playerTick();
}

void JavaBridge::shutdown() {
	if(!gJavaSession) {
		return;
	}
	JavaSession* s = gJavaSession;
	gJavaSession = 0;
	s->shutdown();
	delete s;
}

bool_t JavaBridge::interceptSend(Packet* pk) {
	if(!gJavaSession) {
		return 0;
	}
	return gJavaSession->interceptSend(pk);
}

bool_t JavaBridge::sendChat(const std::string& text) {
	if(!JavaBridge::isActive()) {
		return 0;
	}
	gJavaSession->sendChatLine(text);
	return 1;
}
