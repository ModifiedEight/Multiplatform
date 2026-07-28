#include <tile/GrassPathTile.hpp>
#include <item/Item.hpp>
#include <level/Level.hpp>
#include <tile/material/Material.hpp>

GrassPathTile::GrassPathTile(int32_t id) : Tile(id, Material::dirt) {
	this->init();
	this->setDestroyTime(0.6f);
	this->setSoundType(Tile::SOUND_GRASS);
	this->topTexture = this->getTextureUVCoordinateSet("grass_path_top", 0);
	this->sideTexture = this->getTextureUVCoordinateSet("dirt", 0);
	this->setShape(0.0f, 0.0f, 0.0f, 1.0f, 15.0f/16.0f, 1.0f);
	this->setLightBlock(255);
	this->field_54 = 1;
	this->setDescriptionId("grassPath");
}

TextureUVCoordinateSet* GrassPathTile::getTexture(int32_t face, int32_t data) {
	if (face == 1) {
		return &this->topTexture;
	} else if (face == 0) {
		return Tile::dirt->getTexture(face, data); // bottom is dirt
	}
	return &this->sideTexture;
}

int32_t GrassPathTile::getResource(int32_t data, Random* rand) {
	return Tile::dirt->blockID; // Drops dirt
}

void GrassPathTile::neighborChanged(Level* level, int32_t x, int32_t y, int32_t z, int32_t neighborID, int32_t a6, int32_t a7, int32_t a8) {
	Tile::neighborChanged(level, x, y, z, neighborID, a6, a7, a8);
	int32_t aboveID = level->getTile(x, y + 1, z);
	if (Tile::solid[aboveID]) {
		level->setTileAndData(x, y, z, Tile::dirt->blockID, 0, 3);
	}
}

bool_t GrassPathTile::isSolidRender() {
	return 0;
}

bool_t GrassPathTile::isCubeShaped() {
	return 0;
}

AABB* GrassPathTile::getAABB(Level* level, int32_t x, int32_t y, int32_t z) {
	this->aabb.minX = x;
	this->aabb.minY = y;
	this->aabb.minZ = z;
	this->aabb.maxX = x + 1.0f;
	this->aabb.maxY = y + 1.0f;
	this->aabb.maxZ = z + 1.0f;
	return &this->aabb;
}

AABB GrassPathTile::getTileAABB(Level* level, int32_t x, int32_t y, int32_t z) {
	AABB ret;
	ret.minX = x;
	ret.minY = y;
	ret.minZ = z;
	ret.maxX = x + 1.0f;
	ret.maxY = y + 1.0f;
	ret.maxZ = z + 1.0f;
	return ret;
}

