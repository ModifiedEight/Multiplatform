#include <entity/Squid.hpp>
#include <level/Level.hpp>
#include <tile/material/Material.hpp>
#include <item/Item.hpp>
#include <math/Mth.hpp>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Squid::Squid(Level* level)
	: WaterAnimal(level) {
	this->setSize(0.95f, 0.95f);
	this->xBodyRot = 0.0f;
	this->xBodyRotO = 0.0f;
	this->zBodyRot = 0.0f;
	this->zBodyRotO = 0.0f;
	this->tentacleMovement = 0.0f;
	this->oldTentacleMovement = 0.0f;
	this->tentacleAngle = 0.0f;
	this->oldTentacleAngle = 0.0f;
	this->rotateSpeed = 0.0f;
	this->tx = 0.0f;
	this->ty = 0.0f;
	this->tz = 0.0f;
	this->tentacleSpeed = 1.0f / (this->random.nextFloat() + 1.0f) * 0.2f;
	this->entityRenderId = SQUID;
}

Squid::~Squid() {
}

int32_t Squid::getMaxHealth() {
	return 10;
}

int32_t Squid::getEntityTypeId() const {
	return 17;
}

static std::string _squidTex = "mob/squid.png";
std::string* Squid::getTexture() {
	return &_squidTex;
}

int32_t Squid::getAmbientSoundInterval() {
	return 120;
}

const char_t* Squid::getAmbientSound() {
	return "mob.squid.ambient";
}

std::string Squid::getHurtSound() {
	return "mob.squid.hurt";
}

std::string Squid::getDeathSound() {
	return "mob.squid.death";
}

void Squid::aiStep() {
	WaterAnimal::aiStep();

	this->xBodyRotO = this->xBodyRot;
	this->zBodyRotO = this->zBodyRot;
	this->oldTentacleMovement = this->tentacleMovement;
	this->oldTentacleAngle = this->tentacleAngle;
	this->tentacleMovement += this->tentacleSpeed;

	if (this->tentacleMovement > (float)(M_PI * 2.0)) {
		this->tentacleMovement -= (float)(M_PI * 2.0);
		if (this->random.genrand_int32() % 10 == 0) {
			this->tentacleSpeed = 1.0f / (this->random.nextFloat() + 1.0f) * 0.2f;
		}
	}

	if (this->isInWater()) {
		if (this->tentacleMovement < (float)M_PI) {
			float f = this->tentacleMovement / (float)M_PI;
			this->tentacleAngle = Mth::sin(f * f * (float)M_PI) * (float)M_PI * 0.25f;
			if ((double)f > 0.75) {
				this->speed = 1.0f;
				this->rotateSpeed = 1.0f;
			} else {
				this->rotateSpeed *= 0.8f;
			}
		} else {
			this->tentacleAngle = 0.0f;
			this->speed *= 0.9f;
			this->rotateSpeed *= 0.99f;
		}

		if (this->tx * this->tx + this->ty * this->ty + this->tz * this->tz < 0.0001f || (this->random.genrand_int32() % 50 == 0)) {
			float angle = this->random.nextFloat() * (float)M_PI * 2.0f;
			this->tx = Mth::cos(angle) * 0.12f;
			this->ty = -0.03f + this->random.nextFloat() * 0.06f;
			this->tz = Mth::sin(angle) * 0.12f;
		}

		int bx = (int)std::floor(this->posX);
		int by = (int)std::floor(this->posY + 0.5f);
		int bz = (int)std::floor(this->posZ);
		bool waterAbove = this->level && this->level->getMaterial(bx, by + 1, bz)->isLiquid();

		this->motionX = this->tx * this->speed;
		this->motionY = this->ty * this->speed;
		this->motionZ = this->tz * this->speed;

		if (!waterAbove && this->motionY > 0.0f) {
			this->motionY = -0.02f;
		}

		float horizontalDist = std::sqrt(this->motionX * this->motionX + this->motionZ * this->motionZ);
		this->yaw += (-(float)(std::atan2(this->motionX, this->motionZ) * 180.0 / M_PI) - this->yaw) * 0.1f;
		this->zBodyRot += (float)((double)(float)M_PI * (double)this->rotateSpeed * 1.5 - (double)this->zBodyRot) * 0.1f;
		this->xBodyRot += ((-(float)(std::atan2(horizontalDist, this->motionY) * 180.0 / M_PI) + 90.0f) - this->xBodyRot) * 0.1f;
		this->isJumping = 0;
	} else {
		this->tentacleAngle = std::abs(Mth::sin(this->tentacleMovement)) * (float)M_PI * 0.25f;
		if (this->onGround) {
			if (this->random.nextFloat() < 0.05f) {
				this->motionX = (this->random.nextFloat() - 0.5f) * 0.2f;
				this->motionZ = (this->random.nextFloat() - 0.5f) * 0.2f;
				this->motionY = 0.2f;
			}
		}
		this->motionY = (this->motionY - 0.08f) * 0.98f;
		this->xBodyRot += (-90.0f - this->xBodyRot) * 0.02f;

		if (--this->air <= -20) {
			this->air = 0;
			this->hurt(0, 1);
		}
	}
	if (this->isInWater()) {
		this->air = 300;
	}
}

void Squid::travel(float a2, float a3) {
	this->move(this->motionX, this->motionY, this->motionZ);
}

bool_t Squid::hurt(Entity* source, int32_t damage) {
	bool_t result = WaterAnimal::hurt(source, damage);
	if (this->health > 0) {
		this->tx = -this->motionX * 2.0f;
		this->ty = 0.2f;
		this->tz = -this->motionZ * 2.0f;
	}
	return result;
}

void Squid::die(Entity* source) {
	WaterAnimal::die(source);
}

void Squid::dropDeathLoot() {
	int32_t count = this->random.genrand_int32() % 3 + 1;
	for (int32_t i = 0; i < count; ++i) {
		this->spawnAtLocation(Item::dye_powder->itemID, 1, 0);
	}
}
