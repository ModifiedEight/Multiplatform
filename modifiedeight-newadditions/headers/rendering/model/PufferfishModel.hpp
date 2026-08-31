#pragma once
#include <rendering/model/Model.hpp>
#include <rendering/model/ModelPart.hpp>

struct PufferfishModel: Model {
	ModelPart cube;
	ModelPart eye0;
	ModelPart eye1;
	ModelPart fin0;
	ModelPart fin1;
	ModelPart finBack;

	PufferfishModel();
	virtual ~PufferfishModel();
	virtual void render(Entity*, float, float, float, float, float, float);
	virtual void setupAnim(float, float, float, float, float, float);
};
