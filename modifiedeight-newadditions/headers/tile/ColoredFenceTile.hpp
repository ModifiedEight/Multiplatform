#pragma once
#include <tile/FenceTile.hpp>
#include <rendering/TextureAtlasTextureItem.hpp>

struct ColoredFenceTile : FenceTile {
	TextureAtlasTextureItem tex;
	int32_t colorIndex;

	ColoredFenceTile(int32_t id, int32_t colorIndex);

	virtual ~ColoredFenceTile();
	virtual TextureUVCoordinateSet* getTexture(int32_t face, int32_t data);
	virtual TextureUVCoordinateSet* getTexture(int32_t face);
};
