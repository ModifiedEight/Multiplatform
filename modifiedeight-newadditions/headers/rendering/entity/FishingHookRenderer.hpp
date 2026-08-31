#pragma once
#include <rendering/entity/EntityRenderer.hpp>

struct FishingHookRenderer: EntityRenderer {
	FishingHookRenderer();
	virtual ~FishingHookRenderer();
	virtual void render(Entity*, float, float, float, float, float);
};
