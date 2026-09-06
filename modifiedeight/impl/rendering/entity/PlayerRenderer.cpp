#include <entity/Player.hpp>
#include <item/ArmorItem.hpp>
#include <map>
#include <mutex>
#include <rendering/EntityRenderDispatcher.hpp>
#include <rendering/Font.hpp>
#include <rendering/Tesselator.hpp>
#include <rendering/Textures.hpp>
#include <rendering/entity/PlayerRenderer.hpp>
#include <rendering/model/HumanoidModel.hpp>
#include <stb_image.h>
#include <thread>
#include <util/CrossPlatformWeb.hpp>

#include <unigl.h>

PlayerRenderer::PlayerRenderer(HumanoidModel *a2, float a3)
    : HumanoidMobRenderer(a2, a3) {
  this->hmodel1 = new HumanoidModel(1.0, 0.0);
  this->hmodel2 = new HumanoidModel(0.5, 0.0);
}

PlayerRenderer::~PlayerRenderer() {
  if (this->hmodel1)
    delete this->hmodel1;
  if (this->hmodel2)
    delete this->hmodel2;
}

static std::string _dword_D6E083C8_armorTextures[] = {
    "armor/cloth_1.png",   "armor/cloth_2.png",   "armor/chain_1.png",
    "armor/chain_2.png",   "armor/iron_1.png",    "armor/iron_2.png",
    "armor/diamond_1.png", "armor/diamond_2.png", "armor/gold_1.png",
    "armor/gold_2.png"};

void PlayerRenderer::renderName(Entity *a2_, float a3) {
  Player *a2 = (Player *)a2_;
  Mob *cameraEntity;
  float v7;
  float v8;
  float v9;
  Font *font;
  int32_t v11;
  float v12;

  cameraEntity = EntityRenderer::entityRenderDispatcher->cameraEntity;
  if (a2 != cameraEntity && a2->distanceToSqr(cameraEntity) <= 1024.0) {
    v7 =
        (float)(a2->prevPosX + (float)((float)(a2->posX - a2->prevPosX) * a3)) -
        EntityRenderDispatcher::xOff;
    v8 =
        (float)(a2->prevPosY + (float)((float)(a2->posY - a2->prevPosY) * a3)) -
        EntityRenderDispatcher::yOff;
    v9 =
        (float)(a2->prevPosZ + (float)((float)(a2->posZ - a2->prevPosZ) * a3)) -
        EntityRenderDispatcher::zOff;
    font = this->getFont();
    if (!font) return;
    glPushMatrix();
    float nameY = a2->isSneaking() ? 0.5f : 0.7f;
    glTranslatef(v7, v8 + nameY, v9);
    glRotatef(-EntityRenderer::entityRenderDispatcher->field_14, 0.0f, 1.0f, 0.0f);
    glRotatef(EntityRenderer::entityRenderDispatcher->field_18, 1.0f, 0.0f, 0.0f);
    glScalef(-0.026667f, -0.026667f, 0.026667f);
    glDisable(GL_LIGHTING);
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_TEXTURE_2D);
    int32_t v11 = font->width(a2->username);
    Tesselator::instance.begin(7);
    Tesselator::instance.color(0.0f, 0.0f, 0.0f, 0.35f);
    Tesselator::instance.vertex((float)(-v11 / 2 - 1), -1.0f, 0.0f);
    Tesselator::instance.vertex((float)(-v11 / 2 - 1), 8.0f, 0.0f);
    Tesselator::instance.vertex((float)(v11 / 2 + 1), 8.0f, 0.0f);
    Tesselator::instance.vertex((float)(v11 / 2 + 1), -1.0f, 0.0f);
    Tesselator::instance.draw(1);
    glEnable(GL_TEXTURE_2D);
    float v12 = (float)font->width(a2->username) * -0.5f;
    font->draw(a2->username, v12, 0.0f, 553648127, 0);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    font->draw(a2->username, v12, 0.0f, -1, 0);
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glPopMatrix();
  }
}

int32_t PlayerRenderer::prepareArmor(Mob *a2_, int32_t armour, float a4) {
  Player *a2 = (Player *)a2_;
  ItemInstance *armor;
  HumanoidModel *hmodel2;
  bool_t v9;
  bool_t v10;

  armor = a2->getArmor(armour);
  if (!ItemInstance::isArmorItem(armor)) {
    return -1;
  }
  this->bindTexture(
      _dword_D6E083C8_armorTextures
          [2 * ((ArmorItem *)armor->itemClass)->field_50 + (armour == 2)]);
  if (armour == 2) {
    hmodel2 = this->hmodel2;
  } else {
    hmodel2 = this->hmodel1;
  }
  hmodel2->headModel.field_1D = armour == 0;
  hmodel2->bodyModel.field_1D = (uint32_t)(armour - 1) <= 1;
  v9 = armour == 1;
  hmodel2->rightArmModel.field_1D = armour == 1;
  v10 = (uint32_t)(armour - 2) <= 1;
  hmodel2->leftArmModel.field_1D = v9;
  hmodel2->rightLegModel.field_1D = v10;
  hmodel2->leftLegModel.field_1D = v10;
  this->setArmor(hmodel2);
  hmodel2->field_4 = a2->isRiding();
  return 1;
}

#include <Minecraft.hpp>
#include <chrono>
#include <cstring>
#include <entity/LocalPlayer.hpp>
#ifdef _WIN32
#include <direct.h>
#define mkdirPortable(p) _mkdir(p)
#else
#include <sys/stat.h>
#define mkdirPortable(p) mkdir(p, 0777)
#endif

static std::map<std::string, unsigned char *> g_pendingSkinsData;
static std::map<std::string, std::pair<int, int>> g_pendingSkinsDims;
static std::map<std::string, uint64_t> g_lastSkinCheckTime;
static std::mutex skinMutex;

static uint64_t getCurrentTimeMillis() {
  return (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::steady_clock::now().time_since_epoch())
      .count();
}

static unsigned char *processSkinImage(const unsigned char *src, int w, int h,
                                       int &outW, int &outH) {
  outW = 64;
  outH = 32;
  unsigned char *out = (unsigned char *)calloc(64 * 32 * 4, 1);
  if (!out)
    return nullptr;

  int srcHalfH = (h == w) ? (h / 2) : h;

  for (int dy = 0; dy < 32; ++dy) {
    int sy = dy * srcHalfH / 32;
    if (sy >= h)
      sy = h - 1;
    for (int dx = 0; dx < 64; ++dx) {
      int sx = dx * w / 64;
      if (sx >= w)
        sx = w - 1;
      int srcIdx = (sy * w + sx) * 4;
      int dstIdx = (dy * 64 + dx) * 4;
      out[dstIdx + 0] = src[srcIdx + 0];
      out[dstIdx + 1] = src[srcIdx + 1];
      out[dstIdx + 2] = src[srcIdx + 2];
      out[dstIdx + 3] = src[srcIdx + 3];

      if (!(dx >= 32 && dx < 64 && dy < 16)) {
        out[dstIdx + 3] = 255;
      }
    }
  }
  return out;
}

static void uploadSkinTexture(const std::string &username,
                              const unsigned char *px, int w, int h) {
  int outW = 64, outH = 32;
  unsigned char *uploadPx = processSkinImage(px, w, h, outW, outH);
  if (!uploadPx)
    return;

  std::string texName = username + "_skin";
  Textures *tex = EntityRenderer::entityRenderDispatcher->textures;
  if (!tex) {
    free(uploadPx);
    return;
  }

  auto itTex = tex->textures.find(texName);
  GLuint skinTexId = 0;
  if (itTex != tex->textures.end() && itTex->second.glTexId != 0) {
    skinTexId = itTex->second.glTexId;
    glBindTexture(GL_TEXTURE_2D, skinTexId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, outW, outH, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, uploadPx);
    itTex->second.width = outW;
    itTex->second.height = outH;
  } else {
    glGenTextures(1, &skinTexId);
    glBindTexture(GL_TEXTURE_2D, skinTexId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, outW, outH, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, uploadPx);
    TextureData td;
    td.width = outW;
    td.height = outH;
    td.pixels = nullptr;
    td.glTexId = skinTexId;
    tex->textures.erase(texName);
    tex->textures.insert(
        std::pair<std::string, TextureData>(texName, std::move(td)));
  }
  tex->currentTexture = 0;
  free(uploadPx);
}

static std::vector<unsigned char>
downloadSkinData(const std::string &username) {
  if (username.empty())
    return {};

  std::vector<std::string> urls = {
      "https://raw.githubusercontent.com/gameherobrine2/test123/refs/heads/"
      "main/" +
          username + ".png",
      "https://raw.githubusercontent.com/gameherobrine2/test123/main/" +
          username + ".png",
      "https://raw.githubusercontent.com/gameherobrine2/test123/master/" +
          username + ".png",
      "https://github.com/gameherobrine2/test123/raw/main/" + username + ".png",
  };

  std::string lowerName = username;
  for (char &c : lowerName)
    c = (char)tolower((unsigned char)c);
  if (lowerName != username) {
    urls.push_back("https://raw.githubusercontent.com/gameherobrine2/test123/"
                   "refs/heads/main/" +
                   lowerName + ".png");
    urls.push_back(
        "https://raw.githubusercontent.com/gameherobrine2/test123/main/" +
        lowerName + ".png");
    urls.push_back(
        "https://raw.githubusercontent.com/gameherobrine2/test123/master/" +
        lowerName + ".png");
  }

  for (const auto &url : urls) {
    auto data = CrossPlatform_DownloadBinary(url);
    if (!data.empty() && data.size() > 64) {
      return data;
    }
  }
  return {};
}

void PlayerRenderer::setupPosition(Entity *a2_, float a3, float a4, float a5) {
  Player *a2 = (Player *)a2_;
  if (a2) {
    std::string nick = a2->username;
    if (nick.empty() && EntityRenderer::entityRenderDispatcher &&
        EntityRenderer::entityRenderDispatcher->minecraft) {
      if (a2 == EntityRenderer::entityRenderDispatcher->minecraft->player) {
        nick =
            EntityRenderer::entityRenderDispatcher->minecraft->options.username;
        if (a2->username.empty())
          a2->username = nick;
      }
    }

    if (!nick.empty()) {
      std::string texName = nick + "_skin";
      Textures *tex = EntityRenderer::entityRenderDispatcher->textures;

      if (tex && (tex->textures.find(texName) == tex->textures.end() ||
                  tex->textures[texName].glTexId == 0)) {
        mkdirPortable("skin_cache");
        std::string cachePath = "skin_cache/" + nick + ".png";
        int cw = 0, ch = 0, cch = 0;
        unsigned char *cachedPx =
            stbi_load(cachePath.c_str(), &cw, &ch, &cch, STBI_rgb_alpha);
        if (cachedPx) {
          uploadSkinTexture(nick, cachedPx, cw, ch);
          stbi_image_free(cachedPx);
        }
      }

      uint64_t now = getCurrentTimeMillis();
      bool doCheck = false;
      {
        std::lock_guard<std::mutex> lock(skinMutex);
        if (g_lastSkinCheckTime.find(nick) == g_lastSkinCheckTime.end() ||
            now - g_lastSkinCheckTime[nick] >= 15000) {
          g_lastSkinCheckTime[nick] = now;
          doCheck = true;
        }
      }

      if (doCheck) {
        std::thread([nick]() {
          auto bin = downloadSkinData(nick);
          if (!bin.empty()) {
            mkdirPortable("skin_cache");
            std::string cachePath = "skin_cache/" + nick + ".png";
            bool isDifferent = true;
            FILE *rf = fopen(cachePath.c_str(), "rb");
            if (rf) {
              fseek(rf, 0, SEEK_END);
              long sz = ftell(rf);
              fseek(rf, 0, SEEK_SET);
              if (sz == (long)bin.size()) {
                std::vector<unsigned char> diskBuf(sz);
                if (fread(diskBuf.data(), 1, sz, rf) == (size_t)sz) {
                  if (memcmp(diskBuf.data(), bin.data(), sz) == 0) {
                    isDifferent = false;
                  }
                }
              }
              fclose(rf);
            }

            if (isDifferent) {
              FILE *wf = fopen(cachePath.c_str(), "wb");
              if (wf) {
                fwrite(bin.data(), 1, bin.size(), wf);
                fclose(wf);
              }
            }

            int w, h, ch;
            unsigned char *px = stbi_load_from_memory(
                bin.data(), (int)bin.size(), &w, &h, &ch, STBI_rgb_alpha);
            if (px) {
              std::lock_guard<std::mutex> lock(skinMutex);
              if (g_pendingSkinsData.find(nick) != g_pendingSkinsData.end()) {
                stbi_image_free(g_pendingSkinsData[nick]);
              }
              g_pendingSkinsData[nick] = px;
              g_pendingSkinsDims[nick] = {w, h};
            }
          }
        }).detach();
      }

      {
        std::lock_guard<std::mutex> lock(skinMutex);
        auto it = g_pendingSkinsData.find(nick);
        if (it != g_pendingSkinsData.end()) {
          unsigned char *px = it->second;
          int w = g_pendingSkinsDims[nick].first;
          int h = g_pendingSkinsDims[nick].second;
          uploadSkinTexture(nick, px, w, h);
          stbi_image_free(px);
          g_pendingSkinsData.erase(it);
        }
      }

      if (tex && tex->textures.count(texName) &&
          tex->textures[texName].glTexId != 0) {
        a2->skin = texName;
      }
    }
  }

  float v11, v12;
  if (a2->isAlive() && a2->isSleeping()) {
    v11 = a3 + a2->field_CC4;
    v12 = a4 + a2->field_CC8;
    a5 = a5 + a2->field_CCC;
  } else {
    v11 = a3;
    v12 = a4;
  }
  MobRenderer::setupPosition(a2, v11, v12, a5);
}
void PlayerRenderer::setupRotations(Entity *a2_, float a3, float a4, float a5) {
  Player *a2 = (Player *)a2_;
  float sleepRot, v10;
  if (a2->isAlive() && a2->isSleeping()) {
    sleepRot = a2->getSleepRotation();
    glRotatef(sleepRot, 0.0, 1.0, 0.0);
    v10 = this->getFlipDegrees(a2);
    glRotatef(v10, 0.0, 0.0, 1.0);
    glRotatef(270.0, 0.0, 1.0, 0.0);
  } else {
    MobRenderer::setupRotations(a2, a3, a4, a5);
  }
}
int32_t PlayerRenderer::getArmorTransparencyMode() { return 3008; }
