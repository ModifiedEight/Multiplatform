#include <tile/WallTile.hpp>
#include <level/Level.hpp>
#include <tile/material/Material.hpp>

WallTile::WallTile(int32_t id, Tile* a3)
	: Tile(id, a3->material) {
	this->setDestroyTime(a3->blockResistance / 3.0);
	this->setSoundType(*a3->soundType);
}
bool_t WallTile::connectsTo(LevelSource* level, int32_t x, int32_t y, int32_t z) {
	int32_t v6 = level->getTile(x, y, z);
	if(v6 == this->blockID || (Tile::cobbleWall && v6 == Tile::cobbleWall->blockID) || v6 == Tile::fenceGate->blockID) {
		return 1;
	}
	Tile* v7 = Tile::tiles[v6];
	if(!v7) {
		return 0;
	}
	if(v7->material->isSolidBlocking() && v7->isCubeShaped()) {
		return v7->material != Material::vegetable;
	}
	return 0;
}

WallTile::~WallTile() {
}
bool_t WallTile::isCubeShaped() {
	return 0;
}
int32_t WallTile::getRenderShape() {
	return 32;
}
void WallTile::updateShape(LevelSource* level, int32_t x, int32_t y, int32_t z) {
	bool_t zNeg = this->connectsTo(level, x, y, z - 1);
	bool_t zPos = this->connectsTo(level, x, y, z + 1);
	bool_t xNeg = this->connectsTo(level, x - 1, y, z);
	bool_t xPos = this->connectsTo(level, x + 1, y, z);
	float minX = 0.25f;
	float maxX = 0.75f;
	float minZ = 0.25f;
	float maxZ = 0.75f;
	if(zNeg) minZ = 0.0f;
	if(zPos) maxZ = 1.0f;
	if(xNeg) minX = 0.0f;
	if(xPos) maxX = 1.0f;
	this->setShape(minX, 0.0f, minZ, maxX, 1.0f, maxZ);
}
bool_t WallTile::shouldRenderFace(LevelSource* level, int32_t x, int32_t y, int32_t z, int32_t a6) {
	return a6 || Tile::shouldRenderFace(level, x, y, z, 0);
}
TextureUVCoordinateSet* WallTile::getTexture(int32_t a2, int32_t a3) {
	if(a3 == 1) return Tile::mossStone->getTexture(a2);
	return Tile::stoneBrick->getTexture(a2);
}
AABB* WallTile::getAABB(Level* level, int32_t x, int32_t y, int32_t z) {
	this->updateShape(level, x, y, z);
	this->aabb.minX = (float)x + this->minX;
	this->aabb.maxX = (float)x + this->maxX;
	this->aabb.minY = (float)y;
	this->aabb.minZ = (float)z + this->minZ;
	this->aabb.maxZ = (float)z + this->maxZ;
	this->aabb.maxY = (float)y + 1.5f;
	return &this->aabb;
}
bool_t WallTile::isSolidRender() {
	return 0;
}
int32_t WallTile::getSpawnResourcesAuxValue(int32_t a2) {
	return a2;
}
