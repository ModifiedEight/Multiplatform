#include <entity/Monster.hpp>
#include <entity/Player.hpp>
#include <level/Level.hpp>
#include <level/LightLayer.hpp>
#include <cmath>

Monster::Monster(Level* a2)
	: PathfinderMob(a2) {
	this->attackDamage = 2;
	this->lastHurtByMaybe = 0;
	this->entityRenderId = HUMANOID;
}
bool_t Monster::isDarkEnoughToSpawn() {
	int32_t x = (int32_t)floorf(this->posX);
	int32_t y = (int32_t)floorf(this->boundingBox.minY);
	int32_t z = (int32_t)floorf(this->posZ);

	int32_t rb = this->level->getRawBrightness(x, y, z);
	if(rb > 7) {
		return 0;
	}
	int32_t skyLight = this->level->getBrightness(LightLayer::Sky, x, y, z);
	if(skyLight > 0 && skyLight > (int32_t)(this->random.genrand_int32() & 0x1F)) {
		return 0;
	}
	return 1;
}

Monster::~Monster() {
}
void Monster::tick() {
	Mob::tick();
}
bool_t Monster::hurt(Entity* a2, int32_t a3) {
	int32_t r; // r0

	r = Mob::hurt(a2, a3);
	if(r) {
		if(this->rider != a2 && this->ridingAt != a2 && a2 != this) {
			this->attackTarget = 0;
			if(a2) {
				this->attackTarget = a2->entityId;
				if(a2->isMob()) {
					this->lastHurtByMaybe = a2->entityId;
				}
			}
		}
		return 1;
	}
	return 0;
}

int32_t Monster::getCreatureBaseType() {
	return 1;
}
void Monster::aiStep() {
	this->updateAttackAnim();
	if(this->getBrightness(1) > 0.5) {
		this->noActionTime += 2;
	}
	Mob::aiStep();
}
bool_t Monster::canSpawn() {
	if(this->isDarkEnoughToSpawn()) {
		return PathfinderMob::canSpawn();
	}
	return 0;
}
bool_t Monster::doHurtTarget(Entity* a2) {
	this->swing();
	return a2->hurt(this, this->attackDamage);
}
float Monster::getWalkTargetValue(int32_t x, int32_t y, int32_t z) {
	int tCur = this->level->getTile(x, y, z);
	int tBelow = this->level->getTile(x, y - 1, z);
	if (tCur == 8 || tCur == 9 || tBelow == 8 || tBelow == 9) {
		return -100.0f;
	}
	return 0.5f - ((float)this->level->getRawBrightness(x, y, z) / 15.0f);
}
Entity* Monster::findAttackTarget() {
	Player* np = this->level->getNearestPlayer(this, 16.0);
	if(!np || np->abilities.instabuild || np->abilities.invulnerable || !this->canSee(np)) {
		return 0;
	}
	return np;
}
void Monster::checkHurtTarget(Entity* a2, float a3) {
	int32_t v5; // r0

	if(this->attackTime <= 0 && a3 < 2.0 && a2->boundingBox.maxY > this->boundingBox.minY && a2->boundingBox.minY < this->boundingBox.maxY) {
		v5 = this->getAttackTime();
		this->attackTime = v5;
		this->doHurtTarget(a2);
	}
}
int32_t Monster::getAttackDamage(Entity*) {
	return this->attackDamage;
}
int32_t Monster::getAttackTime() {
	return 20;
}
