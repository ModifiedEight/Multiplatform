#include <tile/entity/MixedSlabTileEntity.hpp>
#include <nbt/CompoundTag.hpp>

MixedSlabTileEntity::MixedSlabTileEntity()
	: TileEntity(5) {
	this->mode = 0;
	this->bottomTileId = 0;
	this->bottomAux = 0;
	this->topTileId = 0;
	this->topAux = 0;
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
}

bool_t MixedSlabTileEntity::save(CompoundTag* tag) {
	if (TileEntity::save(tag)) {
		tag->putInt("Mode", this->mode);
		tag->putInt("BottomTileId", this->bottomTileId);
		tag->putInt("BottomAux", this->bottomAux);
		tag->putInt("TopTileId", this->topTileId);
		tag->putInt("TopAux", this->topAux);
		return 1;
	}
	return 0;
}
