#pragma once
#include <rendering/textures/DynamicTexture.hpp>

struct FoliageTexture : DynamicTexture {
	uint8_t originalPixels[1024];
	int32_t ticks;
	bool initialized;
	float speed;
	float phase;
	float amplitude;
	int fixedRows;
	bool isVine;
	bool isLeaf;
	bool isReeds;
	bool isWaterlily;
	bool isDoublePlantTop;
	bool isDoublePlantBottom;

	FoliageTexture(const TextureUVCoordinateSet&, float speed = 0.06f, float phase = 0.0f, float amplitude = 1.3f, int fixedRows = 2, bool isVine = false, bool isLeaf = false, bool isReeds = false, bool isWaterlily = false, bool isDoublePlantTop = false, bool isDoublePlantBottom = false);
	virtual ~FoliageTexture();
	virtual void tick();
};

