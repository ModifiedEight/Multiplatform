#pragma once
#include <entity/Animal.hpp>

struct Frog: Animal {
	Frog(Level*);
	virtual ~Frog();
	virtual int32_t getEntityTypeId() const;
	virtual std::string* getTexture();
	virtual int32_t getMaxHealth();
	virtual float getBaseSpeed();
	virtual const char_t* getAmbientSound();
	virtual std::string getHurtSound();
	virtual std::string getDeathSound();
	virtual int32_t getAmbientSoundInterval();
	virtual bool_t canSpawn();
	virtual Mob* getBreedOffspring(Animal*);
};
