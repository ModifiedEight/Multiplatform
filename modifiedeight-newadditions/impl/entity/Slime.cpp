#include <entity/Slime.hpp>
#include <entity/Player.hpp>
#include <level/Level.hpp>
#include <item/Item.hpp>
#include <math/Mth.hpp>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Slime::Slime(Level* level)
	: Monster(level) {
	this->entityRenderId = SLIME;
	int32_t r = this->random.genrand_int32() % 3;
	this->slimeSize = (r == 0) ? 1 : ((r == 1) ? 2 : 4);
	this->setSlimeSize(this->slimeSize);
	this->squish = 0.0f;
	this->oSquish = 0.0f;
	this->targetSquish = 0.0f;
	this->jumpDelay = this->random.genrand_int32() % 20 + 10;
}

Slime::~Slime() {
}

void Slime::setSlimeSize(int32_t size) {
	this->slimeSize = size;
	this->setSize(0.6f * (float)size, 0.6f * (float)size);
	this->health = size * size;
	this->attackDamage = (size > 1) ? size : 0;
}

void Slime::playerTouch(Player* player) {
	if (this->slimeSize > 1 && player && !player->abilities.invulnerable && !player->abilities.instabuild) {
		float reach = 0.6f * (float)this->slimeSize;
		float dx = player->posX - this->posX;
		float dz = player->posZ - this->posZ;
		float dy = player->posY - this->posY;
		if (dx * dx + dz * dz < (reach + 0.6f) * (reach + 0.6f) && fabsf(dy) < (reach + 1.2f)) {
			if (this->attackTime <= 0) {
				this->attackTime = 20;
				if (player->hurt(this, this->slimeSize)) {
					this->level->playSound(this, "mob.slime.big", 1.0f, ((this->random.nextFloat() - this->random.nextFloat()) * 0.2f + 1.0f) * 0.8f);
				}
			}
		}
	}
}

void Slime::checkHurtTarget(Entity* a2, float a3) {
	if (this->slimeSize > 1 && a2) {
		float reach = 0.6f * (float)this->slimeSize + 0.5f;
		if (this->attackTime <= 0 && a3 < reach && a2->boundingBox.maxY > this->boundingBox.minY && a2->boundingBox.minY < this->boundingBox.maxY) {
			this->attackTime = this->getAttackTime();
			this->doHurtTarget(a2);
		}
	}
}

int32_t Slime::getEntityTypeId() const {
	return 37;
}

static std::string _slimeTex = "mob/slime.png";
std::string* Slime::getTexture() {
	return &_slimeTex;
}

int32_t Slime::getMaxHealth() {
	return this->slimeSize * this->slimeSize;
}

float Slime::getBaseSpeed() {
	return 0.3f;
}

void Slime::aiStep() {
	Monster::aiStep();

	this->oSquish = this->squish;
	this->squish += (this->targetSquish - this->squish) * 0.5f;

	if (this->onGround) {
		if (this->targetSquish != 0.0f) {
			this->targetSquish = 0.0f;
		}

		if (--this->jumpDelay <= 0) {
			this->jumpDelay = this->random.genrand_int32() % 20 + 10;
			this->targetSquish = -0.5f;

			Player* target = this->level->getNearestPlayer(this->posX, this->posY, this->posZ, 16.0f);
			if (target && !target->abilities.instabuild && !target->abilities.invulnerable) {
				float dx = target->posX - this->posX;
				float dz = target->posZ - this->posZ;
				this->yaw = (float)(atan2f(dz, dx) * 180.0 / M_PI) - 90.0f;
			} else {
				this->yaw += (this->random.nextFloat() - 0.5f) * 90.0f;
			}

			float rad = this->yaw * (float)M_PI / 180.0f;
			float jumpSpeed = 0.2f + 0.1f * this->slimeSize;
			this->motionX = -Mth::sin(rad) * jumpSpeed;
			this->motionZ = Mth::cos(rad) * jumpSpeed;
			this->motionY = 0.42f;
			const char* sname = this->slimeSize > 1 ? "mob.slime.big" : "mob.slime.small";
			this->level->playSound(this, sname, 1.0f, ((this->random.nextFloat() - this->random.nextFloat()) * 0.2f + 1.0f) * 0.8f);
		} else {
			this->motionX = 0.0f;
			this->motionZ = 0.0f;
		}
	} else {
		this->targetSquish = 1.0f;
	}
}

std::string Slime::getHurtSound() {
	return this->slimeSize > 1 ? "mob.slime.big" : "mob.slime.small";
}

std::string Slime::getDeathSound() {
	return this->slimeSize > 1 ? "mob.slime.big" : "mob.slime.small";
}

bool_t Slime::canSpawn() {
	int32_t x = (int32_t)this->posX;
	int32_t y = (int32_t)this->posY;
	int32_t z = (int32_t)this->posZ;
	if (x < 0) --x;
	if (z < 0) --z;
	return this->level->isSolidBlockingTile(x, y - 1, z) && !this->level->isSolidBlockingTile(x, y, z);
}

int32_t Slime::getMaxSpawnClusterSize() {
	return 4;
}

void Slime::die(Entity* cause) {
	if (this->slimeSize > 1 && this->level) {
		int newSize = this->slimeSize / 2;
		int count = 2 + (this->random.genrand_int32() % 3);
		for (int i = 0; i < count; ++i) {
			float ox = ((float)(i % 2) - 0.5f) * (float)newSize * 0.5f;
			float oz = ((float)(i / 2) - 0.5f) * (float)newSize * 0.5f;
			Slime* child = new Slime(this->level);
			child->setSlimeSize(newSize);
			child->moveTo(this->posX + ox, this->posY + 0.5f, this->posZ + oz, this->random.nextFloat() * 360.0f, 0.0f);
			this->level->addEntity(child);
		}
	}
	Monster::die(cause);
}

void Slime::dropDeathLoot() {
	if (this->slimeSize <= 1 && Item::slimeBall) {
		int count = (this->random.genrand_int32() % 3);
		if (count > 0) {
			this->spawnAtLocation(Item::slimeBall->itemID, count, 0);
		}
	}
}
