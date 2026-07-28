#pragma once
#include <tile/GlassTile.hpp>
#include <rendering/TextureUVCoordinateSet.hpp>
#include <string>

struct StainedGlassTile: GlassTile
{
	TextureUVCoordinateSet textures[16];

	StainedGlassTile(int32_t id, const std::string& s, Material* mat);

	virtual ~StainedGlassTile();
	virtual TextureUVCoordinateSet* getTexture(int32_t face, int32_t data);
	virtual std::string getTypeDescriptionId(int32_t data);
};
