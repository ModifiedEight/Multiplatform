#pragma once
#include <tile/Tile.hpp>
#include <rendering/TextureAtlasTextureItem.hpp>

struct ColoredBricksTile : Tile {
	TextureAtlasTextureItem tex;
	ColoredBricksTile(int32_t);

	virtual ~ColoredBricksTile();
	virtual TextureUVCoordinateSet* getTexture(int32_t, int32_t);
	virtual int32_t getSpawnResourcesAuxValue(int32_t);
};
