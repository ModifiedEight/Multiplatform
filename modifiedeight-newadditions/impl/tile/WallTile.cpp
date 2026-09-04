#include <tile/WallTile.hpp>
#include <level/Level.hpp>
#include <tile/material/Material.hpp>

WallTile::WallTile(int32_t id, Tile* a3)
	: Tile(id, a3->material) {
	this->baseTile = a3;
	this->setDestroyTime(a3->blockResistance / 3.0);
	this->setSoundType(*a3->soundType);
}
bool_t WallTile::connectsTo(LevelSource* level, int32_t x, int32_t y, int32_t z) {
	int32_t v6; // r0
	Tile* v7;	// r4

	v6 = level->getTile(x, y, z);
	if(v6 == this->blockID || (Tile::tiles[v6] && Tile::tiles[v6]->getRenderShape() == 32) || v6 == Tile::fenceGate->blockID) {
		return 1;
	}
	v7 = Tile::tiles[v6];
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

	float minX = xNeg ? 0.0f : 0.25f;
	float maxX = xPos ? 1.0f : 0.75f;
	float minZ = zNeg ? 0.0f : 0.25f;
	float maxZ = zPos ? 1.0f : 0.75f;

	this->setShape(minX, 0.0f, minZ, maxX, 1.0f, maxZ);
}
bool_t WallTile::shouldRenderFace(LevelSource* level, int32_t x, int32_t y, int32_t z, int32_t a6) {
	return a6 || Tile::shouldRenderFace(level, x, y, z, 0);
}
TextureUVCoordinateSet* WallTile::getTexture(int32_t a2, int32_t a3) {
	if(this->baseTile == Tile::stoneBrick || this->blockID == 139) {
		if(a3 == 1 && Tile::mossStone) return Tile::mossStone->getTexture(a2);
		return Tile::stoneBrick->getTexture(a2);
	}
	if(this->baseTile) {
		return this->baseTile->getTexture(a2, a3);
	}
	return Tile::stoneBrick->getTexture(a2);
}
AABB* WallTile::getAABB(Level* level, int32_t x, int32_t y, int32_t z) {
	this->updateShape(level, x, y, z);
	this->maxY = 1.5f;
	AABB* ret = Tile::getAABB(level, x, y, z);
	this->maxY = 1.0f;
	return ret;
}
AABB WallTile::getTileAABB(Level* level, int32_t x, int32_t y, int32_t z) {
	this->updateShape(level, x, y, z);
	return Tile::getTileAABB(level, x, y, z);
}
bool_t WallTile::isSolidRender() {
	return 0;
}
int32_t WallTile::getSpawnResourcesAuxValue(int32_t a2) {
	return a2;
}
