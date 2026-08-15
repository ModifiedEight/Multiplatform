#pragma once
#include <tile/EntityTile.hpp>

class MixedSlabTile : public EntityTile {
public:
	MixedSlabTile(int32_t id);
	virtual ~MixedSlabTile();
	virtual TileEntity* newTileEntity();
	virtual int32_t getTileEntityType();
	virtual int32_t getRenderShape();
	virtual void updateShape(LevelSource*, int32_t, int32_t, int32_t);
	virtual void addAABBs(Level*, int32_t, int32_t, int32_t, const AABB*, std::vector<AABB>&);
	virtual void playerDestroy(Level* level, Player* player, int32_t x, int32_t y, int32_t z, int32_t meta);
	virtual int32_t getResource(int32_t meta, Random* random);
	virtual int32_t getResourceCount(Random* random);
	virtual bool_t isSolidRender();
	virtual bool_t isCubeShaped();
};
