#include <entity/Salmon.hpp>
#include <item/Item.hpp>

Salmon::Salmon(Level* level)
	: AbstractFish(level) {
	this->setSize(0.7f, 0.4f);
	this->entityRenderId = SALMON;
}

Salmon::~Salmon() {
}

int32_t Salmon::getEntityTypeId() const {
	return 28;
}

static std::string _salmonTex = "mob/salmon.png";
std::string* Salmon::getTexture() {
	return &_salmonTex;
}

int32_t Salmon::getDeathLoot() {
	return Item::salmon_raw ? Item::salmon_raw->itemID : (Item::fish_raw ? Item::fish_raw->itemID : 0);
}
