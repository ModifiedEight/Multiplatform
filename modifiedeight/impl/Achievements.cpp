#include <Achievements.hpp>
#include <Minecraft.hpp>
#include <entity/Entity.hpp>
#include <entity/Player.hpp>
#include <entity/LocalPlayer.hpp>
#include <gui/Gui.hpp>
#include <item/ItemInstance.hpp>
#include <rendering/Tesselator.hpp>
#include <rendering/Textures.hpp>
#include <unigl.h>
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <vector>

namespace {
const int achievementCount = 15;

struct Popup {
  int id;
  int ticks;
};

int progress[achievementCount];
std::string progressPath;
Popup popup = {0, 0};
std::string loadedTexture;
uint32_t loadedTextureId = 0;

void saveToDisk() {
  if (progressPath.empty()) return;
  std::ofstream out(progressPath.c_str(), std::ios::trunc);
  if (!out) return;
  for (int value : progress) out << value << '\n';
}

void loadFromDisk() {
  std::fill(progress, progress + achievementCount, 1);
  if (progressPath.empty()) return;
  std::ifstream in(progressPath.c_str());
  for (int i = 0; i < achievementCount && in; ++i) {
    int value = 1;
    in >> value;
    progress[i] = value == 2 ? 2 : 1;
  }
}

void announce(Minecraft* minecraft, int id) {
  if (!minecraft) return;
  popup.id = id;
  popup.ticks = 240;
  saveToDisk();
}

bool hasEntityType(Entity* entity, int type) {
  return entity && entity->getEntityTypeId() == type;
}

bool isMonsterType(int type) {
  return type == 32 || type == 33 || type == 34 || type == 35 || type == 36;
}

bool matchesUpgrade(int itemId) {
  return itemId == 274 || itemId == 257 || itemId == 285 || itemId == 278;
}

struct Achievement {
  int id;
  const char* name;
};

const Achievement achievements[] = {
    {1, "Monster Hunter"},
    {2, "Cow Tipper"},
    {3, "Sniper Duel"},
    {4, "Getting Wood"},
    {5, "DIAMONDS!"},
    {6, "Benchmarking"},
    {7, "Hot Topic"},
    {8, "Time to Mine!"},
    {9, "Acquire Hardware"},
    {10, "Time to Farm!"},
    {11, "Bake Bread"},
    {12, "The Lie"},
    {13, "Getting an Upgrade"},
    {14, "Time to Strike!"},
    {15, "Librarian"},
};

}

namespace Achievements {
void load(const std::string& path) {
  if (!path.empty()) progressPath = path;
  loadFromDisk();
}

void save() {
  saveToDisk();
}

void tick() {
  if (popup.ticks > 0) --popup.ticks;
}

void reset() {
  std::fill(progress, progress + achievementCount, 1);
  popup.id = 0;
  popup.ticks = 0;
  saveToDisk();
}

bool executeCommand(Minecraft* minecraft, const std::string& line) {
  if (line.empty() || line[0] != '/') return false;
  std::istringstream input(line.substr(1));
  std::string command;
  input >> command;
  if (command == "save") {
    save();
    minecraft->gui.displayClientMessage("Saved Progress!");
    return true;
  }
  if (command == "load") {
    load(progressPath);
    minecraft->gui.displayClientMessage("Loaded Progress!");
    return true;
  }
  if (command == "reset") {
    reset();
    minecraft->gui.displayClientMessage("Resetted Progress!");
    return true;
  }
  return false;
}

void onAttack(Minecraft* minecraft, Entity* victim) {
  if (!minecraft || !victim) return;
  const int type = victim->getEntityTypeId();
  if (progress[0] == 1 && isMonsterType(type)) {
    progress[0] = 2;
    announce(minecraft, 1);
  }
  if (progress[1] == 1 && type == 11) {
    progress[1] = 2;
    announce(minecraft, 2);
  }
  ItemInstance* carried = minecraft->player ? minecraft->player->getCarriedItem() : nullptr;
  if (progress[2] == 1 && type == 34 && carried && carried->getId() == 261) {
    progress[2] = 2;
    announce(minecraft, 3);
  }
}

void onDestroyBlock(Minecraft* minecraft, int blockId, int carriedItemId, bool destroyed) {
  if (!minecraft || !destroyed) return;
  if (progress[3] == 1 && blockId == 17) {
    progress[3] = 2;
    announce(minecraft, 4);
  }
  if (progress[4] == 1 && blockId == 56 && carriedItemId == 257) {
    progress[4] = 2;
    announce(minecraft, 5);
  }
}

void onUseItem(Minecraft* minecraft, int itemId) {
  if (!minecraft) return;
  for (int i = 5; i < achievementCount; ++i) {
    if (progress[i] != 1) continue;
    bool match = false;
    if (i == 5) match = itemId == 58;
    if (i == 6) match = itemId == 61;
    if (i == 7) match = itemId == 270;
    if (i == 8) match = itemId == 265;
    if (i == 9) match = itemId == 290;
    if (i == 10) match = itemId == 297;
    if (i == 11) match = itemId == 354;
    if (i == 12) match = matchesUpgrade(itemId);
    if (i == 13) match = itemId == 268;
    if (i == 14) match = itemId == 47;
    if (match) {
      progress[i] = 2;
      announce(minecraft, i + 1);
    }
  }
}

void render(Minecraft* minecraft) {
  if (!minecraft || !minecraft->texturesPtr || popup.ticks <= 0 || popup.id < 1 || popup.id > achievementCount) return;
  const float width = 243.0f;
  const float height = 48.0f;
  const float guiWidth = (float)minecraft->field_1C * Gui::InvGuiScale;
  const float x = (guiWidth - width) * 0.5f;
  const float alpha = popup.ticks < 40 ? popup.ticks / 40.0f : 1.0f;
  const std::string texture = "achievements/ac" + std::to_string(popup.id) + ".png";
  if (texture != loadedTexture) {
    loadedTextureId = minecraft->texturesPtr->loadAndBindTexture(texture);
    loadedTexture = texture;
  } else {
    minecraft->texturesPtr->bind(loadedTextureId);
  }
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_DEPTH_TEST);
  glDepthMask(0);
  glColor4f(1.0f, 1.0f, 1.0f, alpha);
  Tesselator::instance.begin(7);
  Tesselator::instance.vertexUV(x, height, -90, 0, 1);
  Tesselator::instance.vertexUV(x + width, height, -90, 1, 1);
  Tesselator::instance.vertexUV(x + width, 0, -90, 1, 0);
  Tesselator::instance.vertexUV(x, 0, -90, 0, 0);
  Tesselator::instance.draw(1);
  glDepthMask(1);
  glEnable(GL_DEPTH_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}
}
