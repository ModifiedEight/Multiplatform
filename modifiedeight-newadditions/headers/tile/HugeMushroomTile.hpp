#pragma once
#include <tile/Tile.hpp>
#include <rendering/TextureUVCoordinateSet.hpp>

struct HugeMushroomTile : public Tile {
	int32_t mushroomType;
	TextureUVCoordinateSet skinTexture;
	TextureUVCoordinateSet stemTexture;
	TextureUVCoordinateSet insideTexture;

	HugeMushroomTile(int32_t id, int32_t type);
	virtual ~HugeMushroomTile();

	virtual TextureUVCoordinateSet* getTexture(int32_t face, int32_t data);
	virtual TextureUVCoordinateSet* getTexture(LevelSource* level, int32_t x, int32_t y, int32_t z, int32_t face);
	virtual TextureUVCoordinateSet* getCarriedTexture(int32_t face, int32_t data);
	virtual int32_t getResource(int32_t data, Random* random);
	virtual int32_t getResourceCount(Random* random);
};

