#pragma once
#include <entity/Zombie.hpp>

struct ZombieVillager : Zombie {
	ZombieVillager(Level*);

	virtual ~ZombieVillager();
	virtual int32_t getEntityTypeId() const;
};
