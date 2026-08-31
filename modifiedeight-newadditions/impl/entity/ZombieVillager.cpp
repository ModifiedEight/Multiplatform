#include <entity/ZombieVillager.hpp>
#include <entity/ai/goals/BreakDoorGoal.hpp>
#include <entity/ai/goals/FloatGoal.hpp>
#include <entity/ai/goals/HurtByTargetGoal.hpp>
#include <entity/ai/goals/LookAtPlayerGoal.hpp>
#include <entity/ai/goals/MeleeAttackGoal.hpp>
#include <entity/ai/goals/NearestAttackableTargetGoal.hpp>
#include <entity/ai/goals/RandomLookAroundGoal.hpp>
#include <entity/ai/goals/RandomStrollGoal.hpp>
#include <level/Level.hpp>

ZombieVillager::ZombieVillager(Level* level)
	: Zombie(level) {
	this->entityRenderId = ZOMBIE_VILLAGER;
	this->skin = "mob/zombie_villager.png";
}

ZombieVillager::~ZombieVillager() {
}

int32_t ZombieVillager::getEntityTypeId() const {
	return 121;
}
