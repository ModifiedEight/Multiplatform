#include <entity/Fox.hpp>
#include <level/Level.hpp>
#include <tile/Tile.hpp>
#include <math.h>

Fox::Fox(Level* level)
	: Animal(level) {
	this->entityRenderId = FOX;
	this->setSize(0.6f, 0.7f);
	this->foxType = 0;
	this->isFoxSleeping = 0;
	this->isFoxSitting = 0;
	this->isFoxCrouching = 0;
}

Fox::~Fox() {
}

int32_t Fox::getEntityTypeId() const {
	return 38;
}

static std::string _foxTex = "mob/fox.png";
static std::string _snowFoxTex = "mob/snow_fox.png";

std::string* Fox::getTexture() {
	if (this->foxType == 1) {
		return &_snowFoxTex;
	}
	return &_foxTex;
}

int32_t Fox::getMaxHealth() {
	return 10;
}

float Fox::getBaseSpeed() {
	return 0.3f;
}

int32_t Fox::getAmbientSoundInterval() {
	return 120;
}

const char_t* Fox::getAmbientSound() {
	if (this->isFoxSleeping) return "mob.fox.sleep";
	if (this->random.genrand_int32() % 4 == 0) return "mob.fox.sniff";
	return "mob.fox.idle";
}

std::string Fox::getHurtSound() {
	return "mob.fox.hurt";
}

std::string Fox::getDeathSound() {
	return "mob.fox.death";
}

void Fox::aiStep() {
	Animal::aiStep();
}

bool_t Fox::canSpawn() {
	if (!this->level) return 0;
	int32_t x = (int32_t)floorf(this->posX);
	int32_t y = (int32_t)floorf(this->posY);
	int32_t z = (int32_t)floorf(this->posZ);
	if (y <= 1 || y >= 127) return 0;
	int32_t tileBelow = this->level->getTile(x, y - 1, z);
	if (tileBelow == Tile::snow->blockID || tileBelow == Tile::topSnow->blockID) {
		this->foxType = 1;
	}
	return (tileBelow == Tile::grass->blockID || tileBelow == Tile::snow->blockID || tileBelow == Tile::topSnow->blockID) && PathfinderMob::canSpawn();
}

Mob* Fox::getBreedOffspring(Animal*) {
	return new Fox(this->level);
}
