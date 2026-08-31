#include <entity/AbstractFish.hpp>
#include <level/Level.hpp>
#include <tile/material/Material.hpp>
#include <math/Mth.hpp>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

AbstractFish::AbstractFish(Level* level)
	: WaterAnimal(level) {
	this->setSize(0.5f, 0.4f);
	this->fishAngle = 0.0f;
	this->targetAngle = 0.0f;
	this->swimSpeed = 0.04f;
	this->leaderEntityId = 0;
}

AbstractFish::~AbstractFish() {
}

int32_t AbstractFish::getMaxHealth() {
	return 3;
}

int32_t AbstractFish::getAmbientSoundInterval() {
	return 120;
}

const char_t* AbstractFish::getAmbientSound() {
	return "mob.fish.swim";
}

std::string AbstractFish::getHurtSound() {
	return "mob.fish.hurt";
}

std::string AbstractFish::getDeathSound() {
	return "mob.fish.hurt";
}

int32_t AbstractFish::getDeathLoot() {
	return 0;
}

void AbstractFish::dropDeathLoot() {
	int32_t loot = this->getDeathLoot();
	if (loot > 0) {
		this->spawnAtLocation(loot, 1, 0);
	}
}

void AbstractFish::startFollowing(AbstractFish* target) {
	this->leaderEntityId = target ? target->entityId : 0;
}

void AbstractFish::aiStep() {
	try {
		WaterAnimal::aiStep();

		if (this->isInWater()) {
			int bx = (int)floor(this->posX);
			int by = (int)floor(this->posY + 0.3f);
			int bz = (int)floor(this->posZ);

			int tileAbove = (this->level && by + 1 < 128) ? this->level->getTile(bx, by + 1, bz) : 0;
			int tileBelow = (this->level && by - 1 >= 0) ? this->level->getTile(bx, by - 1, bz) : 0;
			bool waterAbove = (tileAbove == 8 || tileAbove == 9);
			bool waterBelow = (tileBelow == 8 || tileBelow == 9);

			Entity* leaderEnt = (this->leaderEntityId != 0 && this->level) ? this->level->getEntity(this->leaderEntityId) : nullptr;
			if (leaderEnt && leaderEnt->isAlive()) {
				float dx = leaderEnt->posX - this->posX;
				float dz = leaderEnt->posZ - this->posZ;
				float dsq = dx * dx + dz * dz;
				if (dsq > 2.25f) {
					this->targetAngle = (float)(atan2f(dz, dx) * 180.0 / M_PI) - 90.0f;
					this->swimSpeed = ((AbstractFish*)leaderEnt)->swimSpeed;
				} else {
					this->targetAngle = leaderEnt->yaw;
					this->swimSpeed = ((AbstractFish*)leaderEnt)->swimSpeed;
				}
			} else {
				this->leaderEntityId = 0;
				if (this->random.genrand_int32() % 60 == 0) {
					this->targetAngle = (this->random.nextFloat() * 360.0f - 180.0f);
					this->swimSpeed = 0.03f + this->random.nextFloat() * 0.03f;
				}
			}

			float forwardRad = this->targetAngle * (float)M_PI / 180.0f;
			int fwdX = (int)floor(this->posX - Mth::sin(forwardRad) * 1.5f);
			int fwdY = (int)floor(this->posY);
			int fwdZ = (int)floor(this->posZ + Mth::cos(forwardRad) * 1.5f);
			int fwdTile = (this->level && fwdY >= 0 && fwdY < 128) ? this->level->getTile(fwdX, fwdY, fwdZ) : 0;
			if (fwdTile != 8 && fwdTile != 9) {
				this->targetAngle += 180.0f + (this->random.nextFloat() * 60.0f - 30.0f);
				this->swimSpeed = 0.04f;
			}

			float diff = this->targetAngle - this->yaw;
			while (diff < -180.0f) diff += 360.0f;
			while (diff >= 180.0f) diff -= 360.0f;
			this->yaw += diff * 0.12f;

			float rad = this->yaw * (float)M_PI / 180.0f;
			this->motionX = -Mth::sin(rad) * this->swimSpeed;
			this->motionZ = Mth::cos(rad) * this->swimSpeed;

			if (!waterAbove) {
				this->motionY = -0.02f;
			} else if (!waterBelow) {
				this->motionY = 0.02f;
			} else if (this->random.genrand_int32() % 25 == 0) {
				this->motionY = (this->random.nextFloat() * 0.02f - 0.01f);
			}
			this->motionY *= 0.85f;
			this->isJumping = 0;
		} else {
			if (this->onGround) {
				this->motionY = 0.22f;
				float bestDistSq = 9999.0f;
				float targetDx = 0.0f, targetDz = 0.0f;
				int bx = (int)floor(this->posX);
				int by = (int)floor(this->posY);
				int bz = (int)floor(this->posZ);
				if (this->level) {
					for (int ox = -3; ox <= 3; ++ox) {
						for (int oz = -3; oz <= 3; ++oz) {
							for (int oy = -1; oy <= 1; ++oy) {
								int t = this->level->getTile(bx + ox, by + oy, bz + oz);
								if (t == 8 || t == 9) {
									float distSq = (float)(ox * ox + oz * oz);
									if (distSq < bestDistSq) {
										bestDistSq = distSq;
										targetDx = (float)ox;
										targetDz = (float)oz;
									}
								}
							}
						}
					}
				}
				if (bestDistSq < 9000.0f && bestDistSq > 0.01f) {
					float len = sqrtf(targetDx * targetDx + targetDz * targetDz);
					this->motionX = (targetDx / len) * 0.18f;
					this->motionZ = (targetDz / len) * 0.18f;
					this->yaw = (float)(atan2f(targetDz, targetDx) * 180.0 / M_PI) - 90.0f;
				} else {
					this->motionX = (this->random.nextFloat() * 2.0f - 1.0f) * 0.15f;
					this->motionZ = (this->random.nextFloat() * 2.0f - 1.0f) * 0.15f;
					this->yaw = this->random.nextFloat() * 360.0f;
				}
				if (this->level) {
					this->level->playSound(this->posX, this->posY, this->posZ, "mob.fish.flop", 0.4f, 1.0f + (this->random.nextFloat() - this->random.nextFloat()) * 0.2f);
				}
			}
			this->motionY = (this->motionY - 0.08f) * 0.98f;

			if (--this->air <= -20) {
				this->air = 0;
				this->hurt(0, 1);
			}
		}
		if (this->isInWater()) {
			this->air = 300;
		}
	} catch (...) {
	}
}

void AbstractFish::travel(float a2, float a3) {
	this->move(this->motionX, this->motionY, this->motionZ);
}
