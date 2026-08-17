#include <tile/entity/MixedSlabTileEntity.hpp>
#include <nbt/CompoundTag.hpp>

MixedSlabTileEntity::MixedSlabTileEntity()
	: TileEntity(5) {
	this->mode = 0;
	this->bottomTileId = 0;
	this->bottomAux = 0;
	this->topTileId = 0;
	this->topAux = 0;
	this->bottomColor = 0xFFFFFF;
	this->topColor = 0xFFFFFF;
}

MixedSlabTileEntity::~MixedSlabTileEntity() {
}

bool_t MixedSlabTileEntity::shouldSave() {
	return 1;
}

void MixedSlabTileEntity::load(CompoundTag* tag) {
	TileEntity::load(tag);
	this->mode = tag->getInt("Mode");
	this->bottomTileId = tag->getInt("BottomTileId");
	this->bottomAux = tag->getInt("BottomAux");
	this->topTileId = tag->getInt("TopTileId");
	this->topAux = tag->getInt("TopAux");
	if (tag->contains("BottomColor")) this->bottomColor = (uint32_t)tag->getInt("BottomColor");
	else this->bottomColor = 0xFFFFFF;
	if (tag->contains("TopColor")) this->topColor = (uint32_t)tag->getInt("TopColor");
	else this->topColor = 0xFFFFFF;
}

bool_t MixedSlabTileEntity::save(CompoundTag* tag) {
	if (TileEntity::save(tag)) {
		tag->putInt("Mode", this->mode);
		tag->putInt("BottomTileId", this->bottomTileId);
		tag->putInt("BottomAux", this->bottomAux);
		tag->putInt("TopTileId", this->topTileId);
		tag->putInt("TopAux", this->topAux);
		tag->putInt("BottomColor", (int32_t)this->bottomColor);
		tag->putInt("TopColor", (int32_t)this->topColor);
		return 1;
	}
	return 0;
}
