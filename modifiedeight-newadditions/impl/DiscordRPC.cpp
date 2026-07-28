#if defined(__linux__) && !defined(ANDROID)
#include <DiscordRPC.hpp>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <fcntl.h>
#include <poll.h>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <vector>

static int g_ipcFd = -1;
static std::string g_appId;
static int64_t g_startTime = 0;
static bool g_ready = false;

static std::string jsonEscape(const std::string &s) {
  std::string out;
  for (char c : s) {
    if (c == '"')
      out += "\\\"";
    else if (c == '\\')
      out += "\\\\";
    else if (c == '\n')
      out += "\\n";
    else
      out += c;
  }
  return out;
}

static bool readExact(int fd, void *buf, size_t n, int timeoutMs = 2000) {
  size_t got = 0;
  char *p = (char *)buf;
  while (got < n) {
    struct pollfd pfd = {fd, POLLIN, 0};
    if (poll(&pfd, 1, timeoutMs) <= 0)
      return false;
    ssize_t bytes = read(fd, p + got, n - got);
    if (bytes <= 0)
      return false;
    got += bytes;
  }
  return true;
}

static std::string readFrame(int fd, int timeoutMs = 3000) {
  uint32_t hdr[2];
  if (!readExact(fd, hdr, 8, timeoutMs))
    return "";
  uint32_t length = hdr[1];
  if (length == 0 || length > 65535)
    return "";
  std::string payload(length, '\0');
  if (!readExact(fd, &payload[0], length, timeoutMs))
    return "";
  return payload;
}

static bool sendFrame(int fd, uint32_t op, const std::string &payload) {
  uint32_t hdr[2] = {op, (uint32_t)payload.size()};
  if (write(fd, hdr, 8) != 8)
    return false;
  if (!payload.empty() &&
      write(fd, payload.data(), payload.size()) != (ssize_t)payload.size())
    return false;
  return true;
}

static bool doConnect(const std::string &appId) {
  uid_t uid = getuid();
  for (int i = 0; i < 10; i++) {
    char path[256];
    snprintf(path, sizeof(path), "/run/user/%d/discord-ipc-%d", uid, i);

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0)
      continue;

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
      close(fd);
      fd = socket(AF_UNIX, SOCK_STREAM, 0);
      if (fd < 0)
        continue;
      snprintf(path, sizeof(path), "/tmp/discord-ipc-%d", i);
      strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);
      if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        close(fd);
        continue;
      }
    }

    char handshake[256];
    snprintf(handshake, sizeof(handshake), "{\"v\":1,\"client_id\":\"%s\"}",
             appId.c_str());
    if (!sendFrame(fd, 0, handshake)) {
      close(fd);
      continue;
    }

    std::string resp = readFrame(fd, 5000);
    if (resp.empty()) {
      close(fd);
      continue;
    }

    printf("[DiscordRPC] Connected via %s\n", path);
    g_ipcFd = fd;
    g_ready = true;
    return true;
  }
  return false;
}

void DiscordRPC::init(const std::string &appId) {
  g_appId = appId;
  g_startTime = (int64_t)time(nullptr);
  g_ready = false;
  if (g_ipcFd >= 0) {
    close(g_ipcFd);
    g_ipcFd = -1;
  }
  doConnect(appId);
}

void DiscordRPC::update(const std::string &details,
                        const std::string &largeImageKey,
                        const std::string &largeImageText,
                        const std::vector<Button> &buttons,
                        int partySize, int partyMax) {
  if (!g_ready || g_ipcFd < 0) {
    if (!doConnect(g_appId))
      return;
  }

  std::string assetsJson;
  if (!largeImageKey.empty()) {
    assetsJson = "\"large_image\":\"" + jsonEscape(largeImageKey) +
                 "\","
                 "\"large_text\":\"" +
                 jsonEscape(largeImageText) + "\",";
  }

  std::string buttonsJson;
  if (!buttons.empty()) {
    buttonsJson = "\"buttons\":[";
    for (size_t i = 0; i < buttons.size(); ++i) {
      if (i)
        buttonsJson += ",";
      buttonsJson += "{\"label\":\"" + jsonEscape(buttons[i].label) +
                     "\",\"url\":\"" + jsonEscape(buttons[i].url) + "\"}";
    }
    buttonsJson += "],";
  }

  std::string partyJson;
  if (partySize > 0 && partyMax > 0) {
    char pbuf[256];
    snprintf(pbuf, sizeof(pbuf),
             "\"party\":{\"id\":\"me8party\",\"size\":[%d,%d]},", partySize, partyMax);
    partyJson = pbuf;
  }

  char payload[4096];
  snprintf(payload, sizeof(payload),
           "{"
           "\"cmd\":\"SET_ACTIVITY\","
           "\"args\":{"
           "\"pid\":%d,"
           "\"activity\":{"
           "\"details\":\"%s\","
           "%s"
           "%s"
           "%s"
           "\"timestamps\":{\"start\":%lld}"
           "}"
           "},"
           "\"nonce\":\"discord_rpc_1\""
           "}",
           (int)getpid(), jsonEscape(details).c_str(), assetsJson.c_str(),
           buttonsJson.c_str(), partyJson.c_str(), (long long)g_startTime);

  if (!sendFrame(g_ipcFd, 1, payload)) {
    printf("[DiscordRPC] Failed to send activity.\n");
    close(g_ipcFd);
    g_ipcFd = -1;
    g_ready = false;
    return;
  }

  struct pollfd pfd = {g_ipcFd, POLLIN, 0};
  if (poll(&pfd, 1, 500) > 0) {
    char buf[4096];
    (void)read(g_ipcFd, buf, sizeof(buf));
  }

  printf("[DiscordRPC] Activity updated: %s\n", details.c_str());
}

void DiscordRPC::shutdown() {
  if (g_ipcFd >= 0) {
    close(g_ipcFd);
    g_ipcFd = -1;
  }
  g_ready = false;
}
#endif
