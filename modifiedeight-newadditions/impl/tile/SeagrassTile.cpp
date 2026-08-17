#include <tile/SeagrassTile.hpp>
#include <level/Level.hpp>
#include <level/LevelSource.hpp>
#include <tile/material/Material.hpp>
#include <item/Item.hpp>
#include <item/ItemInstance.hpp>
#include <util/Random.hpp>

SeagrassTile::SeagrassTile(int32_t id, const std::string& name)
	: Bush(id, name, Material::water) {
	this->textureUV = this->getTextureUVCoordinateSet("seagrass", 0);
	this->textureTallLower = this->getTextureUVCoordinateSet("tall_seagrass_bottom", 0);
	this->textureTallUpper = this->getTextureUVCoordinateSet("tall_seagrass_top", 0);
	this->setShape(0.1f, 0.0f, 0.1f, 0.9f, 0.8f, 0.9f);
}

SeagrassTile::~SeagrassTile() {
}

TextureUVCoordinateSet* SeagrassTile::getTexture(int32_t side, int32_t data) {
	if (data == 1) {
		if (this->textureTallLower.minX < 0.0f) {
			this->textureTallLower = this->getTextureUVCoordinateSet("tall_seagrass_bottom", 0);
		}
		return &this->textureTallLower;
	}
	if (data == 2) {
		if (this->textureTallUpper.minX < 0.0f) {
			this->textureTallUpper = this->getTextureUVCoordinateSet("tall_seagrass_top", 0);
		}
		return &this->textureTallUpper;
	}
	if (this->textureUV.minX < 0.0f) {
		this->textureUV = this->getTextureUVCoordinateSet("seagrass", 0);
	}
	return &this->textureUV;
}

TextureUVCoordinateSet* SeagrassTile::getCarriedTexture(int32_t a2, int32_t a3) {
	return this->getTexture(a2, a3);
}

bool_t SeagrassTile::canSurvive(Level* level, int32_t x, int32_t y, int32_t z) {
	if (!level || y <= 0 || y >= 127) return 0;
	int32_t belowTile = level->getTile(x, y - 1, z);
	if (belowTile == this->blockID) {
		return 1;
	}
	if (belowTile == 0) return 0;
	if (belowTile == Tile::dirt->blockID || belowTile == Tile::grass->blockID ||
	    belowTile == Tile::sand->blockID || belowTile == Tile::gravel->blockID ||
	    belowTile == Tile::clay->blockID || belowTile == Tile::rock->blockID ||
	    Tile::solid[belowTile]) {
		return 1;
	}
	return 0;
}

bool_t SeagrassTile::mayPlace(Level* level, int32_t x, int32_t y, int32_t z) {
	if (!level || y <= 0 || y >= 127) return 0;
	int32_t currentTile = level->getTile(x, y, z);
	Material* curMat = level->getMaterial(x, y, z);
	if (currentTile != Tile::water->blockID && currentTile != Tile::calmWater->blockID && (!curMat || curMat != Material::water)) {
		return 0;
	}
	return this->canSurvive(level, x, y, z);
}

void SeagrassTile::neighborChanged(Level* level, int32_t x, int32_t y, int32_t z, int32_t nx, int32_t ny, int32_t nz, int32_t fromTile) {
	if (!this->canSurvive(level, x, y, z)) {
		this->spawnResources(level, x, y, z, level->getData(x, y, z), 0.0f);
		level->setTileAndData(x, y, z, Tile::calmWater->blockID, 0, 3);
	}
}

void SeagrassTile::onRemove(Level* level, int32_t x, int32_t y, int32_t z) {
	if (level) {
		int32_t cur = level->getTile(x, y, z);
		if (cur == 0) {
			level->setTileAndData(x, y, z, Tile::calmWater->blockID, 0, 3);
		}
	}
}

int32_t SeagrassTile::getResource(int32_t data, Random* rand) {
	return this->blockID;
}

int32_t SeagrassTile::getResourceCount(Random* rand) {
	return 1;
}

int32_t SeagrassTile::getColor(LevelSource* level, int32_t x, int32_t y, int32_t z) {
	return 0xFFFFFF;
}

int32_t SeagrassTile::getColor(int32_t) {
	return 0xFFFFFF;
}

int32_t SeagrassTile::getRenderShape() {
	return 1;
}

int32_t SeagrassTile::getRenderLayer() {
	return 1;
}

bool_t SeagrassTile::isSolidRender() {
	return 0;
}

AABB* SeagrassTile::getAABB(Level* level, int32_t x, int32_t y, int32_t z) {
	return nullptr;
}

bool_t SeagrassTile::mayPick() {
	return 1;
}
