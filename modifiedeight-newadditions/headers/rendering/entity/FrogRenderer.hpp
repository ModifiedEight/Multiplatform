#pragma once
#include <rendering/entity/MobRenderer.hpp>
#include <rendering/model/FrogModel.hpp>

struct FrogRenderer: MobRenderer {
	FrogRenderer(FrogModel*, float);
	virtual ~FrogRenderer();
};
