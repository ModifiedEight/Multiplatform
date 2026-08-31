#pragma once
#include <entity/AbstractFish.hpp>

struct Pufferfish: AbstractFish {
	int32_t puffState;
	int32_t inflateCounter;
	int32_t deflateTimer;

	Pufferfish(Level*);
	virtual ~Pufferfish();
	virtual int32_t getEntityTypeId() const;
	virtual std::string* getTexture();
	virtual void aiStep();
	virtual int32_t getDeathLoot();
};
