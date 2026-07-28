#pragma once
#include <tile/FenceTile.hpp>

struct BirchFenceTile : FenceTile {
	BirchFenceTile(int32_t id);
	virtual ~BirchFenceTile();
	virtual TextureUVCoordinateSet* getTexture(int32_t face);
	virtual TextureUVCoordinateSet* getTexture(int32_t face, int32_t data);
	virtual std::string getTypeDescriptionId(int32_t data);
};
