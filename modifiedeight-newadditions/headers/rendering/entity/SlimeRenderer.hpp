#pragma once
#include <rendering/entity/MobRenderer.hpp>
#include <rendering/model/SlimeModel.hpp>

struct SlimeRenderer: MobRenderer {
	SlimeRenderer(SlimeModel*, float);
	virtual ~SlimeRenderer();
	virtual void scale(Mob*, float);
};
