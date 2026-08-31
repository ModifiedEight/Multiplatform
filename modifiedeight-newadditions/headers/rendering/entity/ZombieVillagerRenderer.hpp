#pragma once
#include <rendering/entity/MobRenderer.hpp>
#include <rendering/model/ZombieVillagerModel.hpp>

struct ZombieVillagerRenderer : MobRenderer {
	ZombieVillagerRenderer(ZombieVillagerModel*, float);
	virtual ~ZombieVillagerRenderer();
};
