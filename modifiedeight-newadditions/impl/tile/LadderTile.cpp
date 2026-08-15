#include <tile/LadderTile.hpp>
#include <level/Level.hpp>
#include <item/ItemInstance.hpp>
#include <tile/entity/MixedSlabTileEntity.hpp>

static bool isLadderSupported(LevelSource* level, int32_t x, int32_t y, int32_t z, int32_t face) {
	int32_t nx = x, ny = y, nz = z;
	if (face == 2) ++nz;
	else if (face == 3) --nz;
	else if (face == 4) ++nx;
	else if (face == 5) --nx;
	else return false;

	if (level->isSolidBlockingTile(nx, ny, nz)) return true;

	if (Tile::mixedSlab) {
		int32_t tid = level->getTile(nx, ny, nz);
		if (tid == Tile::mixedSlab->blockID) {
			MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(nx, ny, nz);
			if (te) {
				if (face == 2) {
					if (te->mode == 1 && (te->bottomTileId != 0 || te->topTileId != 0)) return true;
					if (te->mode == 0 || te->mode == 2) return (te->bottomTileId != 0 || te->topTileId != 0);
				} else if (face == 3) {
					if (te->mode == 1 && (te->bottomTileId != 0 || te->topTileId != 0)) return true;
					if (te->mode == 0 || te->mode == 2) return (te->bottomTileId != 0 || te->topTileId != 0);
				} else if (face == 4) {
					if (te->mode == 2 && (te->bottomTileId != 0 || te->topTileId != 0)) return true;
					if (te->mode == 0 || te->mode == 1) return (te->bottomTileId != 0 || te->topTileId != 0);
				} else if (face == 5) {
					if (te->mode == 2 && (te->bottomTileId != 0 || te->topTileId != 0)) return true;
					if (te->mode == 0 || te->mode == 1) return (te->bottomTileId != 0 || te->topTileId != 0);
				}
			}
		}
	}
	return false;
}

LadderTile::LadderTile(int32_t id, const std::string& a3, Material* a4) : Tile(id, a3, a4){

}
LadderTile::~LadderTile() {
}
bool_t LadderTile::isCubeShaped() {
	return 0;
}
int32_t LadderTile::getRenderShape() {
	return 8;
}
AABB* LadderTile::getAABB(Level* level, int32_t x, int32_t y, int32_t z) {
	int32_t v9 = level->getData(x, y, z);
	float offset = 0.0f;
	if (Tile::mixedSlab) {
		if (v9 == 2) {
			int32_t tid = level->getTile(x, y, z + 1);
			if (tid == Tile::mixedSlab->blockID) {
				MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(x, y, z + 1);
				if (te && te->mode == 1 && te->bottomTileId == 0 && te->topTileId != 0) offset = 0.5f;
			}
		} else if (v9 == 3) {
			int32_t tid = level->getTile(x, y, z - 1);
			if (tid == Tile::mixedSlab->blockID) {
				MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(x, y, z - 1);
				if (te && te->mode == 1 && te->bottomTileId != 0 && te->topTileId == 0) offset = -0.5f;
			}
		} else if (v9 == 4) {
			int32_t tid = level->getTile(x + 1, y, z);
			if (tid == Tile::mixedSlab->blockID) {
				MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(x + 1, y, z);
				if (te && te->mode == 2 && te->bottomTileId == 0 && te->topTileId != 0) offset = 0.5f;
			}
		} else if (v9 == 5) {
			int32_t tid = level->getTile(x - 1, y, z);
			if (tid == Tile::mixedSlab->blockID) {
				MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(x - 1, y, z);
				if (te && te->mode == 2 && te->bottomTileId != 0 && te->topTileId == 0) offset = -0.5f;
			}
		}
	}
	switch(v9) {
		case 2:
			this->setShape(0.0, 0.0, 0.875 + offset, 1.0, 1.0, 1.0 + offset);
			break;
		case 3:
			this->setShape(0.0, 0.0, 0.0 + offset, 1.0, 1.0, 0.125 + offset);
			break;
		case 4:
			this->setShape(0.875 + offset, 0.0, 0.0, 1.0 + offset, 1.0, 1.0);
			break;
		case 5:
			this->setShape(0.0 + offset, 0.0, 0.0, 0.125 + offset, 1.0, 1.0);
			break;
		default:
			break;
	}
	return Tile::getAABB(level, x, y, z);
}
AABB LadderTile::getTileAABB(Level* level, int32_t x, int32_t y, int32_t z) {
	this->getAABB(level, x, y, z);
	return Tile::getTileAABB(level, x, y, z);
}
bool_t LadderTile::isSolidRender() {
	return 0;
}
bool_t LadderTile::mayPlace(Level* level, int32_t x, int32_t y, int32_t z, uint8_t) {
	return isLadderSupported(level, x, y, z, 2) ||
	       isLadderSupported(level, x, y, z, 3) ||
	       isLadderSupported(level, x, y, z, 4) ||
	       isLadderSupported(level, x, y, z, 5);
}
void LadderTile::neighborChanged(Level* level, int32_t x, int32_t y, int32_t z, int32_t a6, int32_t a7, int32_t a8, int32_t a9) {
	int32_t v13 = level->getData(x, y, z);
	if (!isLadderSupported(level, x, y, z, v13)) {
		this->popResource(level, x, y, z, ItemInstance(Tile::ladder));
		level->setTile(x, y, z, 0, 3);
	}
	Tile::neighborChanged(level, x, y, z, a6, a7, a8, a9);
}
int32_t LadderTile::getResourceCount(Random*) {
	return 1;
}
int32_t LadderTile::getRenderLayer() {
	return 1;
}
int32_t LadderTile::getPlacementDataValue(Level* level, int32_t x, int32_t y, int32_t z, int32_t side, float, float, float, Mob*, int32_t a11) {
	int32_t v12 = a11;
	if (!a11 || side == 2) {
		if (isLadderSupported(level, x, y, z, 2)) return 2;
	}
	if (!a11 || side == 3) {
		if (isLadderSupported(level, x, y, z, 3)) return 3;
	}
	if (!a11 || side == 4) {
		if (isLadderSupported(level, x, y, z, 4)) return 4;
	}
	if (!a11 || side == 5) {
		if (isLadderSupported(level, x, y, z, 5)) return 5;
	}
	for (int f = 2; f <= 5; ++f) {
		if (isLadderSupported(level, x, y, z, f)) return f;
	}
	return v12;
}
