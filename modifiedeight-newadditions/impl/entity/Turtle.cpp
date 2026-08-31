#include <entity/Turtle.hpp>
#include <level/Level.hpp>
#include <tile/Tile.hpp>
#include <math.h>

Turtle::Turtle(Level* level)
	: Animal(level) {
	this->entityRenderId = TURTLE;
	this->setSize(1.2f, 0.4f);
	this->stepHeight = 1.0f;
}

Turtle::~Turtle() {
}

int32_t Turtle::getEntityTypeId() const {
	return 39;
}

static std::string _turtleTex = "mob/big_sea_turtle.png";
std::string* Turtle::getTexture() {
	return &_turtleTex;
}

int32_t Turtle::getMaxHealth() {
	return 30;
}

float Turtle::getBaseSpeed() {
	return 0.2f;
}

int32_t Turtle::getAmbientSoundInterval() {
	return 200;
}

const char_t* Turtle::getAmbientSound() {
	return "mob.turtle.idle";
}

std::string Turtle::getHurtSound() {
	return "mob.turtle.hurt";
}

std::string Turtle::getDeathSound() {
	return "mob.turtle.death";
}

void Turtle::aiStep() {
	this->stepHeight = 1.0f;
	Animal::aiStep();
	if (this->isInWater() && this->isCollidedHorizontally) {
		this->motionY = 0.3f;
	}
}

bool_t Turtle::canSpawn() {
	if (!this->level) return 0;
	int32_t x = (int32_t)floorf(this->posX);
	int32_t y = (int32_t)floorf(this->posY);
	int32_t z = (int32_t)floorf(this->posZ);
	if (y <= 1 || y >= 127) return 0;
	int32_t tileBelow = this->level->getTile(x, y - 1, z);
	return (tileBelow == Tile::sand->blockID || tileBelow == Tile::grass->blockID || this->isInWater()) && PathfinderMob::canSpawn();
}

Mob* Turtle::getBreedOffspring(Animal*) {
	return new Turtle(this->level);
}
