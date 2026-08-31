#include <entity/TropicalFish.hpp>
#include <item/Item.hpp>

TropicalFish::TropicalFish(Level* level)
	: AbstractFish(level) {
	this->setSize(0.4f, 0.4f);
	this->variant = this->random.genrand_int32() % 6;
	this->entityRenderId = TROPICAL_FISH;
}

TropicalFish::~TropicalFish() {
}

int32_t TropicalFish::getEntityTypeId() const {
	return 30;
}

static std::string _tropicalTex = "mob/tropical_a.png";
std::string* TropicalFish::getTexture() {
	return &_tropicalTex;
}

int32_t TropicalFish::getDeathLoot() {
	return Item::clownfish ? Item::clownfish->itemID : (Item::fish_raw ? Item::fish_raw->itemID : 0);
}
