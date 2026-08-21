#include <tile/WaterLilyTile.hpp>
#include <level/LevelHeight.hpp>
#include <tile/material/Material.hpp>
#include <level/Level.hpp>
#include <level/LevelSource.hpp>

WaterLilyTile::WaterLilyTile(int32_t id, const std::string& textureName)
	: Tile(id, textureName, Material::plant) {
	this->setShape(0.0f, 0.0f, 0.0f, 1.0f, 0.015625f, 1.0f);
}

WaterLilyTile::~WaterLilyTile() {
}

int32_t WaterLilyTile::getRenderLayer() {
	return 1;
}

int32_t WaterLilyTile::getRenderShape() {
	return 23;
}

int32_t WaterLilyTile::getColor(int32_t) {
	return 0x529141;
}

int32_t WaterLilyTile::getColor(LevelSource* level, int32_t x, int32_t y, int32_t z) {
	return 0x529141;
}

bool_t WaterLilyTile::isSolid() {
	return 0;
}

bool_t WaterLilyTile::isSolidRender() {
	return 0;
}

bool_t WaterLilyTile::isCubeShaped() {
	return 0;
}

bool_t WaterLilyTile::canSurvive(Level* level, int32_t x, int32_t y, int32_t z) {
	if (y <= 0 || y >= LevelHeight::height) return 0;
	int32_t belowTile = level->getTile(x, y - 1, z);
	Material* belowMat = level->getMaterial(x, y - 1, z);
	return (belowTile == Tile::water->blockID || belowTile == Tile::calmWater->blockID || (belowMat && belowMat == Material::water));
}

bool_t WaterLilyTile::mayPlace(Level* level, int32_t x, int32_t y, int32_t z) {
	if (y <= 0 || y >= LevelHeight::height) return 0;
	int32_t cur = level->getTile(x, y, z);
	if (cur != 0) return 0;
	return this->canSurvive(level, x, y, z);
}

void WaterLilyTile::neighborChanged(Level* level, int32_t x, int32_t y, int32_t z, int32_t side) {
	if (!this->canSurvive(level, x, y, z)) {
		this->spawnResources(level, x, y, z, level->getData(x, y, z), 0);
		level->setTile(x, y, z, 0, 3);
	}
}

AABB* WaterLilyTile::getAABB(Level* level, int32_t x, int32_t y, int32_t z) {
	this->aabb.minX = (float)x + 0.0f;
	this->aabb.minY = (float)y + 0.0f;
	this->aabb.minZ = (float)z + 0.0f;
	this->aabb.maxX = (float)x + 1.0f;
	this->aabb.maxY = (float)y + 0.015625f;
	this->aabb.maxZ = (float)z + 1.0f;
	return &this->aabb;
}

void WaterLilyTile::getVisualShape(LevelSource* level, int32_t x, int32_t y, int32_t z, AABB& aabb, bool_t a6) {
	this->setShape(0.0f, 0.0f, 0.0f, 1.0f, 0.015625f, 1.0f);
}
