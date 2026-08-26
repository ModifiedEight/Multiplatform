#pragma once
#include <tile/Tile.hpp>

struct SoulSandTile : Tile {
	SoulSandTile(int32_t id, const std::string& name);
	virtual ~SoulSandTile();
	virtual AABB* getAABB(Level*, int32_t, int32_t, int32_t) override;
	virtual void entityInside(Level*, int32_t, int32_t, int32_t, Entity*) override;
};
