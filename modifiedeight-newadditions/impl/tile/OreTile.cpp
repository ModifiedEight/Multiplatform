#include <tile/OreTile.hpp>
#include <util/Random.hpp>
#include <item/Item.hpp>
#include <tile/material/Material.hpp>
#include <level/Level.hpp>
#include <entity/particles/ParticleType.hpp>

OreTile::OreTile(int32_t a2, const std::string& a3) : Tile(a2, a3, Material::stone){
	this->setTicking(1);
}

OreTile::~OreTile() {
}
static void spawnGlowstoneDust(Level* level, int32_t x, int32_t y, int32_t z) {
	float v5 = (float)x;
	float gr = 1.0f, gg = 0.95f, gb = 0.05f;
	for(int32_t i = 0; i != 6; ++i) {
		float v11 = v5 + level->random.nextFloat();
		float v12 = (float)y + level->random.nextFloat();
		float v13 = (float)z;
		float v14 = (float)z + level->random.nextFloat();
		if(i) {
			switch(i) {
				case 1:
					if(!level->isSolidBlockingTile(x, y - 1, z)) {
						v12 = (float)y - 0.0625;
					}
					break;
				case 2:
					if(!level->isSolidBlockingTile(x, y, z + 1)) {
						v14 = (float)(z + 1) + 0.0625;
					}
					break;
				case 3:
					if(!level->isSolidBlockingTile(x, y, z - 1)) {
						v14 = v13 - 0.0625;
					}
					break;
				case 4:
					if(!level->isSolidBlockingTile(x + 1, y, z)) {
						v11 = (float)(x + 1) + 0.0625;
					}
					break;
				default:
					if(i == 5 && !level->isSolidBlockingTile(x - 1, y, z)) {
						v11 = v5 - 0.0625;
					}
					break;
			}
		} else if(!level->isSolidBlockingTile(x, y + 1, z)) {
			v12 = (float)(y + 1) + 0.0625;
		}
		if(v11 < v5 || v11 > (float)(x + 1) || v12 < 0.0 || v12 > (float)(y + 1) || v14 < v13 || v14 > (float)(z + 1)) {
			level->addParticle(PT_RED_DUST, v11, v12, v14, gr, gg, gb, 0);
		}
	}
}

void OreTile::animateTick(Level* level, int32_t x, int32_t y, int32_t z, Random* random) {
	if (Tile::glowstoneOre && this->blockID == Tile::glowstoneOre->blockID) {
		spawnGlowstoneDust(level, x, y, z);
	}
}

bool_t OreTile::use(Level* level, int32_t x, int32_t y, int32_t z, Player* player) {
	if (Tile::glowstoneOre && this->blockID == Tile::glowstoneOre->blockID) {
		spawnGlowstoneDust(level, x, y, z);
	}
	return Tile::use(level, x, y, z, player);
}

void OreTile::stepOn(Level* level, int32_t x, int32_t y, int32_t z, Entity* entity) {
	if (Tile::glowstoneOre && this->blockID == Tile::glowstoneOre->blockID) {
		spawnGlowstoneDust(level, x, y, z);
	}
}

void OreTile::attack(Level* level, int32_t x, int32_t y, int32_t z, Player* player) {
	if (Tile::glowstoneOre && this->blockID == Tile::glowstoneOre->blockID) {
		spawnGlowstoneDust(level, x, y, z);
	}
}
int32_t OreTile::getResource(int32_t a2, Random* a3) {
	int32_t result; // r0
	Item* v4;
	result = this->blockID;
	if(result == Tile::coalOre->blockID) {
		v4 = Item::coal;
	} else if(result == Tile::emeraldOre->blockID) {
		v4 = Item::emerald;
	} else if(Tile::netherQuartz && result == Tile::netherQuartz->blockID) {
		v4 = Item::netherQuartz;
	} else if(Tile::glowstoneOre && result == Tile::glowstoneOre->blockID) {
		v4 = Item::yellowDust;
	} else {
		if(result != Tile::lapisOre->blockID) {
			return result;
		}
		v4 = Item::dye_powder;
	}
	return v4 ? v4->itemID : result;
}
int32_t OreTile::getResourceCount(Random* a2) {
	if(this->blockID == Tile::lapisOre->blockID) {
		return a2->genrand_int32() % 5 + 4;
	} else if(Tile::glowstoneOre && this->blockID == Tile::glowstoneOre->blockID) {
		return a2->genrand_int32() % 2 + 3;
	} else {
		return 1;
	}
}
int32_t OreTile::getSpawnResourcesAuxValue(int32_t a2) {
	if(this->blockID == Tile::lapisOre->blockID) {
		return 4;
	} else {
		return 0;
	}
}
