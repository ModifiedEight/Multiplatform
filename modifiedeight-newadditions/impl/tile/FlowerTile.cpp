#include <tile/FlowerTile.hpp>
#include <level/Level.hpp>
#include <item/ItemInstance.hpp>
#include <entity/ItemEntity.hpp>

bool_t FlowerTile::_randomWalk(struct Level* level, int32_t& x, int32_t& y, int32_t& z, int32_t a6) {
	Random* p_randomInstance;
	int32_t i;
	int32_t zz;
	uint32_t v13;

	p_randomInstance = &level->random;
	for(i = 0;; ++i) {
		if(i >= a6 / 16) {
			return a6 > 15;
		}
		x += p_randomInstance->genrand_int32() % 3 - 1;
		v13 = p_randomInstance->genrand_int32() % 3 - 1;
		y += (int32_t)(p_randomInstance->genrand_int32() % 3 * v13) / 2;
		zz = p_randomInstance->genrand_int32() % 3 - 1 + z;
		z = zz;
		if(level->getTile(x, y - 1, zz) != Tile::grass->blockID || level->isSolidBlockingTile(x, y, z)) {
			break;
		}
	}
	return 0;
}
FlowerTile::FlowerTile(int32_t id, const std::string& name)
	: Bush(id, name) {
}

FlowerTile::~FlowerTile() {
}
bool_t FlowerTile::onFertilized(Level* level, int32_t x, int32_t y, int32_t z) {
	Random* p_random;
	int32_t i;
	int8_t v10;
	Tile* v11;
	int32_t rx;
	int32_t ry;
	int32_t rz;

	p_random = &level->random;
	for(i = 16; i != 64; ++i) {
		rx = x;
		ry = y + 1;
		rz = z;
		if(this->_randomWalk(level, rx, ry, rz, i) && !level->getTile(rx, ry, rz)) {
			v10 = p_random->genrand_int32();
			v11 = Tile::rose;
			if((v10 & 7) != 0) {
				if(this == Tile::rose) {
LABEL_8:
					if(v11->canSurvive(level, rx, ry, rz)) {
						level->setTileAndData(rx, ry, rz, v11->blockID, 0, 3);
					}
					continue;
				}
			} else if(this != (FlowerTile*)Tile::rose) {
				goto LABEL_8;
			}
			v11 = Tile::flower;
			goto LABEL_8;
		}
	}
	return 1;
}

void FlowerTile::spawnResources(Level* level, int32_t x, int32_t y, int32_t z, int32_t data, float chance) {
	int32_t count = (data & 3) + 1;
	for (int32_t i = 0; i < count; ++i) {
		float xOffset = level->random.nextFloat() * 0.7f + 0.15f;
		float yOffset = level->random.nextFloat() * 0.7f + 0.15f;
		float zOffset = level->random.nextFloat() * 0.7f + 0.15f;
		ItemEntity* ent = new ItemEntity(level, (float)x + xOffset, (float)y + yOffset, (float)z + zOffset, ItemInstance(this->blockID, 1, 0));
		ent->delayBeforePickup = 10;
		level->addEntity(ent);
	}
}
