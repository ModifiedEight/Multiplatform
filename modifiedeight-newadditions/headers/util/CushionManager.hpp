#pragma once
#include <_types.h>
#include <cstdint>
#include <string>

struct Level;
struct LevelSource;
struct Player;
struct ItemInstance;
struct Tile;
struct TileRenderer;

class CushionManager {
public:
	static bool isCushionableTile(Tile* tile, int meta);
	static bool isStairTile(Tile* tile);
	static bool isSlabTile(Tile* tile);
	static bool hasCushion(LevelSource* level, int x, int y, int z);
	static int getCushionColor(LevelSource* level, int x, int y, int z);
	static bool setCushion(Level* level, int x, int y, int z, int color);
	static bool removeCushion(Level* level, int x, int y, int z);
	static void clear();
	static bool handleUse(Player* player, Level* level, int x, int y, int z, int face, float faceX, float faceY, float faceZ, ItemInstance* sel);
	static void renderCushion(TileRenderer* renderer, Tile* tile, int x, int y, int z);
	static void onTileDestroyed(Level* level, int x, int y, int z);
	static void save(const std::string& dir);
	static void load(const std::string& dir);
};
