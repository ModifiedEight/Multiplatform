#pragma once
#include <tile/Bush.hpp>
#include <rendering/TextureUVCoordinateSet.hpp>

struct DoublePlantTile : Bush {
	TextureUVCoordinateSet texGrassBottom;
	TextureUVCoordinateSet texGrassTop;
	TextureUVCoordinateSet texFernBottom;
	TextureUVCoordinateSet texFernTop;
	TextureUVCoordinateSet texPaeoniaBottom;
	TextureUVCoordinateSet texPaeoniaTop;
	TextureUVCoordinateSet texRoseBottom;
	TextureUVCoordinateSet texRoseTop;

	DoublePlantTile(int32_t id, const std::string& name);
	virtual ~DoublePlantTile();

	virtual TextureUVCoordinateSet* getTexture(int32_t face, int32_t data);
	virtual TextureUVCoordinateSet* getCarriedTexture(int32_t face, int32_t data);
	virtual int32_t getColor(int32_t data);
	virtual int32_t getColor(LevelSource* level, int32_t x, int32_t y, int32_t z);
	virtual bool_t canSurvive(Level* level, int32_t x, int32_t y, int32_t z);
	virtual bool_t mayPlace(Level* level, int32_t x, int32_t y, int32_t z, uint8_t side);
	virtual void neighborChanged(Level* level, int32_t x, int32_t y, int32_t z, int32_t fromX, int32_t fromY, int32_t fromZ, int32_t fromTileId);
	virtual void playerDestroy(Level* level, Player* player, int32_t x, int32_t y, int32_t z, int32_t meta);
	virtual int32_t getSpawnResourcesAuxValue(int32_t meta);
	virtual int32_t getResource(int32_t meta, Random* random);
	virtual int32_t getResourceCount(Random* random);
};
