#pragma once
#include <entity/AbstractFish.hpp>

struct Cod: AbstractFish {
	Cod(Level*);
	virtual ~Cod();
	virtual int32_t getEntityTypeId() const;
	virtual std::string* getTexture();
	virtual int32_t getDeathLoot();
};
