#include <entity/Wolf.hpp>
#include <level/Level.hpp>
#include <level/biome/Biome.hpp>
#include <math/Mth.hpp>
#include <cmath>

Wolf::Wolf(Level* level)
	: Animal(level) {
	this->entityRenderId = WOLF;
	this->setSize(0.6f, 0.8f);
	this->wolfType = 0;
	this->isAngry = 0;
	this->isTame = 0;
	this->isSitting = 0;
}

Wolf::~Wolf() {
}

int32_t Wolf::getEntityTypeId() const {
	return 14;
}

static std::string _wolfTex = "mob/wolf.png";
static std::string _wolfAngryTex = "mob/wolf_angry.png";
static std::string _wolfTameTex = "mob/wolf_tame.png";
static std::string _snowWolfTex = "mob/snow_wolf.png";
static std::string _snowWolfAngryTex = "mob/snow_wolf_angry.png";
static std::string _snowWolfTameTex = "mob/snow_wolf_tame.png";

std::string* Wolf::getTexture() {
	if (this->wolfType == 1) {
		if (this->isAngry) return &_snowWolfAngryTex;
		if (this->isTame) return &_snowWolfTameTex;
		return &_snowWolfTex;
	}
	if (this->isAngry) return &_wolfAngryTex;
	if (this->isTame) return &_wolfTameTex;
	return &_wolfTex;
}

int32_t Wolf::getMaxHealth() {
	return this->isTame ? 20 : 8;
}

float Wolf::getBaseSpeed() {
	return 0.3f;
}

int32_t Wolf::getAmbientSoundInterval() {
	return 120;
}

const char_t* Wolf::getAmbientSound() {
	if (this->isAngry) return "mob.wolf.growl";
	if (this->random.genrand_int32() % 3 == 0) {
		return (this->isTame && this->health < 10) ? "mob.wolf.whine" : "mob.wolf.panting";
	}
	return "mob.wolf.bark";
}

std::string Wolf::getHurtSound() {
	return "mob.wolf.hurt";
}

std::string Wolf::getDeathSound() {
	return "mob.wolf.death";
}

void Wolf::aiStep() {
	Animal::aiStep();
}

bool_t Wolf::canSpawn() {
	if (!this->level) return 0;
	int32_t x = Mth::floor(this->posX);
	int32_t y = Mth::floor(this->posY);
	int32_t z = Mth::floor(this->posZ);
	if (y <= 1 || y >= 127) return 0;
	int32_t tileBelow = this->level->getTile(x, y - 1, z);
	if (tileBelow == 78 || tileBelow == 80) { // topSnow or snow
		this->wolfType = 1;
	}
	Biome* b = this->level->getBiome(x, z);
	if (b == Biome::taiga || b == Biome::tundra || b == Biome::icePeaks || b == Biome::iceDesert) {
		this->wolfType = 1;
	}
	return (tileBelow == 2 || tileBelow == 78 || tileBelow == 80) && PathfinderMob::canSpawn();
}

Mob* Wolf::getBreedOffspring(Animal*) {
	Wolf* baby = new Wolf(this->level);
	baby->wolfType = this->wolfType;
	return baby;
}
