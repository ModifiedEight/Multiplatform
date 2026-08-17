#include <tile/MixedSlabTile.hpp>
#include <tile/entity/MixedSlabTileEntity.hpp>
#include <level/Level.hpp>
#include <entity/Player.hpp>
#include <tile/material/Material.hpp>
#include <math/HitResult.hpp>

MixedSlabTile::MixedSlabTile(int32_t id)
	: EntityTile(id, "stoneSlab", Material::stone) {
	this->setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	this->setDestroyTime(2.0f);
	this->setExplodeable(10.0f);
	this->setSoundType(Tile::SOUND_STONE);
}

MixedSlabTile::~MixedSlabTile() {
}

TileEntity* MixedSlabTile::newTileEntity() {
	return new MixedSlabTileEntity();
}

int32_t MixedSlabTile::getTileEntityType() {
	return 5;
}

int32_t MixedSlabTile::getRenderShape() {
	return 40;
}

void MixedSlabTile::updateShape(LevelSource* level, int32_t x, int32_t y, int32_t z) {
	MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(x, y, z);
	if (!te) {
		this->setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
		return;
	}
	bool hasB = (te->bottomTileId > 0);
	bool hasT = (te->topTileId > 0);
	if (hasB && hasT) {
		this->setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
		return;
	}
	if (te->mode == 1) {
		if (hasB) {
			this->setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.5f);
		} else if (hasT) {
			this->setShape(0.0f, 0.0f, 0.5f, 1.0f, 1.0f, 1.0f);
		}
	} else if (te->mode == 2) {
		if (hasB) {
			this->setShape(0.0f, 0.0f, 0.0f, 0.5f, 1.0f, 1.0f);
		} else if (hasT) {
			this->setShape(0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
		}
	} else {
		if (hasB) {
			this->setShape(0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 1.0f);
		} else if (hasT) {
			this->setShape(0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f);
		}
	}
}

void MixedSlabTile::addAABBs(Level* level, int32_t x, int32_t y, int32_t z, const AABB* a6, std::vector<AABB>& a7) {
	this->updateShape(level, x, y, z);
	Tile::addAABBs(level, x, y, z, a6, a7);
}

bool_t MixedSlabTile::isSolidRender() {
	return 0;
}

bool_t MixedSlabTile::isCubeShaped() {
	return 0;
}

int32_t MixedSlabTile::getResource(int32_t meta, Random* random) {
	return 0;
}

int32_t MixedSlabTile::getResourceCount(Random* random) {
	return 0;
}

void MixedSlabTile::playerDestroy(Level* level, Player* player, int32_t x, int32_t y, int32_t z, int32_t meta) {
	MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(x, y, z);
	if (!te) {
		return;
	}

	int32_t mode = te->mode;
	int32_t bTileId = te->bottomTileId;
	int32_t bAux = te->bottomAux;
	int32_t tTileId = te->topTileId;
	int32_t tAux = te->topAux;

	float hitCoord = 0.5f;
	if (player) {
		Vec3 eyePos = player->getPos(1.0f);
		eyePos.y += player->getHeadHeight();
		Vec3 viewVec = player->getViewVector(1.0f);
		Vec3 reachEnd(eyePos.x + viewVec.x * 7.0f, eyePos.y + viewVec.y * 7.0f, eyePos.z + viewVec.z * 7.0f);
		AABB fullBox = {(float)x, (float)y, (float)z, (float)x + 1.0f, (float)y + 1.0f, (float)z + 1.0f};
		HitResult hit = fullBox.clip(eyePos, reachEnd);
		if (hit.hitType == 0) {
			if (mode == 1) hitCoord = hit.hitVec.z - (float)z;
			else if (mode == 2) hitCoord = hit.hitVec.x - (float)x;
			else hitCoord = hit.hitVec.y - (float)y;
		}
	}

	bool hitTop = (hitCoord >= 0.5f);
	if (bTileId > 0 && tTileId == 0) hitTop = false;
	if (tTileId > 0 && bTileId == 0) hitTop = true;

	if (hitTop) {
		Tile* topTile = (tTileId > 0 && tTileId < 256) ? Tile::tiles[tTileId] : nullptr;
		int32_t dropId = topTile ? topTile->getResource(tAux, &level->random) : tTileId;
		int32_t dropAux = topTile ? topTile->getSpawnResourcesAuxValue(tAux) : tAux;
		if (dropId > 0) {
			this->popResource(level, x, y, z, ItemInstance(dropId, 1, dropAux));
		}
		if (bTileId > 0) {
			te->topTileId = 0;
			te->topAux = 0;
			level->sendTileUpdated(x, y, z);
		} else {
			level->removeTileEntity(x, y, z);
			level->setTile(x, y, z, 0, 3);
		}
	} else {
		Tile* bottomTile = (bTileId > 0 && bTileId < 256) ? Tile::tiles[bTileId] : nullptr;
		int32_t dropId = bottomTile ? bottomTile->getResource(bAux, &level->random) : bTileId;
		int32_t dropAux = bottomTile ? bottomTile->getSpawnResourcesAuxValue(bAux) : bAux;
		if (dropId > 0) {
			this->popResource(level, x, y, z, ItemInstance(dropId, 1, dropAux));
		}
		if (tTileId > 0) {
			te->bottomTileId = 0;
			te->bottomAux = 0;
			level->sendTileUpdated(x, y, z);
		} else {
			level->removeTileEntity(x, y, z);
			level->setTile(x, y, z, 0, 3);
		}
	}
}

void MixedSlabTile::playerWillDestroy(Level* level, int32_t x, int32_t y, int32_t z, int32_t meta, Player* player) {
	MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(x, y, z);
	if (!te) return;
	if (player && player->abilities.instabuild) return;

	int32_t remTileId = (te->bottomTileId > 0) ? te->bottomTileId : te->topTileId;
	int32_t remAux = (te->bottomTileId > 0) ? te->bottomAux : te->topAux;
	if (remTileId > 0) {
		Tile* remTile = (remTileId < 256) ? Tile::tiles[remTileId] : nullptr;
		int32_t dropId = remTile ? remTile->getResource(remAux, &level->random) : remTileId;
		int32_t dropAux = remTile ? remTile->getSpawnResourcesAuxValue(remAux) : remAux;
		if (dropId > 0) {
			this->popResource(level, x, y, z, ItemInstance(dropId, 1, dropAux));
		}
	}
}
