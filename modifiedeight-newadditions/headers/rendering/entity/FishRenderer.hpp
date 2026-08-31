#pragma once
#include <rendering/entity/MobRenderer.hpp>
#include <string>

struct FishRenderer: MobRenderer {
	std::string texturePath;

	FishRenderer(Model*, const std::string&, float);
	virtual ~FishRenderer();
	virtual void setupRotations(Entity*, float, float, float);
	virtual void render(Entity*, float, float, float, float, float);
};
