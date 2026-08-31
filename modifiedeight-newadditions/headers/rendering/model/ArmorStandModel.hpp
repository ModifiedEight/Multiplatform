#pragma once
#include <rendering/model/HumanoidModel.hpp>

struct ArmorStandModel : HumanoidModel {
	ModelPart rightBodyStick;
	ModelPart leftBodyStick;
	ModelPart shoulderStick;
	ModelPart basePlate;

	ArmorStandModel();
	virtual ~ArmorStandModel();
	virtual void render(Entity*, float, float, float, float, float, float);
};
