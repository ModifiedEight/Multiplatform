#include <tile/VineTile.hpp>
#include <NinecraftApp.hpp>
#include <level/Level.hpp>
#include <level/biome/Biome.hpp>
#include <tile/material/Material.hpp>
#include <tile/BlockColorRegistry.hpp>
#include <item/Item.hpp>
#include <item/ItemInstance.hpp>
#include <entity/Player.hpp>
#include <util/Random.hpp>

VineTile::VineTile(int32_t id, const std::string& name)
	: Tile(id, name, Material::replaceable_plant) {
	this->setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	this->replaceable = 1;
	this->setTicking(true);
}

VineTile::~VineTile() {
}

bool_t VineTile::isCubeShaped() {
	return 0;
}

bool_t VineTile::isSolidRender() {
	return 0;
}

int32_t VineTile::getRenderShape() {
	return 20;
}

int32_t VineTile::getRenderLayer() {
	return 1;
}

AABB* VineTile::getAABB(Level*, int32_t, int32_t, int32_t) {
	return nullptr;
}

int32_t VineTile::getColor(int32_t) {
	return 0x30BB0B;
}

int32_t VineTile::getColor(LevelSource* level, int32_t x, int32_t y, int32_t z) {
	if (level && BlockColorRegistry::hasBlockColor(x, y, z)) {
		return BlockColorRegistry::getBlockColor(x, y, z) & 0xFFFFFF;
	}
	if (level) {
		Biome* b = level->getBiome(x, z);
		if (b == Biome::swampland) {
			return 0x6A7039;
		}
	}
	return 0x30BB0B;
}

bool_t VineTile::canAttachTo(LevelSource* level, int32_t x, int32_t y, int32_t z) {
	int32_t id = level->getTile(x, y, z);
	if (id == 0) return 0;
	Tile* t = Tile::tiles[id];
	if (!t) return 0;
	return t->isSolidRender() || id == Tile::leaves->blockID || id == Tile::treeTrunk->blockID;
}

bool_t VineTile::mayPlace(Level* level, int32_t x, int32_t y, int32_t z, uint8_t side) {
	if (side == 2) return canAttachTo(level, x, y, z + 1);
	if (side == 3) return canAttachTo(level, x, y, z - 1);
	if (side == 4) return canAttachTo(level, x + 1, y, z);
	if (side == 5) return canAttachTo(level, x - 1, y, z);
	return canAttachTo(level, x, y + 1, z);
}

int32_t VineTile::getPlacementDataValue(Level* level, int32_t x, int32_t y, int32_t z, int32_t side, float, float, float, Mob*, int32_t) {
	if (side == 2 && canAttachTo(level, x, y, z + 1)) return 4;
	if (side == 3 && canAttachTo(level, x, y, z - 1)) return 1;
	if (side == 4 && canAttachTo(level, x + 1, y, z)) return 2;
	if (side == 5 && canAttachTo(level, x - 1, y, z)) return 8;
	if (canAttachTo(level, x, y, z + 1)) return 4;
	if (canAttachTo(level, x, y, z - 1)) return 1;
	if (canAttachTo(level, x + 1, y, z)) return 2;
	if (canAttachTo(level, x - 1, y, z)) return 8;
	return 0;
}

bool_t VineTile::canSurvive(Level* level, int32_t x, int32_t y, int32_t z) {
	int32_t meta = level->getData(x, y, z);
	bool_t hasWall = 0;
	if ((meta & 1) && canAttachTo(level, x, y, z - 1)) hasWall = 1;
	if ((meta & 2) && canAttachTo(level, x + 1, y, z)) hasWall = 1;
	if ((meta & 4) && canAttachTo(level, x, y, z + 1)) hasWall = 1;
	if ((meta & 8) && canAttachTo(level, x - 1, y, z)) hasWall = 1;
	if (hasWall) return 1;

	int32_t aboveTile = level->getTile(x, y + 1, z);
	if (aboveTile == this->blockID || canAttachTo(level, x, y + 1, z)) {
		return meta != 0;
	}
	return 0;
}

void VineTile::neighborChanged(Level* level, int32_t x, int32_t y, int32_t z, int32_t, int32_t, int32_t, int32_t) {
	if (!this->canSurvive(level, x, y, z)) {
		level->setTile(x, y, z, 0, 3);
	}
}

void VineTile::tick(Level* level, int32_t x, int32_t y, int32_t z, Random* random) {
	if (!this->canSurvive(level, x, y, z)) {
		level->setTile(x, y, z, 0, 3);
		return;
	}

	if (random->genrand_int32() % 4 == 0) {
		int32_t meta = level->getData(x, y, z);
		if (y > 1 && level->isEmptyTile(x, y - 1, z)) {
			level->setTileAndData(x, y - 1, z, this->blockID, meta, 3);
		}
	}
}

int32_t VineTile::getResource(int32_t, Random*) {
	return 0;
}

int32_t VineTile::getResourceCount(Random*) {
	return 0;
}

void VineTile::playerDestroy(Level* level, Player* player, int32_t x, int32_t y, int32_t z, int32_t meta) {
	if(!level->isClientMaybe && player && player->getSelectedItem() && player->getSelectedItem()->itemClass == Item::shears && Item::shears) {
		this->popResource(level, x, y, z, ItemInstance(this->blockID, 1, 0));
	} else {
		Tile::playerDestroy(level, player, x, y, z, meta);
	}
}

bool_t VineTile::onFertilized(Level* level, int32_t x, int32_t y, int32_t z) {
	int32_t data = level->getData(x, y, z);
	for (int32_t ny = y - 1; ny > 0; --ny) {
		if (level->isEmptyTile(x, ny, z)) {
			level->setTileAndData(x, ny, z, this->blockID, data, 3);
			return 1;
		} else if (level->getTile(x, ny, z) != this->blockID) {
			break;
		}
	}
	return 0;
}
