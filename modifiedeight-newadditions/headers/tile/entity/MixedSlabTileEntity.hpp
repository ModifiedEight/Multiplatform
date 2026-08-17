#pragma once
#include <tile/entity/TileEntity.hpp>

struct MixedSlabTileEntity : TileEntity {
	int32_t mode;
	int32_t bottomTileId;
	int32_t bottomAux;
	int32_t topTileId;
	int32_t topAux;
	uint32_t bottomColor;
	uint32_t topColor;

	MixedSlabTileEntity();
	virtual ~MixedSlabTileEntity();
	virtual bool_t shouldSave();
	virtual void load(struct CompoundTag*);
	virtual bool_t save(struct CompoundTag*);
};
