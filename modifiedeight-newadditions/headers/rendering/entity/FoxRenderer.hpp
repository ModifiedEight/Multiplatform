#pragma once
#include <rendering/entity/MobRenderer.hpp>
#include <rendering/model/FoxModel.hpp>

struct FoxRenderer: MobRenderer {
	FoxRenderer(FoxModel*, float);
	virtual ~FoxRenderer();
};
