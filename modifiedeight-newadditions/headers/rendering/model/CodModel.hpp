#pragma once
#include <rendering/model/Model.hpp>
#include <rendering/model/ModelPart.hpp>

struct CodModel: Model {
	ModelPart body;
	ModelPart head;
	ModelPart nose;
	ModelPart finSide0;
	ModelPart finSide1;
	ModelPart finBack;
	ModelPart finTop;

	CodModel();
	virtual ~CodModel();
	virtual void render(Entity*, float, float, float, float, float, float);
	virtual void setupAnim(float, float, float, float, float, float);
};
