#pragma once

#include <string>

struct Entity;
struct Minecraft;

namespace Achievements {
void load(const std::string& path);
void save();
void tick();
void reset();
bool executeCommand(Minecraft* minecraft, const std::string& line);
void onAttack(Minecraft* minecraft, Entity* victim);
void onDestroyBlock(Minecraft* minecraft, int blockId, int carriedItemId, bool destroyed);
void onUseItem(Minecraft* minecraft, int itemId);
void onUseBlock(Minecraft* minecraft, int blockId);
void render(Minecraft* minecraft);
}
