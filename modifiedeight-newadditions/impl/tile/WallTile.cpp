#include <tile/WallTile.hpp>
#include <level/Level.hpp>
#include <tile/material/Material.hpp>
#include <math/HitResult.hpp>

WallTile::WallTile(int32_t id, Tile* a3, int32_t aux)
	: Tile(id, a3 ? a3->material : Material::stone) {
	this->baseTile = a3;
	this->baseAux = aux;
	if (a3) {
		this->setDestroyTime(a3->blockResistance / 3.0);
		if (a3->soundType) this->setSoundType(*a3->soundType);
	}
}
bool_t WallTile::connectsTo(LevelSource* level, int32_t x, int32_t y, int32_t z) {
	if(!level) return 0;
	int32_t v6 = level->getTile(x, y, z);
	if(v6 == 0) return 0;
	if(v6 == this->blockID || (Tile::tiles[v6] && Tile::tiles[v6]->getRenderShape() == 32) || (Tile::fenceGate && v6 == Tile::fenceGate->blockID)) {
		return 1;
	}
	Tile* v7 = Tile::tiles[v6];
	if(!v7) return 0;
	if(v7->material == Material::vegetable || v7->material == Material::leaves || v7->material == Material::plant) {
		return 0;
	}
	if(v7->material->isSolidBlocking() || (Tile::mixedSlab && v6 == Tile::mixedSlab->blockID) || v7->material->isSolid()) {
		return 1;
	}
	return 0;
}

HitResult WallTile::clip(Level* level, int32_t x, int32_t y, int32_t z, const Vec3& a6, const Vec3& a7) {
	this->updateShape(level, x, y, z);
	return Tile::clip(level, x, y, z, a6, a7);
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
		return this->baseTile->getTexture(a2, this->baseAux != 0 ? this->baseAux : a3);
	}
	return Tile::stoneBrick->getTexture(a2);
}
TextureUVCoordinateSet* WallTile::getTexture(LevelSource* level, int32_t x, int32_t y, int32_t z, int32_t a6) {
	int32_t aux = (level ? level->getData(x, y, z) : 0);
	return this->getTexture(a6, this->baseAux != 0 ? this->baseAux : aux);
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
