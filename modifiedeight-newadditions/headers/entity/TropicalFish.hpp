#pragma once
#include <entity/AbstractFish.hpp>

struct TropicalFish: AbstractFish {
	int32_t variant;

	TropicalFish(Level*);
	virtual ~TropicalFish();
	virtual int32_t getEntityTypeId() const;
	virtual std::string* getTexture();
	virtual int32_t getDeathLoot();
};
