#pragma once
#include <tile/Tile.hpp>
#include <rendering/TextureUVCoordinateSet.hpp>

struct MyceliumTile : public Tile {
	TextureUVCoordinateSet topUV;
	TextureUVCoordinateSet sideUV;
	TextureUVCoordinateSet bottomUV;
	TextureUVCoordinateSet snowSideUV;

	MyceliumTile(int32_t id);
	virtual ~MyceliumTile();

	virtual TextureUVCoordinateSet* getTexture(int32_t face, int32_t data);
	virtual TextureUVCoordinateSet* getTexture(LevelSource* level, int32_t x, int32_t y, int32_t z, int32_t face);
	virtual TextureUVCoordinateSet* getCarriedTexture(int32_t face, int32_t data);
	virtual void tick(Level* level, int32_t x, int32_t y, int32_t z, Random* random);
	virtual int32_t getResource(int32_t data, Random* random);
	virtual int32_t getColor(int32_t data);
	virtual int32_t getColor(LevelSource* level, int32_t x, int32_t y, int32_t z);
	virtual bool_t onFertilized(Level* level, int32_t x, int32_t y, int32_t z);
};

