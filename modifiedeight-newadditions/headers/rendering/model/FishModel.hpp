#pragma once
#include <rendering/model/Model.hpp>
#include <rendering/model/ModelPart.hpp>

struct FishModel: Model {
	ModelPart body;
	ModelPart tail;
	ModelPart finRight;
	ModelPart finLeft;
	ModelPart finTop;

	FishModel();
	virtual ~FishModel();
	virtual void render(Entity*, float, float, float, float, float, float);
	virtual void setupAnim(float, float, float, float, float, float);
};
