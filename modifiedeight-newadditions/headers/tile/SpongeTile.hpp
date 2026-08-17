#pragma once
#include <tile/Tile.hpp>

struct SpongeTile : Tile {
	SpongeTile(int32_t);
	virtual ~SpongeTile();
	virtual void onPlace(Level*, int32_t, int32_t, int32_t);
	void _absorbWater(Level*, int32_t, int32_t, int32_t);
};
