#pragma once
#include <rendering/model/Model.hpp>
#include <rendering/model/ModelPart.hpp>

struct VillagerModel : Model {
	ModelPart head;
	ModelPart hat;
	ModelPart hatRim;
	ModelPart body;
	ModelPart jacket;
	ModelPart arms;
	ModelPart leg0;
	ModelPart leg1;
	ModelPart nose;

	VillagerModel(float f = 0.0f);
	virtual ~VillagerModel();
	virtual void render(Entity*, float, float, float, float, float, float);
	virtual void setupAnim(float, float, float, float, float, float);
};
