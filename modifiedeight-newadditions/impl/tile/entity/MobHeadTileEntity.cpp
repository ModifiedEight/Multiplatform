#include <tile/entity/MobHeadTileEntity.hpp>
#include <nbt/CompoundTag.hpp>

MobHeadTileEntity::MobHeadTileEntity()
	: TileEntity(7)
	, rotation(0)
	, headType(0) {
	this->renderId = TER_SKULL;
}

MobHeadTileEntity::MobHeadTileEntity(int32_t headType, int32_t rotation)
	: TileEntity(7)
	, rotation(rotation)
	, headType(headType) {
	this->renderId = TER_SKULL;
}

MobHeadTileEntity::~MobHeadTileEntity() {
}

bool_t MobHeadTileEntity::shouldSave() {
	return 1;
}

void MobHeadTileEntity::load(CompoundTag* tag) {
	TileEntity::load(tag);
	this->rotation = tag->getByte("Rot");
	this->headType = tag->getByte("SkullType");
}

bool_t MobHeadTileEntity::save(CompoundTag* tag) {
	bool_t res = TileEntity::save(tag);
	tag->putByte("Rot", (int8_t)this->rotation);
	tag->putByte("SkullType", (int8_t)this->headType);
	return res;
}
