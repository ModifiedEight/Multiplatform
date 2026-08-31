#pragma once
#include <rendering/model/Model.hpp>
#include <rendering/model/ModelPart.hpp>

struct TropicalFishModel: Model {
	ModelPart body;
	ModelPart tail;
	ModelPart leftFin;
	ModelPart rightFin;
	ModelPart topFin;

	TropicalFishModel();
	virtual ~TropicalFishModel();
	virtual void render(Entity*, float, float, float, float, float, float);
	virtual void setupAnim(float, float, float, float, float, float);
};
