#include <entity/ai/goals/NearestAttackableTargetGoal.hpp>
#include <entity/Player.hpp>
#include <level/Level.hpp>
NearestAttackableTargetGoal::NearestAttackableTargetGoal(Mob* mob, float radius)
	: TargetGoal(mob, radius, 1) {
	this->field_18 = 1;
	this->field_1C = 0;
	this->setRequiredControlFlags(1);
}
NearestAttackableTargetGoal::~NearestAttackableTargetGoal(){
}
bool_t NearestAttackableTargetGoal::canUse() {
	int32_t v2;			   // r5
	Player* nearestPlayer; // r5

	v2 = this->field_1C;
	if(v2 > 0 && this->mob->random.genrand_int32() % v2) {
		return 0;
	}
	if(this->field_18 == 1) {
		nearestPlayer = this->mob->level->getNearestPlayer(this->mob, this->radius);
	} else {
		nearestPlayer = 0;
	}

	if(this->canAttack(nearestPlayer, 0)) {
		this->targetedPlayer = nearestPlayer;
		return 1;
	}

	if(this->mob && this->mob->level && (this->mob->getEntityTypeId() == 32 || this->mob->getEntityTypeId() == 121)) {
		Mob* nearestVillager = 0;
		float nearestDistSq = this->radius * this->radius;
		for(auto ent : this->mob->level->entities) {
			if(ent && ent->isMob() && ent->getEntityTypeId() == 120) {
				float dx = ent->posX - this->mob->posX;
				float dy = ent->posY - this->mob->posY;
				float dz = ent->posZ - this->mob->posZ;
				float distSq = dx * dx + dy * dy + dz * dz;
				if(distSq < nearestDistSq && this->canAttack((Mob*)ent, 0)) {
					nearestDistSq = distSq;
					nearestVillager = (Mob*)ent;
				}
			}
		}
		if(nearestVillager) {
			this->targetedPlayer = (Player*)nearestVillager;
			return 1;
		}
	}
	return 0;
}
void NearestAttackableTargetGoal::start() {
	this->mob->setTarget(this->targetedPlayer);
}
