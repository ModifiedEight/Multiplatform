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
	int32_t tileBelow = this->level->getTile(x, y - 1, z);
	int32_t tileAt = this->level->getTile(x, y, z);
	if (tileAt != 0 && tileAt != Tile::waterLily->blockID && tileAt != Tile::tallgrass->blockID && tileAt != Tile::water->blockID && tileAt != Tile::calmWater->blockID && Tile::tiles[tileAt] && Tile::tiles[tileAt]->isSolidRender()) return 0;
	return (tileBelow == Tile::grass->blockID || tileBelow == Tile::dirt->blockID || tileBelow == Tile::sand->blockID || tileBelow == Tile::leaves->blockID || tileBelow == Tile::waterLily->blockID || tileBelow == Tile::water->blockID || tileBelow == Tile::calmWater->blockID || tileBelow == Tile::clay->blockID || tileBelow == Tile::gravel->blockID || tileBelow == Tile::wood->blockID || tileBelow == Tile::treeTrunk->blockID);
}

bool_t Frog::removeWhenFarAway() {
	return !this->isInLove() && this->getAge() == 0;
}

int32_t Frog::getMaxSpawnClusterSize() {
	return 6;
}

Mob* Frog::getBreedOffspring(Animal*) {
	return new Frog(this->level);
}
