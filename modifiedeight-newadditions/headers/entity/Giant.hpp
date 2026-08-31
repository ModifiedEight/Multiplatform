#pragma once
#include <entity/Monster.hpp>

struct Giant: Monster {
	Giant(Level*);
	virtual ~Giant();
	virtual int32_t getEntityTypeId() const;
	virtual std::string* getTexture();
	virtual int32_t getMaxHealth();
	virtual float getBaseSpeed();
	virtual int32_t getAttackDamage(Entity*);
	virtual const char_t* getAmbientSound();
	virtual std::string getHurtSound();
	virtual std::string getDeathSound();
	virtual void aiStep();
};
