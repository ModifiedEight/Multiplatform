#pragma once
#include <rendering/textures/DynamicTexture.hpp>

struct TallSeagrassBottomTexture : DynamicTexture {
	int32_t ticks;

	TallSeagrassBottomTexture(const TextureUVCoordinateSet&);
	virtual ~TallSeagrassBottomTexture();
	virtual void tick();
};
