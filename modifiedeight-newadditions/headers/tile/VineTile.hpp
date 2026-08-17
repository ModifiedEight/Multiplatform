#pragma once

#include <tile/Tile.hpp>

class VineTile : public Tile {
public:
	VineTile(int32_t id, const std::string& name);
	virtual ~VineTile();

	virtual bool_t isCubeShaped();
	virtual bool_t isSolidRender();
	virtual int32_t getRenderShape();
	virtual int32_t getRenderLayer();
	virtual AABB* getAABB(Level* level, int32_t x, int32_t y, int32_t z);
	virtual int32_t getColor(int32_t data);
	virtual int32_t getColor(LevelSource* level, int32_t x, int32_t y, int32_t z);
	virtual bool_t mayPlace(Level* level, int32_t x, int32_t y, int32_t z, uint8_t side);
	virtual int32_t getPlacementDataValue(Level* level, int32_t x, int32_t y, int32_t z, int32_t side, float fx, float fy, float fz, Mob* mob, int32_t meta);
	virtual bool_t canSurvive(Level* level, int32_t x, int32_t y, int32_t z);
	virtual void neighborChanged(Level* level, int32_t x, int32_t y, int32_t z, int32_t fromX, int32_t fromY, int32_t fromZ, int32_t fromTileId);
	virtual void tick(Level* level, int32_t x, int32_t y, int32_t z, Random* random);
	virtual int32_t getResource(int32_t meta, Random* random);
	virtual int32_t getResourceCount(Random* random);
	virtual bool_t onFertilized(Level* level, int32_t x, int32_t y, int32_t z);

	static bool_t canAttachTo(LevelSource* level, int32_t x, int32_t y, int32_t z);
};
