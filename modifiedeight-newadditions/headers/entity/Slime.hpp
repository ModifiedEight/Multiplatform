#pragma once
#include <entity/Monster.hpp>

struct Slime: Monster {
	int32_t slimeSize;
	float targetSquish;
	float squish;
	float oSquish;
	int32_t jumpDelay;

	Slime(Level*);
	virtual ~Slime();
	virtual int32_t getEntityTypeId() const;
	virtual std::string* getTexture();
	virtual int32_t getMaxHealth();
	virtual float getBaseSpeed();
	virtual void aiStep();
	virtual bool_t canSpawn();
	virtual int32_t getMaxSpawnClusterSize();
	virtual void die(Entity*);
	virtual void dropDeathLoot();
	virtual std::string getHurtSound();
	virtual std::string getDeathSound();
	virtual void playerTouch(Player*);
	virtual void checkHurtTarget(Entity*, float);
	void setSlimeSize(int32_t);
};
