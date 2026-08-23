#include <tile/PressurePlateTile.hpp>
#include <level/Level.hpp>
#include <entity/Entity.hpp>
#include <item/ItemInstance.hpp>
#include <tile/material/Material.hpp>
#include <vector>

PressurePlateTile::PressurePlateTile(int32_t id, const std::string& textureName, Material* material)
	: Tile(id, textureName, material) {
	this->updateDefaultShape();
}

PressurePlateTile::PressurePlateTile(int32_t id, TextureUVCoordinateSet uv, Material* material)
	: Tile(id, uv, material) {
	this->updateDefaultShape();
}

PressurePlateTile::~PressurePlateTile() {
}

bool_t PressurePlateTile::isCubeShaped() {
	return 0;
}

bool_t PressurePlateTile::isSolidRender() {
	return 0;
}

int32_t PressurePlateTile::getRenderShape() {
	return 0;
}

AABB* PressurePlateTile::getAABB(Level* level, int32_t x, int32_t y, int32_t z) {
	return nullptr;
}

void PressurePlateTile::updateDefaultShape() {
	this->setShape(0.0625f, 0.0f, 0.0625f, 0.9375f, 0.0625f, 0.9375f);
}

static bool isBottomHalfSlab(LevelSource* level, int32_t x, int32_t y, int32_t z) {
	int32_t id = level->getTile(x, y, z);
	int32_t meta = level->getData(x, y, z);
	if (id == Tile::stoneSlabHalf->blockID || id == Tile::woodSlabHalf->blockID ||
	    (Tile::coloredSlabHalf1 && id == Tile::coloredSlabHalf1->blockID) ||
	    (Tile::coloredSlabHalf2 && id == Tile::coloredSlabHalf2->blockID)) {
		return (meta & 8) == 0;
	}
	return false;
}

void PressurePlateTile::updateShape(LevelSource* level, int32_t x, int32_t y, int32_t z) {
	int32_t meta = level->getData(x, y, z);
	float offset = 0.0f;
	if (isBottomHalfSlab(level, x, y - 1, z)) {
		offset = -0.5f;
	}
	if (meta > 0) {
		this->setShape(0.0625f, offset, 0.0625f, 0.9375f, offset + 0.03125f, 0.9375f);
	} else {
		this->setShape(0.0625f, offset, 0.0625f, 0.9375f, offset + 0.0625f, 0.9375f);
	}
}

bool_t PressurePlateTile::mayPlace(Level* level, int32_t x, int32_t y, int32_t z) {
	return level->getTile(x, y - 1, z) != 0;
}

void PressurePlateTile::neighborChanged(Level* level, int32_t x, int32_t y, int32_t z, int32_t, int32_t, int32_t, int32_t) {
	if (!this->mayPlace(level, x, y, z)) {
		this->popResource(level, x, y, z, ItemInstance(this->blockID, 1, 0));
		level->setTile(x, y, z, 0, 3);
	}
}

void PressurePlateTile::onPlace(Level* level, int32_t x, int32_t y, int32_t z) {
	level->setData(x, y, z, 0, 2);
}

void PressurePlateTile::onRemove(Level* level, int32_t x, int32_t y, int32_t z) {
	if (level->getData(x, y, z) > 0) {
		level->updateNeighborsAt(x, y, z, this->blockID);
		level->updateNeighborsAt(x, y - 1, z, this->blockID);
	}
	Tile::onRemove(level, x, y, z);
}

void PressurePlateTile::entityInside(Level* level, int32_t x, int32_t y, int32_t z, Entity* entity) {
	if (!level->isClientMaybe) {
		this->checkPressed(level, x, y, z);
	}
}

void PressurePlateTile::checkPressed(Level* level, int32_t x, int32_t y, int32_t z) {
	int32_t meta = level->getData(x, y, z);
	AABB box = {(float)x - 0.15f, (float)y - 0.2f, (float)z - 0.15f, (float)x + 1.15f, (float)y + 0.8f, (float)z + 1.15f};
	std::vector<Entity*>* ents = level->getEntities(nullptr, box);
	bool hasEntities = false;
	if (ents) {
		for (auto ent : *ents) {
			if (ent && !ent->isDead) {
				hasEntities = true;
				break;
			}
		}
	}

	if (hasEntities) {
		if (meta == 0) {
			level->setData(x, y, z, 1, 3);
			level->playSound((float)x + 0.5f, (float)y + 0.1f, (float)z + 0.5f, "random.click", 0.3f, 0.6f);
			level->updateNeighborsAt(x, y, z, this->blockID);
			level->updateNeighborsAt(x, y - 1, z, this->blockID);
		}
		level->addToTickNextTick(x, y, z, this->blockID, 20);
	}
}

void PressurePlateTile::tick(Level* level, int32_t x, int32_t y, int32_t z, Random*) {
	if (level->isClientMaybe) return;
	int32_t meta = level->getData(x, y, z);
	if (meta == 0) return;

	AABB box = {(float)x - 0.15f, (float)y - 0.2f, (float)z - 0.15f, (float)x + 1.15f, (float)y + 0.8f, (float)z + 1.15f};
	std::vector<Entity*>* ents = level->getEntities(nullptr, box);
	bool hasEntities = false;
	if (ents) {
		for (auto ent : *ents) {
			if (ent && !ent->isDead) {
				hasEntities = true;
				break;
			}
		}
	}

	if (!hasEntities) {
		level->setData(x, y, z, 0, 3);
		level->playSound((float)x + 0.5f, (float)y + 0.1f, (float)z + 0.5f, "random.click", 0.3f, 0.5f);
		level->updateNeighborsAt(x, y, z, this->blockID);
		level->updateNeighborsAt(x, y - 1, z, this->blockID);
	} else {
		level->addToTickNextTick(x, y, z, this->blockID, 20);
	}
}

bool_t PressurePlateTile::isSignalSource() {
	return 1;
}

int32_t PressurePlateTile::getSignal(LevelSource* level, int32_t x, int32_t y, int32_t z, int32_t side) {
	return (level->getData(x, y, z) > 0) ? 15 : 0;
}

int32_t PressurePlateTile::getDirectSignal(Level* level, int32_t x, int32_t y, int32_t z, int32_t side) {
	return (side == 1 && level->getData(x, y, z) > 0) ? 15 : 0;
}
