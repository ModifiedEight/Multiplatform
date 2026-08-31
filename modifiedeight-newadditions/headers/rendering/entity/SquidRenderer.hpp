#pragma once
#include <rendering/entity/MobRenderer.hpp>
#include <rendering/model/SquidModel.hpp>

struct SquidRenderer: MobRenderer {
	SquidRenderer(SquidModel*, float);
	virtual ~SquidRenderer();
	virtual void setupRotations(Entity*, float, float, float);
	virtual float getBob(Mob*, float);
	virtual float getAttackAnim(Mob*, float);
};
