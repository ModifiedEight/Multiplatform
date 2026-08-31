#pragma once
#include <rendering/model/Model.hpp>
#include <rendering/model/ModelPart.hpp>

struct FrogModel: Model {
	ModelPart body;
	ModelPart head;
	ModelPart croakingBody;
	ModelPart leftEye;
	ModelPart rightEye;
	ModelPart leftArm;
	ModelPart rightArm;
	ModelPart leftLeg;
	ModelPart rightLeg;

	FrogModel();
	virtual ~FrogModel();
	virtual void render(Entity*, float, float, float, float, float, float);
	virtual void setupAnim(float, float, float, float, float, float);
};
