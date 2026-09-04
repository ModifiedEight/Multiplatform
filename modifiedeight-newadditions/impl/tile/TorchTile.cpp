#include <tile/TorchTile.hpp>
#include <level/Level.hpp>
#include <math/HitResult.hpp>
#include <tile/entity/MixedSlabTileEntity.hpp>

static bool isTorchSupportValid(LevelSource* level, int32_t x, int32_t y, int32_t z, int32_t side) {
	if (side == 1) {
		if (level->isSolidBlockingTile(x - 1, y, z)) return true;
		if (Tile::mixedSlab && level->getTile(x - 1, y, z) == Tile::mixedSlab->blockID) {
			MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(x - 1, y, z);
			if (te) return (te->bottomTileId != 0 || te->topTileId != 0);
		}
	} else if (side == 2) {
		if (level->isSolidBlockingTile(x + 1, y, z)) return true;
		if (Tile::mixedSlab && level->getTile(x + 1, y, z) == Tile::mixedSlab->blockID) {
			MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(x + 1, y, z);
			if (te) return (te->bottomTileId != 0 || te->topTileId != 0);
		}
	} else if (side == 3) {
		if (level->isSolidBlockingTile(x, y, z - 1)) return true;
		if (Tile::mixedSlab && level->getTile(x, y, z - 1) == Tile::mixedSlab->blockID) {
			MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(x, y, z - 1);
			if (te) return (te->bottomTileId != 0 || te->topTileId != 0);
		}
	} else if (side == 4) {
		if (level->isSolidBlockingTile(x, y, z + 1)) return true;
		if (Tile::mixedSlab && level->getTile(x, y, z + 1) == Tile::mixedSlab->blockID) {
			MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(x, y, z + 1);
			if (te) return (te->bottomTileId != 0 || te->topTileId != 0);
		}
	}
	return false;
}

bool_t TorchTile::isConnection(Level* level, int32_t x, int32_t y, int32_t z) {
	if(level->isSolidBlockingTile(x, y, z) || level->isTopSolidBlocking(x, y, z)) {
		return 1;
	}
	int32_t v8 = level->getTile(x, y, z);
	int32_t vMeta = level->getData(x, y, z);
	if (v8 == Tile::stoneSlabHalf->blockID || v8 == Tile::woodSlabHalf->blockID ||
	    (Tile::dirtSlabHalf && v8 == Tile::dirtSlabHalf->blockID) ||
	    (Tile::grassSlabHalf && v8 == Tile::grassSlabHalf->blockID) ||
	    (Tile::rockSlabHalf && v8 == Tile::rockSlabHalf->blockID)) {
		return (vMeta & 8) == 0;
	}
	if (Tile::mixedSlab && v8 == Tile::mixedSlab->blockID) {
		MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(x, y, z);
		if (te) {
			if (te->mode == 0) return te->topTileId != 0 || te->bottomTileId != 0;
			return (te->bottomTileId != 0 || te->topTileId != 0);
		}
	}
	return v8 == Tile::fence->blockID || v8 == Tile::glass->blockID || v8 == Tile::cobbleWall->blockID;
}
bool_t TorchTile::checkCanSurvive(Level* level, int32_t x, int32_t y, int32_t z) {
	if(this->mayPlace(level, x, y, z)) {
		return 1;
	}
	if(level->getTile(x, y, z) == this->blockID) {
		int32_t v10 = level->getData(x, y, z);
		this->spawnResources(level, x, y, z, v10);
		level->setTile(x, y, z, 0, 3);
	}
	return 0;
}

TorchTile::TorchTile(int32_t id, const std::string& s, Material* m)
	: Tile(id, s, m) {
	this->setTicking(1);
}
TorchTile::~TorchTile() {
}
bool_t TorchTile::isCubeShaped() {
	return 0;
}
int32_t TorchTile::getRenderShape() {
	return 2;
}
AABB* TorchTile::getAABB(Level*, int32_t, int32_t, int32_t) {
	return 0;
}
bool_t TorchTile::isSolidRender() {
	return 0;
}
bool_t TorchTile::mayPlace(Level* level, int32_t x, int32_t y, int32_t z) {
	return isTorchSupportValid(level, x, y, z, 1) ||
	       isTorchSupportValid(level, x, y, z, 2) ||
	       isTorchSupportValid(level, x, y, z, 3) ||
	       isTorchSupportValid(level, x, y, z, 4) ||
	       this->isConnection(level, x, y - 1, z);
}
void TorchTile::tick(Level* level, int32_t x, int32_t y, int32_t z, Random*) {
	if(!level->getData(x, y, z)) {
		this->onPlace(level, x, y, z);
	}
}
void TorchTile::animateTick(Level* level, int32_t x, int32_t y, int32_t z, Random* r) {
	int32_t v9 = level->getData(x, y, z);
	float v10 = (float)y + 0.7f;
	float v11 = (float)x + 0.5f;
	float v12 = (float)z + 0.5f;
	float offset = 0.0f;
	if (Tile::mixedSlab) {
		if (v9 == 1) {
			int32_t tid = level->getTile(x - 1, y, z);
			if (tid == Tile::mixedSlab->blockID) {
				MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(x - 1, y, z);
				if (te && te->mode == 2 && te->bottomTileId != 0 && te->topTileId == 0) offset = -0.5f;
			}
		} else if (v9 == 2) {
			int32_t tid = level->getTile(x + 1, y, z);
			if (tid == Tile::mixedSlab->blockID) {
				MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(x + 1, y, z);
				if (te && te->mode == 2 && te->bottomTileId == 0 && te->topTileId != 0) offset = 0.5f;
			}
		} else if (v9 == 3) {
			int32_t tid = level->getTile(x, y, z - 1);
			if (tid == Tile::mixedSlab->blockID) {
				MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(x, y, z - 1);
				if (te && te->mode == 1 && te->bottomTileId != 0 && te->topTileId == 0) offset = -0.5f;
			}
		} else if (v9 == 4) {
			int32_t tid = level->getTile(x, y, z + 1);
			if (tid == Tile::mixedSlab->blockID) {
				MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(x, y, z + 1);
				if (te && te->mode == 1 && te->bottomTileId == 0 && te->topTileId != 0) offset = 0.5f;
			}
		}
	}
	switch(v9) {
		case 1:
			v11 = v11 - 0.27f + offset;
			v10 = v10 + 0.22f;
			break;
		case 2:
			v11 = v11 + 0.27f + offset;
			v10 = v10 + 0.22f;
			break;
		case 3:
			v12 = v12 - 0.27f + offset;
			v10 = v10 + 0.22f;
			break;
		case 4:
			v12 = v12 + 0.27f + offset;
			v10 = v10 + 0.22f;
			break;
	}
	level->addParticle(PT_SMOKE, v11, v10, v12, 0.0, 0.0, 0.0, 0);
	level->addParticle(PT_FLAME, v11, v10, v12, 0.0, 0.0, 0.0, 0);
}
void TorchTile::neighborChanged(Level* level, int32_t x, int32_t y, int32_t z, int32_t, int32_t, int32_t, int32_t) {
	if(this->checkCanSurvive(level, x, y, z)) {
		int32_t data = level->getData(x, y, z);
		bool_t drop = false;
		if (data == 1 && !isTorchSupportValid(level, x, y, z, 1)) drop = true;
		if (data == 2 && !isTorchSupportValid(level, x, y, z, 2)) drop = true;
		if (data == 3 && !isTorchSupportValid(level, x, y, z, 3)) drop = true;
		if (data == 4 && !isTorchSupportValid(level, x, y, z, 4)) drop = true;
		if (data == 5 && !this->isConnection(level, x, y - 1, z)) drop = true;
		if (drop) {
			int32_t v16 = level->getData(x, y, z);
			this->spawnResources(level, x, y, z, v16);
			level->setTile(x, y, z, 0, 3);
		}
	}
}
void TorchTile::onPlace(Level* level, int32_t x, int32_t y, int32_t z) {
	if(!level->getData(x, y, z)) {
		if(isTorchSupportValid(level, x, y, z, 1)) {
			level->setData(x, y, z, 1, 2);
		} else if(isTorchSupportValid(level, x, y, z, 2)) {
			level->setData(x, y, z, 2, 2);
		} else if(isTorchSupportValid(level, x, y, z, 3)) {
			level->setData(x, y, z, 3, 2);
		} else if(isTorchSupportValid(level, x, y, z, 4)) {
			level->setData(x, y, z, 4, 2);
		} else if(this->isConnection(level, x, y - 1, z)) {
			level->setData(x, y, z, 5, 2);
		}
	}
	this->checkCanSurvive(level, x, y, z);
}
HitResult TorchTile::clip(Level* level, int32_t x, int32_t y, int32_t z, const Vec3& a7, const Vec3& a8) {
	int32_t v9 = level->getData(x, y, z);
	float offset = 0.0f;
	if (Tile::mixedSlab) {
		if (v9 == 1) {
			int32_t tid = level->getTile(x - 1, y, z);
			if (tid == Tile::mixedSlab->blockID) {
				MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(x - 1, y, z);
				if (te && te->mode == 2 && te->bottomTileId != 0 && te->topTileId == 0) offset = -0.5f;
			}
		} else if (v9 == 2) {
			int32_t tid = level->getTile(x + 1, y, z);
			if (tid == Tile::mixedSlab->blockID) {
				MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(x + 1, y, z);
				if (te && te->mode == 2 && te->bottomTileId == 0 && te->topTileId != 0) offset = 0.5f;
			}
		} else if (v9 == 3) {
			int32_t tid = level->getTile(x, y, z - 1);
			if (tid == Tile::mixedSlab->blockID) {
				MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(x, y, z - 1);
				if (te && te->mode == 1 && te->bottomTileId != 0 && te->topTileId == 0) offset = -0.5f;
			}
		} else if (v9 == 4) {
			int32_t tid = level->getTile(x, y, z + 1);
			if (tid == Tile::mixedSlab->blockID) {
				MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(x, y, z + 1);
				if (te && te->mode == 1 && te->bottomTileId == 0 && te->topTileId != 0) offset = 0.5f;
			}
		}
	}
	switch(v9) {
		case 1:
			this->setShape(0.0 + offset, 0.2, 0.35, 0.3 + offset, 0.8, 0.65);
			break;
		case 2:
			this->setShape(0.7 + offset, 0.2, 0.35, 1.0 + offset, 0.8, 0.65);
			break;
		case 3:
			this->setShape(0.35, 0.2, 0.0 + offset, 0.65, 0.8, 0.3 + offset);
			break;
		case 4:
			this->setShape(0.35, 0.2, 0.7 + offset, 0.65, 0.8, 1.0 + offset);
			break;
		default:
			this->setShape(0.4, 0.0, 0.4, 0.6, 0.6, 0.6);
			break;
	}
	return Tile::clip(level, x, y, z, a7, a8);
}
int32_t TorchTile::getRenderLayer() {
	return 0;
}
int32_t TorchTile::getPlacementDataValue(Level* level, int32_t x, int32_t y, int32_t z, int32_t a6, float, float, float, Mob*, int32_t a11) {
	if(a6 == 1 && this->isConnection(level, x, y - 1, z)) return 5;
	if(a6 == 2 && isTorchSupportValid(level, x, y, z, 4)) return 4;
	if(a6 == 3 && isTorchSupportValid(level, x, y, z, 3)) return 3;
	if(a6 == 4 && isTorchSupportValid(level, x, y, z, 2)) return 2;
	if(a6 == 5 && isTorchSupportValid(level, x, y, z, 1)) return 1;
	return a11;
}
