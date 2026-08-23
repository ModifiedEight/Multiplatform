#include <tile/StonecutterTile.hpp>
#include <entity/Player.hpp>
#include <tile/material/Material.hpp>
#include <rendering/TextureAtlasTextureItem.hpp>
#include <level/LevelSource.hpp>


StonecutterTile::StonecutterTile(int32_t a2)
	: Tile(a2, Material::stone) {
	TextureAtlasTextureItem* texItem = this->getTextureItem("stonecutter");
	this->field_80 = *texItem->getUV(0);
	this->field_98 = *texItem->getUV(1);
	this->field_B0 = *texItem->getUV(2);
	this->field_C8 = *texItem->getUV(3);
}

StonecutterTile::~StonecutterTile() {
}
TextureUVCoordinateSet* StonecutterTile::getTexture(int32_t a2) {
	if(a2 == 1) {
		return &this->field_B0;
	}
	if(!a2) {
		return &this->field_C8;
	}
	if(a2 == 3) {
		return &this->field_80;
	}
	return &this->field_98;
}
TextureUVCoordinateSet* StonecutterTile::getTexture(LevelSource* level, int32_t x, int32_t y, int32_t z, int32_t face) {
	if (face == 1) return &this->field_B0;
	if (face == 0) return &this->field_C8;
	int32_t data = level ? (level->getData(x, y, z) & 7) : 3;
	if (data < 2 || data > 5) data = 3;
	if (face == data) {
		return &this->field_80;
	}
	return &this->field_98;
}
int32_t StonecutterTile::getPlacementDataValue(Level* level, int32_t x, int32_t y, int32_t z, int32_t face, float a7, float a8, float a9, Mob* mob, int32_t a11) {
	if (!mob) return 3;
	float v11 = ((mob->yaw * 4.0f) / 360.0f) + 0.5f;
	int32_t v12 = (int32_t)v11;
	if (v11 < (float)v12) v12 -= 1;
	switch(v12 & 3) {
		case 0: return 2;
		case 1: return 5;
		case 2: return 3;
		case 3: return 4;
	}
	return 3;
}
bool_t StonecutterTile::use(Level* level, int32_t x, int32_t y, int32_t z, Player* player) {
	player->startStonecutting(x, y, z);
	return 1;
}
