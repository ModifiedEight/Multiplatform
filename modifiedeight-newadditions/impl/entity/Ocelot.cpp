#include <entity/Ocelot.hpp>
#include <level/Level.hpp>
#include <entity/Player.hpp>
#include <inventory/Inventory.hpp>
#include <item/Item.hpp>
#include <math/Mth.hpp>
#include <cmath>

static std::string _ocelotTex = "mob/ocelot.png";

Ocelot::Ocelot(Level* level)
	: Animal(level) {
	this->entityRenderId = OCELOT;
	this->setSize(0.6f, 0.7f);
	this->isTrusting = 0;
	this->isSitting = 0;
}

Ocelot::~Ocelot() {
}

int32_t Ocelot::getEntityTypeId() const {
	return 22;
}

std::string* Ocelot::getTexture() {
	return &_ocelotTex;
}

int32_t Ocelot::getMaxHealth() {
	return 10;
}

float Ocelot::getBaseSpeed() {
	return 0.35f;
}

int32_t Ocelot::getAmbientSoundInterval() {
	return 160;
}

const char_t* Ocelot::getAmbientSound() {
	if (this->random.genrand_int32() % 2 == 0) {
		return "mob.cat.meow";
	}
	return "mob.cat.purr";
}

std::string Ocelot::getHurtSound() {
	return "mob.cat.hitt";
}

std::string Ocelot::getDeathSound() {
	return "mob.cat.hitt";
}

void Ocelot::aiStep() {
	Animal::aiStep();
}

bool_t Ocelot::interactWithPlayer(Player* player) {
	if (!player || !player->inventory) return Animal::interactWithPlayer(player);
	ItemInstance* held = player->inventory->getSelected();
	if (held && (held->getId() == Item::fish_raw->itemID || (Item::salmon_raw && held->getId() == Item::salmon_raw->itemID))) {
		if (!this->isTrusting) {
			this->isTrusting = 1;
			if (!this->level->isClientMaybe) {
				held->count--;
				this->level->playSound(this, "random.pop", 1.0f, 1.0f);
			}
			return 1;
		}
	}
	return Animal::interactWithPlayer(player);
}

bool_t Ocelot::canSpawn() {
	return Animal::canSpawn();
}

Mob* Ocelot::getBreedOffspring(Animal* mate) {
	Ocelot* baby = new Ocelot(this->level);
	return baby;
}
