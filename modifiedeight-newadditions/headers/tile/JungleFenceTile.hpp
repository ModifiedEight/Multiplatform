#pragma once
#include <tile/FenceTile.hpp>

struct JungleFenceTile : FenceTile {
	JungleFenceTile(int32_t id);
	virtual ~JungleFenceTile();
	virtual TextureUVCoordinateSet* getTexture(int32_t face);
	virtual TextureUVCoordinateSet* getTexture(int32_t face, int32_t data);
	virtual std::string getTypeDescriptionId(int32_t data);
};
