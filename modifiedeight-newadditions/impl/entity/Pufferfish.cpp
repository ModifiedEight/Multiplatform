#include <entity/Pufferfish.hpp>
#include <item/Item.hpp>
#include <level/Level.hpp>
#include <entity/Player.hpp>

Pufferfish::Pufferfish(Level* level)
	: AbstractFish(level) {
	this->setSize(0.35f, 0.35f);
	this->puffState = 0;
	this->inflateCounter = 0;
	this->deflateTimer = 0;
	this->entityRenderId = PUFFERFISH;
}

Pufferfish::~Pufferfish() {
}

int32_t Pufferfish::getEntityTypeId() const {
	return 29;
}

static std::string _pufferTex = "mob/pufferfish.png";
std::string* Pufferfish::getTexture() {
	return &_pufferTex;
}

int32_t Pufferfish::getDeathLoot() {
	return Item::pufferfish ? Item::pufferfish->itemID : 0;
}

void Pufferfish::aiStep() {
	AbstractFish::aiStep();

	if (!this->level) return;
	Player* player = this->level->getNearestPlayer(this, 4.0f);
	if (player && !player->abilities.instabuild) {
		if (this->puffState == 0) {
			this->puffState = 1;
		} else if (this->puffState == 1 && ++this->inflateCounter > 20) {
			this->puffState = 2;
			this->inflateCounter = 0;
		}
		this->deflateTimer = 60;
	} else if (this->deflateTimer > 0) {
		if (--this->deflateTimer == 0) {
			if (this->puffState > 0) {
				--this->puffState;
				if (this->puffState > 0) {
					this->deflateTimer = 40;
				}
			}
		}
	}
}
