#pragma once
#include <tile/Bush.hpp>

struct SweetBerryBushTile : Bush {
	TextureUVCoordinateSet stages[4];

	SweetBerryBushTile(int32_t id, const std::string& name);
	virtual ~SweetBerryBushTile();
	virtual TextureUVCoordinateSet* getTexture(int32_t face, int32_t data);
	virtual TextureUVCoordinateSet* getCarriedTexture(int32_t face, int32_t data);
	virtual AABB* getAABB(Level* level, int32_t x, int32_t y, int32_t z);
	virtual bool_t isSolidRender();
	virtual bool_t isCubeShaped();
	virtual int32_t getRenderLayer();
	virtual int32_t getRenderShape();
	virtual void entityInside(Level* level, int32_t x, int32_t y, int32_t z, Entity* ent);
	virtual bool_t use(Level* level, int32_t x, int32_t y, int32_t z, Player* player);
	virtual void tick(Level* level, int32_t x, int32_t y, int32_t z, Random* rand);
	virtual int32_t getResource(int32_t data, Random* rand);
	virtual int32_t getResourceCount(Random* rand);
	virtual int32_t getSpawnResourcesAuxValue(int32_t data);
	virtual bool_t mayPlaceOn(int32_t tileId);
};
