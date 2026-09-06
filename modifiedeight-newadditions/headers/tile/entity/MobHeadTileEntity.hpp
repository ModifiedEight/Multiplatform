#pragma once
#include <tile/entity/TileEntity.hpp>

struct MobHeadTileEntity: TileEntity {
	int32_t rotation;
	int32_t headType;

	MobHeadTileEntity();
	MobHeadTileEntity(int32_t headType, int32_t rotation);
	virtual ~MobHeadTileEntity();

	virtual bool_t shouldSave();
	virtual void load(CompoundTag*);
	virtual bool_t save(CompoundTag*);
};
