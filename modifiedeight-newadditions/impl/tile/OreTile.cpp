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
void OreTile::animateTick(Level* level, int32_t x, int32_t y, int32_t z, Random* random) {
	if (Tile::glowstoneOre && this->blockID == Tile::glowstoneOre->blockID) {
		if ((random->genrand_int32() & 3) == 0) {
			float px = (float)x + random->nextFloat();
			float py = (float)y + random->nextFloat();
			float pz = (float)z + random->nextFloat();
			level->addParticle(PT_RED_DUST, px, py, pz, 1.0, 1.0, 0.0, 0);
		}
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
