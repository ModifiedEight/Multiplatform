#pragma once
#include <rendering/entity/MobRenderer.hpp>
#include <rendering/model/VillagerModel.hpp>

struct VillagerRenderer : MobRenderer {
	VillagerRenderer(VillagerModel*, float);
	virtual ~VillagerRenderer();
	virtual void additionalRendering(Mob*, float);
};

