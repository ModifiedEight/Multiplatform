#pragma once
#include <rendering/model/Model.hpp>
#include <rendering/model/ModelPart.hpp>

struct PolarBearModel: Model {
	ModelPart head;
	ModelPart body;
	ModelPart leg0;
	ModelPart leg1;
	ModelPart leg2;
	ModelPart leg3;

	PolarBearModel();
	virtual ~PolarBearModel();
	virtual void render(Entity*, float, float, float, float, float, float);
	virtual void setupAnim(float, float, float, float, float, float);
};
