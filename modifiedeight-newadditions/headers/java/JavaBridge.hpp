#pragma once
#include <_types.h>
#include <string>

struct Minecraft;
struct Packet;
struct JavaSession;

struct JavaBridge {
  static bool_t begin(Minecraft *minecraft, const std::string &displayName,
                      const std::string &host, int32_t typedPort);

  static bool_t isActive();
  static void pump();
  static void playerTick();
  static void shutdown();
  static bool_t interceptSend(Packet *pk);
  static bool_t sendChat(const std::string &text);
  static JavaSession *session();
};
