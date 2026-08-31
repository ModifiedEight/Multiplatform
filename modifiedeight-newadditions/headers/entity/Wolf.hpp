#pragma once
#include <entity/Animal.hpp>

struct Wolf: Animal {
	int32_t wolfType;
	bool_t isAngry;
	bool_t isTame;
	bool_t isSitting;

	Wolf(Level*);
	virtual ~Wolf();
	virtual int32_t getEntityTypeId() const;
	virtual std::string* getTexture();
	virtual int32_t getMaxHealth();
	virtual float getBaseSpeed();
	virtual int32_t getAmbientSoundInterval();
	virtual const char_t* getAmbientSound();
	virtual std::string getHurtSound();
	virtual std::string getDeathSound();
	virtual void aiStep();
	virtual bool_t canSpawn();
	virtual Mob* getBreedOffspring(Animal*);
};
