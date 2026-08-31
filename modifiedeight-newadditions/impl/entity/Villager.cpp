#include <entity/Villager.hpp>
#include <entity/ZombieVillager.hpp>
#include <entity/ai/goals/FloatGoal.hpp>
#include <entity/ai/goals/LookAtPlayerGoal.hpp>
#include <entity/ai/goals/OpenDoorGoal.hpp>
#include <entity/ai/goals/PanicGoal.hpp>
#include <entity/ai/goals/RandomLookAroundGoal.hpp>
#include <entity/ai/goals/RandomStrollGoal.hpp>
#include <entity/path/PathNavigation.hpp>
#include <gui/screens/VillagerTradeScreen.hpp>
#include <item/Item.hpp>
#include <item/ItemInstance.hpp>
#include <level/Level.hpp>
#include <level/MobSpawner.hpp>
#include <tile/Tile.hpp>
#include <Minecraft.hpp>
#include <entity/LocalPlayer.hpp>
#include <nbt/CompoundTag.hpp>
#include <cmath>

Villager::Villager(Level* level)
	: PathfinderMob(level) {
	this->zombieHitCount = 0;
	this->tradeCount = 0;
	this->houseX = 0;
	this->houseY = 0;
	this->houseZ = 0;
	this->hasBed = 0;
	this->sleepTimer = 0;
	this->profession = this->random.genrand_int32() % 5;
	this->entityRenderId = VILLAGER;

	switch (this->profession) {
		case 0: this->skin = "mob/farmer.png"; break;
		case 1: this->skin = "mob/librarian.png"; break;
		case 2: this->skin = "mob/priest.png"; break;
		case 3: this->skin = "mob/smith.png"; break;
		case 4: this->skin = "mob/butcher.png"; break;
		default: this->skin = "mob/villager.png"; break;
	}

	this->getNavigation()->setCanOpenDoors(1);
	this->getNavigation()->setAvoidWater(1);
	this->goalSelector.addGoal(0, new FloatGoal(this), 1);
	this->goalSelector.addGoal(1, new PanicGoal(this, 1.25f), 1);
	this->goalSelector.addGoal(2, new OpenDoorGoal(this, 1), 1);
	this->goalSelector.addGoal(3, new LookAtPlayerGoal(this, 8.0f), 1);
	this->goalSelector.addGoal(4, new RandomLookAroundGoal(this), 1);
	this->goalSelector.addGoal(5, new RandomStrollGoal(this, 0.6f), 1);
	this->initTrades();
}


void Villager::initTrades() {
	struct TradeOption { int32_t inId, inCnt, inMeta, outId, outCnt, outMeta; };

	static TradeOption farmerTrades[] = {
		{ Item::wheat      ? Item::wheat->itemID      : 296, 18, 0, Item::emerald      ? Item::emerald->itemID      : 388, 1, 0 },
		{ Item::emerald    ? Item::emerald->itemID    : 388,  1, 0, Item::bread        ? Item::bread->itemID        : 297, 4, 0 },
		{ Item::carrot     ? Item::carrot->itemID     : 391, 24, 0, Item::emerald      ? Item::emerald->itemID      : 388, 1, 0 },
		{ Item::emerald    ? Item::emerald->itemID    : 388,  1, 0, Item::apple        ? Item::apple->itemID        : 260, 5, 0 },
		{ Item::potato     ? Item::potato->itemID     : 392, 24, 0, Item::emerald      ? Item::emerald->itemID      : 388, 1, 0 },
	};
	static TradeOption librarianTrades[] = {
		{ Item::paper      ? Item::paper->itemID      : 339, 24, 0, Item::emerald      ? Item::emerald->itemID      : 388, 1, 0 },
		{ Item::emerald    ? Item::emerald->itemID    : 388,  1, 0, Item::compass      ? Item::compass->itemID      : 345, 1, 0 },
		{ Item::book       ? Item::book->itemID       : 340,  8, 0, Item::emerald      ? Item::emerald->itemID      : 388, 1, 0 },
		{ Item::emerald    ? Item::emerald->itemID    : 388,  1, 0, Item::book         ? Item::book->itemID         : 340, 3, 0 },
		{ Item::emerald    ? Item::emerald->itemID    : 388,  5, 0, Item::clock        ? Item::clock->itemID        : 347, 1, 0 },
	};
	static TradeOption priestTrades[] = {
		{ Item::redStone   ? Item::redStone->itemID   : 331, 24, 0, Item::emerald      ? Item::emerald->itemID      : 388, 1, 0 },
		{ Item::goldIngot  ? Item::goldIngot->itemID  : 266,  8, 0, Item::emerald      ? Item::emerald->itemID      : 388, 1, 0 },
		{ Item::emerald    ? Item::emerald->itemID    : 388,  1, 0, Item::yellowDust   ? Item::yellowDust->itemID   : 348, 4, 0 },
		{ Item::emerald    ? Item::emerald->itemID    : 388,  4, 0, Item::sulphur      ? Item::sulphur->itemID      : 289, 4, 0 },
		{ Item::slimeBall  ? Item::slimeBall->itemID  : 341,  9, 0, Item::emerald      ? Item::emerald->itemID      : 388, 1, 0 },
	};
	static TradeOption smithTrades[] = {
		{ Item::ironIngot  ? Item::ironIngot->itemID  : 265, 15, 0, Item::emerald      ? Item::emerald->itemID      : 388, 1, 0 },
		{ Item::emerald    ? Item::emerald->itemID    : 388,  1, 0, Item::chestplate_chain ? Item::chestplate_chain->itemID : 303, 1, 0 },
		{ Item::coal       ? Item::coal->itemID       : 263, 16, 0, Item::emerald      ? Item::emerald->itemID      : 388, 1, 0 },
		{ Item::emerald    ? Item::emerald->itemID    : 388,  1, 0, Item::sword_iron   ? Item::sword_iron->itemID   : 267, 1, 0 },
		{ Item::emerald    ? Item::emerald->itemID    : 388,  4, 0, Item::helmet_diamond ? Item::helmet_diamond->itemID : 310, 1, 0 },
	};
	static TradeOption butcherTrades[] = {
		{ Item::porkChop_raw   ? Item::porkChop_raw->itemID   : 319, 14, 0, Item::emerald      ? Item::emerald->itemID      : 388, 1, 0 },
		{ Item::emerald        ? Item::emerald->itemID        : 388,  1, 0, Item::porkChop_cooked ? Item::porkChop_cooked->itemID : 320, 5, 0 },
		{ Item::beef_raw       ? Item::beef_raw->itemID       : 363, 14, 0, Item::emerald      ? Item::emerald->itemID      : 388, 1, 0 },
		{ Item::emerald        ? Item::emerald->itemID        : 388,  1, 0, Item::beef_cooked  ? Item::beef_cooked->itemID  : 364, 5, 0 },
		{ Item::chicken_raw    ? Item::chicken_raw->itemID    : 365, 14, 0, Item::chicken_cooked ? Item::chicken_cooked->itemID : 366, 5, 0 },
	};

	TradeOption* table;
	switch (this->profession % 5) {
		case 1: table = librarianTrades; break;
		case 2: table = priestTrades;    break;
		case 3: table = smithTrades;     break;
		case 4: table = butcherTrades;   break;
		default: table = farmerTrades;   break;
	}

	this->tradeCount = 3 + (this->random.genrand_int32() % 3);
	if (this->tradeCount > 5) this->tradeCount = 5;
	for (int32_t i = 0; i < this->tradeCount; ++i) {
		int32_t j = i % 5;
		this->trades[i].inputId     = table[j].inId;
		this->trades[i].inputCount  = table[j].inCnt;
		this->trades[i].inputMeta   = table[j].inMeta;
		this->trades[i].outputId    = table[j].outId;
		this->trades[i].outputCount = table[j].outCnt;
		this->trades[i].outputMeta  = table[j].outMeta;
	}
}




void Villager::convertToZombieVillager() {
	if (!this->level->isClientMaybe) {
		ZombieVillager* zv = new ZombieVillager(this->level);
		zv->moveTo(this->posX, this->posY, this->posZ, this->yaw, this->pitch);
		MobSpawner::finalizeMobSettings(zv, this->level, 0.0, 0.0, 0.0);
		this->level->addEntity(zv);
		this->remove();
	}
}

Villager::~Villager() {
}

int32_t Villager::getEntityTypeId() const {
	return 120;
}

int32_t Villager::getCreatureBaseType() {
	return 1;
}

void Villager::die(Entity* attacker) {
	if (attacker && attacker->isMob()) {
		int32_t typeId = attacker->getEntityTypeId();
		if (typeId == 32 || typeId == 121) {
			this->convertToZombieVillager();
			return;
		}
	}
	Mob::die(attacker);
}

bool_t Villager::hurt(Entity* attacker, int32_t damage) {
	if (attacker && attacker->isMob()) {
		int32_t typeId = attacker->getEntityTypeId();
		if (typeId == 32 || typeId == 121) {
			if (this->health - damage <= 0) {
				this->convertToZombieVillager();
				return 1;
			}
		}
	}
	return PathfinderMob::hurt(attacker, damage);
}

float Villager::getBaseSpeed() {
	return 0.23f;
}

int32_t Villager::getMaxHealth() {
	return 20;
}

void Villager::aiStep() {
	if (!this->level->isClientMaybe) {
		if (this->hurtTime == 0) {
			AABB searchBox{this->posX - 8.0f, this->posY - 4.0f, this->posZ - 8.0f,
			               this->posX + 8.0f, this->posY + 4.0f, this->posZ + 8.0f};
			std::vector<Entity*>* ents = this->level->getEntities(this, searchBox);
			if (ents) {
				for (Entity* e : *ents) {
					if (e && !e->isDead && (e->getEntityTypeId() == 32 || e->getEntityTypeId() == 121)) {
						float dx = this->posX - e->posX;
						float dz = this->posZ - e->posZ;
						float len = sqrtf(dx * dx + dz * dz);
						if (len > 0.1f) {
							dx /= len; dz /= len;
							this->getNavigation()->moveTo(this->posX + dx * 10.0f, this->posY, this->posZ + dz * 10.0f, 1.25f);
						}
						break;
					}
				}
			}
		}

		if (!this->level->isDay()) {
			if (!this->hasBed && (this->random.genrand_int32() % 20 == 0)) {
				int vx = (int)std::floor(this->posX);
				int vy = (int)std::floor(this->posY);
				int vz = (int)std::floor(this->posZ);
				for (int dy = -2; dy <= 2 && !this->hasBed; ++dy) {
					for (int dx = -16; dx <= 16 && !this->hasBed; ++dx) {
						for (int dz = -16; dz <= 16 && !this->hasBed; ++dz) {
							int tileId = this->level->getTile(vx + dx, vy + dy, vz + dz);
							if (tileId == 26 || tileId == 220 || (Tile::bed && tileId == Tile::bed->blockID)) {
								this->hasBed = true;
								this->houseX = vx + dx;
								this->houseY = vy + dy;
								this->houseZ = vz + dz;
								break;
							}
						}
					}
				}
			}

			if (this->hasBed) {
				float bx = (float)this->houseX + 0.5f;
				float by = (float)this->houseY;
				float bz = (float)this->houseZ + 0.5f;
				float dist = this->distanceTo(bx, by, bz);
				if (dist > 1.2f) {
					this->getNavigation()->moveTo(bx, by, bz, 0.85f);
					this->sleepTimer = 0;
					this->setSneaking(false);
				} else {
					this->sleepTimer++;
					this->motionX = 0.0f;
					this->motionZ = 0.0f;
					this->moveTo(bx, (float)this->houseY + 0.55f, bz, this->yaw, 0.0f);
					this->setSneaking(true);
				}
			}
		} else {
			this->setSneaking(false);
			this->sleepTimer = 0;
			Player* nearbyPlayer = this->level->getNearestPlayer(this, 6.0f);
			if (nearbyPlayer && !nearbyPlayer->isDead) {
				float dx = nearbyPlayer->posX - this->posX;
				float dz = nearbyPlayer->posZ - this->posZ;
				float dy = (nearbyPlayer->posY + 1.2f) - (this->posY + 1.2f);
				float d = sqrtf(dx * dx + dz * dz);
				if (d > 0.05f) {
					float targetYaw = (float)(atan2(dz, dx) * 180.0 / 3.141592653589793) - 90.0f;
					float targetPitch = (float)(-(atan2(dy, d) * 180.0 / 3.141592653589793));
					this->yaw = targetYaw;
					this->pitch = targetPitch;
					this->lookAt(nearbyPlayer, 30.0f, 30.0f);
				}
				if (d < 3.0f) {
					this->getNavigation()->stop();
					this->motionX = 0.0f;
					this->motionZ = 0.0f;
				}
			} else if (this->getNavigation()->isDone() && (this->random.genrand_int32() % 80 == 0)) {
				float centerBaseX = this->hasBed ? (float)this->houseX : this->posX;
				float centerBaseZ = this->hasBed ? (float)this->houseZ : this->posZ;
				float wanderX = centerBaseX + (float)((this->random.genrand_int32() % 25) - 12);
				float wanderZ = centerBaseZ + (float)((this->random.genrand_int32() % 25) - 12);
				int32_t wanderY = this->level->getHeightmap((int32_t)wanderX, (int32_t)wanderZ);
				if (wanderY > 0) {
					this->getNavigation()->moveTo(wanderX, (float)wanderY, wanderZ, 0.55f);
				}
			}
		}

	}
	Mob::aiStep();
}

void Villager::push(Entity* attacker) {
	if (attacker && attacker->isPlayer()) {
		return;
	}
	PathfinderMob::push(attacker);
}

int32_t Villager::getDeathLoot() {

	return 0;
}

const char_t* Villager::getAmbientSound() {
	return "mob.villager";
}

std::string Villager::getHurtSound() {
	return "mob.villagerhurt";
}

std::string Villager::getDeathSound() {
	return "mob.villagerdie";
}

bool_t Villager::interactWithPlayer(Player* player) {
	if (player->isLocalPlayer()) {
		LocalPlayer* lp = (LocalPlayer*)player;
		if (this->tradeCount <= 0) {
			this->initTrades();
		}
		lp->minecraft->setScreen(new VillagerTradeScreen(player, this));
		this->level->playSound(this, "mob.villager.yes", 1.0f, 1.0f);
		return 1;
	}
	return 0;
}

void Villager::readAdditionalSaveData(CompoundTag* tag) {
	PathfinderMob::readAdditionalSaveData(tag);
	this->houseX = tag->getInt("HouseX");
	this->houseY = tag->getInt("HouseY");
	this->houseZ = tag->getInt("HouseZ");
	this->hasBed = tag->getByte("HasBed") != 0;
	this->profession = tag->getInt("Profession");
	this->tradeCount = tag->getInt("TradeCount");
	if (this->tradeCount > 5) this->tradeCount = 5;
	for (int32_t i = 0; i < this->tradeCount; ++i) {
		char keyBuf[32];
		snprintf(keyBuf, sizeof(keyBuf), "TradeInId%d", i);
		this->trades[i].inputId = tag->getInt(keyBuf);
		snprintf(keyBuf, sizeof(keyBuf), "TradeInCnt%d", i);
		this->trades[i].inputCount = tag->getInt(keyBuf);
		snprintf(keyBuf, sizeof(keyBuf), "TradeInMeta%d", i);
		this->trades[i].inputMeta = tag->getInt(keyBuf);
		snprintf(keyBuf, sizeof(keyBuf), "TradeOutId%d", i);
		this->trades[i].outputId = tag->getInt(keyBuf);
		snprintf(keyBuf, sizeof(keyBuf), "TradeOutCnt%d", i);
		this->trades[i].outputCount = tag->getInt(keyBuf);
		snprintf(keyBuf, sizeof(keyBuf), "TradeOutMeta%d", i);
		this->trades[i].outputMeta = tag->getInt(keyBuf);
	}
	if (this->tradeCount <= 0) {
		this->initTrades();
	}
}

void Villager::addAdditonalSaveData(CompoundTag* tag) {
	PathfinderMob::addAdditonalSaveData(tag);
	tag->putInt("HouseX", this->houseX);
	tag->putInt("HouseY", this->houseY);
	tag->putInt("HouseZ", this->houseZ);
	tag->putByte("HasBed", this->hasBed ? 1 : 0);
	tag->putInt("Profession", this->profession);
	tag->putInt("TradeCount", this->tradeCount);
	for (int32_t i = 0; i < this->tradeCount; ++i) {
		char keyBuf[32];
		snprintf(keyBuf, sizeof(keyBuf), "TradeInId%d", i);
		tag->putInt(keyBuf, this->trades[i].inputId);
		snprintf(keyBuf, sizeof(keyBuf), "TradeInCnt%d", i);
		tag->putInt(keyBuf, this->trades[i].inputCount);
		snprintf(keyBuf, sizeof(keyBuf), "TradeInMeta%d", i);
		tag->putInt(keyBuf, this->trades[i].inputMeta);
		snprintf(keyBuf, sizeof(keyBuf), "TradeOutId%d", i);
		tag->putInt(keyBuf, this->trades[i].outputId);
		snprintf(keyBuf, sizeof(keyBuf), "TradeOutCnt%d", i);
		tag->putInt(keyBuf, this->trades[i].outputCount);
		snprintf(keyBuf, sizeof(keyBuf), "TradeOutMeta%d", i);
		tag->putInt(keyBuf, this->trades[i].outputMeta);
	}
}
