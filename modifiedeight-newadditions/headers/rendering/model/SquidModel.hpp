#pragma once
#include <rendering/model/Model.hpp>
#include <rendering/model/ModelPart.hpp>

struct SquidModel: Model {
	ModelPart body;
	ModelPart* tentacles[8];

	SquidModel();
	virtual ~SquidModel();
	virtual void render(Entity*, float, float, float, float, float, float);
	virtual void setupAnim(float, float, float, float, float, float);
};
