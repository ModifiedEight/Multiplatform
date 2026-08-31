#pragma once
#include <entity/WaterAnimal.hpp>

struct AbstractFish: WaterAnimal {
	float fishAngle;
	float targetAngle;
	float swimSpeed;
	int32_t leaderEntityId;

	AbstractFish(Level*);
	virtual ~AbstractFish();

	virtual int32_t getMaxHealth();
	virtual int32_t getAmbientSoundInterval();
	virtual const char_t* getAmbientSound();
	virtual std::string getHurtSound();
	virtual std::string getDeathSound();
	virtual int32_t getDeathLoot();
	virtual void dropDeathLoot();
	virtual void aiStep();
	virtual void travel(float, float);
	void startFollowing(AbstractFish*);
};
