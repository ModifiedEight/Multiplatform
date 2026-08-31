#pragma once
#include <rendering/model/Model.hpp>
#include <rendering/model/ModelPart.hpp>

struct FoxModel: Model {
	ModelPart head;
	ModelPart earL;
	ModelPart earR;
	ModelPart nose;
	ModelPart body;
	ModelPart leg0;
	ModelPart leg1;
	ModelPart leg2;
	ModelPart leg3;
	ModelPart tail;

	FoxModel();
	virtual ~FoxModel();
	virtual void render(Entity*, float, float, float, float, float, float);
	virtual void setupAnim(float, float, float, float, float, float);
};
