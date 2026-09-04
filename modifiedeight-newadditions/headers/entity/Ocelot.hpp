#pragma once
#include <entity/Animal.hpp>

struct Ocelot: Animal {
	bool_t isTrusting;
	bool_t isSitting;

	Ocelot(Level*);
	virtual ~Ocelot();
	virtual int32_t getEntityTypeId() const;
	virtual std::string* getTexture();
	virtual int32_t getMaxHealth();
	virtual float getBaseSpeed();
	virtual int32_t getAmbientSoundInterval();
	virtual const char_t* getAmbientSound();
	virtual std::string getHurtSound();
	virtual std::string getDeathSound();
	virtual void aiStep();
	virtual bool_t interactWithPlayer(Player*);
	virtual bool_t canSpawn();
	virtual Mob* getBreedOffspring(Animal*);
};
