#pragma once
#include <tile/ThinFenceTile.hpp>
#include <rendering/TextureUVCoordinateSet.hpp>
#include <string>

struct StainedGlassPaneTile: ThinFenceTile
{
	TextureUVCoordinateSet textures[16];

	StainedGlassPaneTile(int32_t id, const std::string& s, const std::string& topTex, const Material* mat, bool_t canBeConnected);

	virtual ~StainedGlassPaneTile();
	bool_t attachsTo(int32_t a2);
	virtual TextureUVCoordinateSet* getTexture(int32_t face, int32_t data);
	virtual std::string getTypeDescriptionId(int32_t data);
};
