#include <entity/Giant.hpp>
#include <level/Level.hpp>

Giant::Giant(Level* level)
	: Monster(level) {
	this->entityRenderId = GIANT;
	this->setSize(0.6f * 6.0f, 1.8f * 6.0f);
	this->health = 100;
	this->stepHeight = 1.5f;
}

Giant::~Giant() {
}

int32_t Giant::getEntityTypeId() const {
	return 53;
}

static std::string _giantTex = "mob/zombie.png";
std::string* Giant::getTexture() {
	return &_giantTex;
}

int32_t Giant::getMaxHealth() {
	return 100;
}

float Giant::getBaseSpeed() {
	return 0.5f;
}

int32_t Giant::getAttackDamage(Entity*) {
	return 30;
}

const char_t* Giant::getAmbientSound() {
	return "mob.zombie";
}

std::string Giant::getHurtSound() {
	return "mob.zombiehurt";
}

std::string Giant::getDeathSound() {
	return "mob.zombiedeath";
}

void Giant::aiStep() {
	Monster::aiStep();
}
