#pragma once
#include <rendering/entity/EntityRenderer.hpp>

struct ItemFrameRenderer : EntityRenderer {
	ItemFrameRenderer();
	virtual ~ItemFrameRenderer();
	virtual void render(Entity*, float, float, float, float, float);
};
