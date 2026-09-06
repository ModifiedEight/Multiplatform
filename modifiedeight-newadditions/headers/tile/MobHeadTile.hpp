#pragma once
#include <tile/EntityTile.hpp>

struct MobHeadTile: EntityTile {
	int32_t headType;

	MobHeadTile(int32_t id, int32_t headType, const std::string& name);
	virtual ~MobHeadTile();

	virtual bool_t isCubeShaped();
	virtual bool_t isSolidRender();
	virtual int32_t getRenderShape();
	virtual int32_t getTileEntityType();
	virtual int32_t getPlacementDataValue(Level*, int32_t, int32_t, int32_t, int32_t, float, float, float, struct Mob*, int32_t);
	virtual int32_t getResource(int32_t, Random*);
	virtual int32_t getResourceCount(Random*);
	virtual AABB* getAABB(Level*, int32_t, int32_t, int32_t);
	virtual void updateShape(LevelSource*, int32_t, int32_t, int32_t);

	static bool isHeadBlock(int32_t id);
	static int32_t getHeadType(int32_t id);
};
