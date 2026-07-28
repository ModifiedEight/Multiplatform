#pragma once
#include <tile/RotatedPillarTile.hpp>
#include <rendering/TextureAtlasTextureItem.hpp>

struct ColoredLogTile : RotatedPillarTile {
	TextureAtlasTextureItem texSide;
	TextureAtlasTextureItem texTop;
	int32_t colorIndex;

	ColoredLogTile(int32_t id, int32_t colorIndex);

	virtual ~ColoredLogTile();
	virtual int32_t getResource(int32_t, Random*);
	virtual int32_t getResourceCount(Random*);
	virtual int32_t getSpawnResourcesAuxValue(int32_t);
	virtual TextureUVCoordinateSet* getDirTexture(int32_t dir, int32_t data);
};
