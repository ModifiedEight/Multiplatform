#include <entity/Frog.hpp>
#include <level/Level.hpp>
#include <level/biome/Biome.hpp>
#include <tile/Tile.hpp>
#include <math.h>

Frog::Frog(Level* level)
	: Animal(level) {
	this->entityRenderId = FROG;
	this->setSize(0.5f, 0.5f);
	this->stepHeight = 0.6f;
}

Frog::~Frog() {
}

int32_t Frog::getEntityTypeId() const {
	return 40;
}

static std::string _frogTex = "mob/frog.png";
std::string* Frog::getTexture() {
	return &_frogTex;
}

int32_t Frog::getMaxHealth() {
	return 10;
}

float Frog::getBaseSpeed() {
	return 0.25f;
}

int32_t Frog::getAmbientSoundInterval() {
	return 120;
}

const char_t* Frog::getAmbientSound() {
	return "mob.frog.ambient";
}

std::string Frog::getHurtSound() {
	return "mob.frog.hurt";
}

std::string Frog::getDeathSound() {
	return "mob.frog.death";
}

bool_t Frog::canSpawn() {
	if (!this->level) return 0;
	int32_t x = (int32_t)floorf(this->posX);
	int32_t y = (int32_t)floorf(this->posY);
	int32_t z = (int32_t)floorf(this->posZ);
	if (y <= 1 || y >= 127) return 0;
	Biome* b = this->level->getBiome(x, z);
	if (b != Biome::swampland && b != Biome::equatorialRainforest) return 0;
	int32_t tileBelow = this->level->getTile(x, y - 1, z);
	return (tileBelow == Tile::grass->blockID || tileBelow == Tile::dirt->blockID || tileBelow == Tile::sand->blockID) && PathfinderMob::canSpawn();
}

Mob* Frog::getBreedOffspring(Animal*) {
	return new Frog(this->level);
}
