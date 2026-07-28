#pragma once

#include <tile/Tile.hpp>

struct GrassPathTile : Tile {
	TextureUVCoordinateSet topTexture;
	TextureUVCoordinateSet sideTexture;

	GrassPathTile(int32_t);

	virtual TextureUVCoordinateSet* getTexture(int32_t, int32_t);
	virtual int32_t getResource(int32_t, Random*);
	virtual void neighborChanged(Level*, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t);
	virtual bool_t isSolidRender();
	virtual bool_t isCubeShaped();
	virtual AABB* getAABB(Level*, int32_t, int32_t, int32_t);
	virtual AABB getTileAABB(Level*, int32_t, int32_t, int32_t);
};
