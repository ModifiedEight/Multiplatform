#pragma once
#include <rendering/textures/DynamicTexture.hpp>

struct SeagrassTexture : DynamicTexture {
	uint8_t originalPixels[1024];
	int32_t ticks;

	SeagrassTexture(const TextureUVCoordinateSet&);
	virtual ~SeagrassTexture();
	virtual void tick();
};
