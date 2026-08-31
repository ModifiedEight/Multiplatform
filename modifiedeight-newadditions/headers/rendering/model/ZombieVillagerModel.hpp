#pragma once
#include <rendering/model/HumanoidModel.hpp>

struct ZombieVillagerModel : HumanoidModel {
	ModelPart hatRim;
	ModelPart nose;

	ZombieVillagerModel(float f = 0.0f);
	virtual ~ZombieVillagerModel();
	virtual void setupAnim(float, float, float, float, float, float);
	virtual void render(Entity*, float, float, float, float, float, float);
};
