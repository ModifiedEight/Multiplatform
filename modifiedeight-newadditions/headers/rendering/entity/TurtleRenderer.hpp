#pragma once
#include <rendering/entity/MobRenderer.hpp>
#include <rendering/model/TurtleModel.hpp>

struct TurtleRenderer: MobRenderer {
	TurtleRenderer(TurtleModel*, float);
	virtual ~TurtleRenderer();
	virtual void render(Entity*, float, float, float, float, float);
};
