#include <tile/StainedGlassTile.hpp>
#include <rendering/TextureAtlasTextureItem.hpp>

StainedGlassTile::StainedGlassTile(int32_t id, const std::string& s, Material* mat)
	: GlassTile(id, s, mat) {
	
	const char* colors[] = {
		"white", "orange", "magenta", "light_blue", "yellow", "lime", 
		"pink", "gray", "silver", "cyan", "purple", "blue", "brown", 
		"green", "red", "black"
	};
	
	for(int i = 0; i < 16; i++) {
		std::string texName = "glass_" + std::string(colors[i]);
		this->textures[i] = *this->getTextureItem(texName)->getUV(0);
	}
}

StainedGlassTile::~StainedGlassTile() {
}

TextureUVCoordinateSet* StainedGlassTile::getTexture(int32_t face, int32_t data) {
	int index = data & 15;
	return &this->textures[index];
}

std::string StainedGlassTile::getTypeDescriptionId(int32_t data) {
	// Need to use DyePowderItem::COLOR_DESCS array
	const char* colors[] = {"black", "red", "green", "brown", "blue", "purple", "cyan", "silver", "gray", "pink", "lime", "yellow", "lightBlue", "magenta", "orange", "white"};
	return colors[(~data) & 0xF];
}
