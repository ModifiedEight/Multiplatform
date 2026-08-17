#pragma once
#include <rendering/textures/DynamicTexture.hpp>

struct TallSeagrassTopTexture : DynamicTexture {
	int32_t ticks;

	TallSeagrassTopTexture(const TextureUVCoordinateSet&);
	virtual ~TallSeagrassTopTexture();
	virtual void tick();
};
