#pragma once
#include <tile/Tile.hpp>
#include <rendering/TextureAtlasTextureItem.hpp>

struct ColoredPlanksTile : Tile {
	TextureAtlasTextureItem tex;
	ColoredPlanksTile(int32_t);

	virtual ~ColoredPlanksTile();
	virtual TextureUVCoordinateSet* getTexture(int32_t, int32_t);
	virtual int32_t getSpawnResourcesAuxValue(int32_t);
};
