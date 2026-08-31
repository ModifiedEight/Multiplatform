#pragma once
#include <rendering/model/Model.hpp>
#include <rendering/model/ModelPart.hpp>

struct SlimeModel: Model {
	ModelPart outerCube;
	ModelPart innerCube;
	ModelPart eye0;
	ModelPart eye1;
	ModelPart mouth;

	SlimeModel();
	virtual ~SlimeModel();
	virtual void render(Entity*, float, float, float, float, float, float);
};
