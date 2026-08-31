#pragma once
#include <rendering/model/Model.hpp>
#include <rendering/model/ModelPart.hpp>

struct SalmonModel: Model {
	ModelPart bodyFront;
	ModelPart head;
	ModelPart bodyBack;
	ModelPart tail;
	ModelPart finTop;
	ModelPart finTopBack;
	ModelPart finSide0;
	ModelPart finSide1;

	SalmonModel();
	virtual ~SalmonModel();
	virtual void render(Entity*, float, float, float, float, float, float);
	virtual void setupAnim(float, float, float, float, float, float);
};
