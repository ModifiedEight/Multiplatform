#pragma once
#include <entity/AbstractFish.hpp>

struct Salmon: AbstractFish {
	Salmon(Level*);
	virtual ~Salmon();
	virtual int32_t getEntityTypeId() const;
	virtual std::string* getTexture();
	virtual int32_t getDeathLoot();
};
