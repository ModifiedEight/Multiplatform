#pragma once
#include <rendering/model/Model.hpp>
#include <rendering/model/ModelPart.hpp>

struct Boat;

struct BoatModel : Model {
	ModelPart cubes[5];
	ModelPart paddles[2];

	BoatModel();
	virtual ~BoatModel();

	virtual void render(Entity*, float, float, float, float, float, float);
	void animatePaddle(Boat*, int32_t, float);
};
