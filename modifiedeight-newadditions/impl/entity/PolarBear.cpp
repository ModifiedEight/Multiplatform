#include <entity/PolarBear.hpp>
#include <level/Level.hpp>
#include <tile/Tile.hpp>
#include <math.h>

PolarBear::PolarBear(Level* level)
	: Animal(level) {
	this->entityRenderId = POLAR_BEAR;
	this->setSize(1.3f, 1.4f);
}

PolarBear::~PolarBear() {
}

int32_t PolarBear::getEntityTypeId() const {
	return 26;
}

static std::string _polarBearTex = "mob/polarbear.png";
std::string* PolarBear::getTexture() {
	return &_polarBearTex;
}

int32_t PolarBear::getMaxHealth() {
	return 30;
}

float PolarBear::getBaseSpeed() {
	return 0.25f;
}

int32_t PolarBear::getAmbientSoundInterval() {
	return 120;
}

const char_t* PolarBear::getAmbientSound() {
	return "mob.polarbear.idle";
}

std::string PolarBear::getHurtSound() {
	return "mob.polarbear.hurt";
}

std::string PolarBear::getDeathSound() {
	return "mob.polarbear.death";
}

void PolarBear::aiStep() {
	Animal::aiStep();
}

bool_t PolarBear::canSpawn() {
	if (!this->level) return 0;
	int32_t x = (int32_t)floorf(this->posX);
	int32_t y = (int32_t)floorf(this->posY);
	int32_t z = (int32_t)floorf(this->posZ);
	if (y <= 1 || y >= 127) return 0;
	int32_t tileBelow = this->level->getTile(x, y - 1, z);
	return (tileBelow == Tile::grass->blockID || tileBelow == Tile::snow->blockID || tileBelow == Tile::ice->blockID || tileBelow == Tile::topSnow->blockID) && PathfinderMob::canSpawn();
}

Mob* PolarBear::getBreedOffspring(Animal*) {
	return new PolarBear(this->level);
}
