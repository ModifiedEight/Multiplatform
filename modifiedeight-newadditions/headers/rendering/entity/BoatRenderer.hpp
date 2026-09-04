#pragma once
#include <rendering/entity/EntityRenderer.hpp>
#include <rendering/model/BoatModel.hpp>

struct BoatRenderer : EntityRenderer {
	BoatModel model;

	BoatRenderer();
	virtual ~BoatRenderer();

	virtual void render(Entity*, float, float, float, float, float);
};
