#include <tile/SlabTile.hpp>
#include <level/Level.hpp>
#include <math/Facing.hpp>

SlabTile::SlabTile(int32_t a2, const std::string& a3, bool_t full, const struct Material* mat)
	: Tile(a2, "planks", mat) {
	this->isFull = full;
	SlabTile::updateDefaultShape();
	this->setLightBlock(255);
}
bool_t SlabTile::isBottomSlab(LevelSource* level, int32_t x, int32_t y, int32_t z) {
	return ((level->getData(x, y, z) ^ 8u) >> 3) & 1;
}

SlabTile::~SlabTile() {
}
int32_t SlabTile::getTileType() {
	return 2;
}
bool_t SlabTile::isCubeShaped() {
	return this->isFull;
}
void SlabTile::updateShape(LevelSource* level, int32_t x, int32_t y, int32_t z) {
	if(this->isFull) {
		this->setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	} else {
		if(this->isBottomSlab(level, x, y, z)) {
			this->setShape(0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 1.0f);
		} else {
			this->setShape(0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f);
		}
	}
}
void SlabTile::updateDefaultShape() {
	if(this->isFull) {
		this->setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	} else {
		this->setShape(0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 1.0f);
	}
}
bool_t SlabTile::shouldRenderFace(LevelSource* level, int32_t x, int32_t y, int32_t z, int32_t side) {
	if(this->isFull) {
		return Tile::shouldRenderFace(level, x, y, z, side);
	}
	if(side <= 1 || Tile::shouldRenderFace(level, x, y, z, side)) {
		if(this->isBottomSlab(level, Facing::STEP_X[Facing::OPPOSITE_FACING[side]] + x, Facing::STEP_Y[Facing::OPPOSITE_FACING[side]] + y, z + Facing::STEP_Z[Facing::OPPOSITE_FACING[side]])) {
			if(side != 1 && (side || !Tile::shouldRenderFace(level, x, y, z, 0))) {
				int32_t v13 = level->getTile(x, y, z);
				if(v13 == Tile::woodSlabHalf->blockID || v13 == Tile::stoneSlabHalf->blockID) {
					return (level->getData(x, y, z) >> 3) & 1;
				}
			}
		} else if(side) {
			if(side != 1 || !Tile::shouldRenderFace(level, x, y, z, 1)) {
				int32_t v11 = level->getTile(x, y, z);
				if(v11 == Tile::woodSlabHalf->blockID || v11 == Tile::stoneSlabHalf->blockID) {
					return ((level->getData(x, y, z) ^ 8) >> 3) & 1;
				}
			}
		}
		return 1;
	}
	return 0;
}
void SlabTile::addAABBs(Level* level, int32_t x, int32_t y, int32_t z, const AABB* a6, std::vector<AABB>& a7) {
	this->updateShape(level, x, y, z);
	Tile::addAABBs(level, x, y, z, a6, a7);
}
bool_t SlabTile::isSolidRender() {
	return this->isFull;
}
int32_t SlabTile::getResourceCount(Random*) {
	if(this->isFull) return 2;
	return 1;
}
int32_t SlabTile::getPlacementDataValue(Level* a2, int32_t a3, int32_t a4, int32_t a5, int32_t side, float a7, float a8, float a9, Mob* a10, int32_t a11) {
	int32_t v11 = a11;
	if(!this->isFull && (!side || (side != 1 && a8 > 0.5f))) {
		return a11 | 8;
	}
	return v11;
}
