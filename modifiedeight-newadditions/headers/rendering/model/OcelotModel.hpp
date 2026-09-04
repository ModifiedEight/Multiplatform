#pragma once
#include <rendering/model/Model.hpp>
#include <rendering/model/ModelPart.hpp>

struct OcelotModel: Model {
	ModelPart head;
	ModelPart body;
	ModelPart leg0;
	ModelPart leg1;
	ModelPart leg2;
	ModelPart leg3;
	ModelPart tail1;
	ModelPart tail2;

	OcelotModel();
	virtual ~OcelotModel();
	virtual void render(Entity*, float, float, float, float, float, float);
	virtual void setupAnim(float, float, float, float, float, float);
};
