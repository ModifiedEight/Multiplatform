#include <tile/EnderChestTile.hpp>
#include <entity/Player.hpp>
#include <level/Level.hpp>
#include <tile/entity/EnderChestTileEntity.hpp>
#include <entity/ItemEntity.hpp>
#include <tile/material/Material.hpp>
#include <rendering/TextureAtlasTextureItem.hpp>
#include <entity/particles/ParticleType.hpp>

EnderChestTile::EnderChestTile(int32_t id)
	: EntityTile(id, Material::stone) {
	this->setShape(0.0625f, 0.0f, 0.0625f, 0.9375f, 0.875f, 0.9375f);
	this->setDestroyTime(22.5f);
	this->setExplodeable(600.0f);
	this->setSoundType(Tile::SOUND_STONE);
	TextureAtlasTextureItem* topItem = this->getTextureItem("ender_chest_top");
	this->field_A50 = topItem ? *topItem->getUV(0) : TextureUVCoordinateSet();
	TextureAtlasTextureItem* sideItem = this->getTextureItem("ender_chest_side");
	this->field_A68 = sideItem ? *sideItem->getUV(0) : TextureUVCoordinateSet();
	TextureAtlasTextureItem* frontItem = this->getTextureItem("ender_chest_front");
	this->field_A80 = frontItem ? *frontItem->getUV(0) : TextureUVCoordinateSet();
}

EnderChestTile::~EnderChestTile() {
}

bool_t EnderChestTile::isCubeShaped() {
	return 0;
}

int32_t EnderChestTile::getRenderShape() {
	return 22;
}

TextureUVCoordinateSet* EnderChestTile::getTexture(int32_t a2) {
	if (this->field_A50.minX == 0.0f && this->field_A50.maxX == 0.0f) {
		TextureAtlasTextureItem* topItem = this->getTextureItem("ender_chest_top");
		if (topItem) this->field_A50 = *topItem->getUV(0);
		TextureAtlasTextureItem* sideItem = this->getTextureItem("ender_chest_side");
		if (sideItem) this->field_A68 = *sideItem->getUV(0);
		TextureAtlasTextureItem* frontItem = this->getTextureItem("ender_chest_front");
		if (frontItem) this->field_A80 = *frontItem->getUV(0);
	}
	if ((uint32_t)a2 <= 1) return &this->field_A50;
	if (a2 == 3) return &this->field_A80;
	return &this->field_A68;
}

TextureUVCoordinateSet* EnderChestTile::getTexture(LevelSource* ls, int32_t x, int32_t y, int32_t z, int32_t a6) {
	if (this->field_A50.minX == 0.0f && this->field_A50.maxX == 0.0f) {
		TextureAtlasTextureItem* topItem = this->getTextureItem("ender_chest_top");
		if (topItem) this->field_A50 = *topItem->getUV(0);
		TextureAtlasTextureItem* sideItem = this->getTextureItem("ender_chest_side");
		if (sideItem) this->field_A68 = *sideItem->getUV(0);
		TextureAtlasTextureItem* frontItem = this->getTextureItem("ender_chest_front");
		if (frontItem) this->field_A80 = *frontItem->getUV(0);
	}
	if ((uint32_t)a6 <= 1) return &this->field_A50;
	if (a6 == ls->getData(x, y, z)) return &this->field_A80;
	return &this->field_A68;
}

TextureUVCoordinateSet* EnderChestTile::getCarriedTexture(int32_t a2, int32_t a3) {
	return this->getTexture(a2);
}

bool_t EnderChestTile::isSolidRender() {
	return 0;
}

int32_t EnderChestTile::getTileEntityType() {
	return 6;
}

void EnderChestTile::onRemove(Level* level, int32_t x, int32_t y, int32_t z) {
	EntityTile::onRemove(level, x, y, z);
}

bool_t EnderChestTile::use(Level* level, int32_t x, int32_t y, int32_t z, Player* player) {
	TileEntity* te = level->getTileEntity(x, y, z);
	if (te) {
		if (!level->isClientMaybe) {
			((EnderChestTileEntity*)te)->openBy(player);
		}
	}
	return 1;
}

int32_t EnderChestTile::getPlacementDataValue(Level*, int32_t, int32_t, int32_t, int32_t, float, float, float, struct Mob* a10, int32_t a11) {
	float v11 = ((a10->yaw * 4) / 360.0f) + 0.5f;
	int32_t v12 = (int32_t)v11;
	if (v11 < (float)(int32_t)v11) v12 = v12 - 1;
	switch (v12 & 3) {
		case 0: return a11 | 2;
		case 1: return a11 | 5;
		case 2: return a11 | 3;
	}
	return a11 | 4;
}

int32_t EnderChestTile::getResource(int32_t, Random*) {
	return Tile::obsidian ? Tile::obsidian->blockID : 49;
}

int32_t EnderChestTile::getResourceCount(Random*) {
	return 8;
}

void EnderChestTile::animateTick(Level* level, int32_t x, int32_t y, int32_t z, Random* random) {
	for (int i = 0; i < 3; ++i) {
		int sign = random->genrand_int32() % 2 == 0 ? -1 : 1;
		int sign2 = random->genrand_int32() % 2 == 0 ? -1 : 1;
		double px = (double)x + 0.5 + 0.25 * (double)sign;
		double py = (double)((float)y + random->nextFloat());
		double pz = (double)z + 0.5 + 0.25 * (double)sign2;
		double mx = (double)(random->nextFloat() * (float)sign);
		double my = ((double)random->nextFloat() - 0.5) * 0.125;
		double mz = (double)(random->nextFloat() * (float)sign2);
		level->addParticle(PT_MOB_FLAME, px, py, pz, mx, my, mz, 0);
	}
}
