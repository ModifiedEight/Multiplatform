#include <entity/Cod.hpp>
#include <item/Item.hpp>

Cod::Cod(Level* level)
	: AbstractFish(level) {
	this->setSize(0.5f, 0.3f);
	this->entityRenderId = COD;
}

Cod::~Cod() {
}

int32_t Cod::getEntityTypeId() const {
	return 27;
}

static std::string _codTex = "mob/cod.png";
std::string* Cod::getTexture() {
	return &_codTex;
}

int32_t Cod::getDeathLoot() {
	return Item::fish_raw ? Item::fish_raw->itemID : 0;
}
