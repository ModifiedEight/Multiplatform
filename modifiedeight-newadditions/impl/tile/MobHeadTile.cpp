#include <tile/MobHeadTile.hpp>
#include <tile/entity/MobHeadTileEntity.hpp>
#include <tile/material/Material.hpp>
#include <entity/Mob.hpp>
#include <level/Level.hpp>
#include <math/Mth.hpp>

static const int32_t _headBlockIds[18] = {
	115, 116, 117, 118, 119, 120, 121, 122, 125, 127,
	129, 130, 131, 132, 137, 138, 188, 189
};

bool MobHeadTile::isHeadBlock(int32_t id) {
	for (int i = 0; i < 18; ++i) {
		if (_headBlockIds[i] == id) return true;
	}
	return false;
}

int32_t MobHeadTile::getHeadType(int32_t id) {
	for (int i = 0; i < 18; ++i) {
		if (_headBlockIds[i] == id) return i;
	}
	return -1;
}

MobHeadTile::MobHeadTile(int32_t id, int32_t headType, const std::string& name)
	: EntityTile(id, Material::decoration)
	, headType(headType) {
	this->setShape(0.25f, 0.0f, 0.25f, 0.75f, 0.5f, 0.75f);
	this->setDestroyTime(1.0f);
	this->setExplodeable(1.0f);
	this->setSoundType(Tile::SOUND_STONE);
	this->setDescriptionId(name);
	this->setCategory(2, 8);
}

MobHeadTile::~MobHeadTile() {
}

bool_t MobHeadTile::isCubeShaped() {
	return 0;
}

bool_t MobHeadTile::isSolidRender() {
	return 0;
}

int32_t MobHeadTile::getRenderShape() {
	return -1;
}

int32_t MobHeadTile::getTileEntityType() {
	return 7;
}

int32_t MobHeadTile::getPlacementDataValue(Level* level, int32_t x, int32_t y, int32_t z, int32_t side, float faceX, float faceY, float faceZ, Mob* player, int32_t meta) {
	if (side == 0) side = 1;
	return side;
}

int32_t MobHeadTile::getResource(int32_t, Random*) {
	return this->blockID;
}

int32_t MobHeadTile::getResourceCount(Random*) {
	return 1;
}

AABB* MobHeadTile::getAABB(Level* level, int32_t x, int32_t y, int32_t z) {
	static AABB headBB;
	int meta = level ? level->getData(x, y, z) : 1;
	if (meta == 2) {
		headBB.minX = (float)x + 0.25f; headBB.minY = (float)y + 0.25f; headBB.minZ = (float)z + 0.5f;
		headBB.maxX = (float)x + 0.75f; headBB.maxY = (float)y + 0.75f; headBB.maxZ = (float)z + 1.0f;
	} else if (meta == 3) {
		headBB.minX = (float)x + 0.25f; headBB.minY = (float)y + 0.25f; headBB.minZ = (float)z;
		headBB.maxX = (float)x + 0.75f; headBB.maxY = (float)y + 0.75f; headBB.maxZ = (float)z + 0.5f;
	} else if (meta == 4) {
		headBB.minX = (float)x + 0.5f; headBB.minY = (float)y + 0.25f; headBB.minZ = (float)z + 0.25f;
		headBB.maxX = (float)x + 1.0f; headBB.maxY = (float)y + 0.75f; headBB.maxZ = (float)z + 0.75f;
	} else if (meta == 5) {
		headBB.minX = (float)x; headBB.minY = (float)y + 0.25f; headBB.minZ = (float)z + 0.25f;
		headBB.maxX = (float)x + 0.5f; headBB.maxY = (float)y + 0.75f; headBB.maxZ = (float)z + 0.75f;
	} else {
		headBB.minX = (float)x + 0.25f; headBB.minY = (float)y; headBB.minZ = (float)z + 0.25f;
		headBB.maxX = (float)x + 0.75f; headBB.maxY = (float)y + 0.5f; headBB.maxZ = (float)z + 0.75f;
	}
	return &headBB;
}

void MobHeadTile::updateShape(LevelSource* ls, int32_t x, int32_t y, int32_t z) {
	int meta = ls ? ls->getData(x, y, z) : 1;
	if (meta == 2) {
		this->setShape(0.25f, 0.25f, 0.5f, 0.75f, 0.75f, 1.0f);
	} else if (meta == 3) {
		this->setShape(0.25f, 0.25f, 0.0f, 0.75f, 0.75f, 0.5f);
	} else if (meta == 4) {
		this->setShape(0.5f, 0.25f, 0.25f, 1.0f, 0.75f, 0.75f);
	} else if (meta == 5) {
		this->setShape(0.0f, 0.25f, 0.25f, 0.5f, 0.75f, 0.75f);
	} else {
		this->setShape(0.25f, 0.0f, 0.25f, 0.75f, 0.5f, 0.75f);
	}
}
