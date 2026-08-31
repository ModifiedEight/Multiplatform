#include <entity/FishingHook.hpp>
#include <entity/Player.hpp>
#include <inventory/Inventory.hpp>
#include <entity/ItemEntity.hpp>
#include <item/Item.hpp>
#include <item/ItemInstance.hpp>
#include <level/Level.hpp>
#include <math/Mth.hpp>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

FishingHook::FishingHook(Level* level, Player* player)
	: Entity(level) {
	this->owner = player;
	this->flightTime = 0;
	this->nibble = 0;
	this->timeUntilLured = 80 + this->random.genrand_int32() % 200;
	this->timeUntilHooked = 0;
	this->life = 0;
	this->inGround = 0;
	this->fishAngle = 0.0f;
	this->setSize(0.25f, 0.25f);
	this->entityRenderId = FISHING_HOOK;

	if (player) {
		player->fishing = this;
		float f = player->pitch;
		float g = player->yaw;
		float h = Mth::cos(-g * (float)(M_PI / 180.0) - (float)M_PI);
		float k = Mth::sin(-g * (float)(M_PI / 180.0) - (float)M_PI);
		float l = -Mth::cos(-f * (float)(M_PI / 180.0));
		float m = Mth::sin(-f * (float)(M_PI / 180.0));
		float px = player->posX - k * 0.3f;
		float py = player->posY + 1.62f;
		float pz = player->posZ - h * 0.3f;
		this->setPos(px, py, pz);
		this->prevPosX = px;
		this->prevPosY = py;
		this->prevPosZ = pz;

		float vx = -k;
		float vy = -(m / (l != 0.0f ? l : 0.0001f));
		if (vy < -5.0f) vy = -5.0f;
		if (vy > 5.0f) vy = 5.0f;
		float vz = -h;
		float len = sqrtf(vx * vx + vy * vy + vz * vz);
		float speed = 0.8f / (len > 0.0001f ? len : 1.0f) + 0.4f;
		this->motionX = vx * speed + (this->random.nextFloat() - 0.5f) * 0.05f;
		this->motionY = vy * speed + (this->random.nextFloat() - 0.5f) * 0.05f;
		this->motionZ = vz * speed + (this->random.nextFloat() - 0.5f) * 0.05f;
	}
}

FishingHook::FishingHook(Level* level, Player* player, float x, float y, float z)
	: FishingHook(level, player) {
	this->setPos(x, y, z);
	this->prevPosX = x;
	this->prevPosY = y;
	this->prevPosZ = z;
}

FishingHook::~FishingHook() {
	if (this->owner && this->owner->fishing == this) {
		this->owner->fishing = 0;
	}
}

int32_t FishingHook::getEntityTypeId() const {
	return 77;
}

void FishingHook::readAdditionalSaveData(CompoundTag* tag) {
}

void FishingHook::addAdditonalSaveData(CompoundTag* tag) {
}

bool_t FishingHook::shouldRenderAtSqrDistance(float dist) {
	return dist < 4096.0f;
}

void FishingHook::catchingFishLogic() {
	if (this->nibble > 0) {
		--this->nibble;
		if (this->nibble <= 0) {
			this->timeUntilLured = 60 + this->random.genrand_int32() % 100;
			this->timeUntilHooked = 0;
		} else {
			this->motionY -= 0.03f * this->random.nextFloat();
			if (this->random.genrand_int32() % 4 == 0) {
				this->level->addParticle(PT_BUBBLE, this->posX + (this->random.nextFloat() - 0.5f) * 0.4f, this->posY + 0.1f, this->posZ + (this->random.nextFloat() - 0.5f) * 0.4f, 0.0f, 0.08f, 0.0f, 0);
			}
		}
	} else if (this->timeUntilHooked > 0) {
		--this->timeUntilHooked;
		float prog = (float)this->timeUntilHooked / 35.0f;
		float bDist = prog * 4.2f;
		float bAngle = (float)this->entityId * 2.399f;
		float bX = this->posX + sinf(bAngle) * bDist;
		float bZ = this->posZ + cosf(bAngle) * bDist;
		float bY = this->posY + 0.05f;
		for (int i = 0; i < 3; ++i) {
			float rX = bX + (this->random.nextFloat() - 0.5f) * 0.15f;
			float rZ = bZ + (this->random.nextFloat() - 0.5f) * 0.15f;
			this->level->addParticle(PT_BUBBLE, rX, bY, rZ, -sinf(bAngle) * 0.08f, 0.08f, -cosf(bAngle) * 0.08f, 0);
		}
		if (this->timeUntilHooked == 0) {
			this->motionY = -0.45f;
			this->level->playSound(this, "random.splash", 1.2f, 0.9f + (this->random.nextFloat() - this->random.nextFloat()) * 0.2f);
			for (int p = 0; p < 20; ++p) {
				float ox = (this->random.nextFloat() - 0.5f) * 0.5f;
				float oz = (this->random.nextFloat() - 0.5f) * 0.5f;
				this->level->addParticle(PT_BUBBLE, this->posX + ox, this->posY + 0.1f, this->posZ + oz, ox * 0.25f, 0.18f, oz * 0.25f, 0);
			}
			this->nibble = 45 + this->random.genrand_int32() % 30;
		}
	} else if (this->timeUntilLured > 0) {
		--this->timeUntilLured;
		if (this->timeUntilLured <= 0) {
			this->timeUntilHooked = 35;
		}
	} else {
		this->timeUntilLured = 60 + this->random.genrand_int32() % 100;
	}
}

void FishingHook::tick() {
	Entity::tick();

	if (!this->owner || !this->owner->isAlive() || this->distanceToSqr(this->owner) > 1024.0f) {
		this->remove();
		return;
	}

	if (this->inGround) {
		++this->life;
		if (this->life >= 1200) {
			this->remove();
			return;
		}
	}

	bool_t inWater = this->isInWater();

	if (inWater) {
		this->motionX *= 0.85f;
		this->motionZ *= 0.85f;
		this->motionY = (this->motionY + 0.02f) * 0.8f;
		this->catchingFishLogic();
	} else {
		this->motionY -= 0.04f;
	}

	this->move(this->motionX, this->motionY, this->motionZ);

	this->motionX *= 0.92f;
	this->motionY *= 0.92f;
	this->motionZ *= 0.92f;
}

int32_t FishingHook::retrieve(ItemInstance* item) {
	if (!this->level || !this->owner) {
		return 0;
	}

	int32_t damage = 0;

	if (this->nibble > 0) {
		int32_t roll = this->random.genrand_int32() % 100;
		Item* caughtItem = Item::fish_raw;
		if (roll < 60) {
			caughtItem = Item::fish_raw ? Item::fish_raw : Item::items[349];
		} else if (roll < 85) {
			caughtItem = Item::salmon_raw ? Item::salmon_raw : Item::fish_raw;
		} else if (roll < 97) {
			caughtItem = Item::clownfish ? Item::clownfish : Item::fish_raw;
		} else {
			caughtItem = Item::pufferfish ? Item::pufferfish : Item::fish_raw;
		}

		if (caughtItem) {
			ItemInstance resultStack(caughtItem, 1, 0);
			ItemEntity* entityItem = new ItemEntity(this->level, this->posX, this->posY, this->posZ, resultStack);
			float dx = this->owner->posX - this->posX;
			float dy = this->owner->posY - this->posY;
			float dz = this->owner->posZ - this->posZ;
			float dist = sqrtf(dx * dx + dy * dy + dz * dz);
			entityItem->motionX = dx * 0.12f;
			entityItem->motionY = dy * 0.12f + sqrtf(dist) * 0.10f;
			entityItem->motionZ = dz * 0.12f;
			this->level->addEntity(entityItem);
			this->level->playSound(this->owner, "random.splash", 0.9f, 1.2f);
		}
		damage = 1;
	}

	if (this->inGround) {
		damage = 2;
	}

	this->remove();
	return damage;
}
