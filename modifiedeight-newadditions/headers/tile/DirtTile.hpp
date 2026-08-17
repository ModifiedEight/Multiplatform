#pragma once
#include <tile/Tile.hpp>

struct DirtTile : Tile{
	DirtTile(int32_t, std::string, Material*);
	virtual ~DirtTile();
	virtual int32_t getColor(LevelSource*, int32_t, int32_t, int32_t);
};
