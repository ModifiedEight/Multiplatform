#pragma once
#include <string>
#include <utility>
#include <vector>

struct DiscordRPC {
  struct Button {
    std::string label;
    std::string url;
  };

#if defined(__linux__) && !defined(ANDROID)
  static void init(const std::string &appId);
  static void update(const std::string &details,
                     const std::string &largeImageKey = "",
                     const std::string &largeImageText = "",
                     const std::vector<Button> &buttons = {}, int partySize = 0,
                     int partyMax = 0);
  static void shutdown();
#else
  static inline void init(const std::string &) {}
  static inline void update(const std::string &,
                            const std::string & = "",
                            const std::string & = "",
                            const std::vector<Button> & = {}, int = 0,
                            int = 0) {}
  static inline void shutdown() {}
#endif
};
