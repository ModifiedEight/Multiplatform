#include <tile/StainedGlassPaneTile.hpp>
#include <rendering/TextureAtlasTextureItem.hpp>

StainedGlassPaneTile::StainedGlassPaneTile(int32_t id, const std::string& s, const std::string& topTex, const Material* mat, bool_t canBeConnected)
	: ThinFenceTile(id, s, topTex, mat, canBeConnected) {
	
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

StainedGlassPaneTile::~StainedGlassPaneTile() {
}

bool_t StainedGlassPaneTile::attachsTo(int32_t a2) {
	// Attaches to itself, glass, solid blocks, and also stained glass blocks (id 195)
	return Tile::solid[a2] || a2 == this->blockID || a2 == Tile::glass->blockID || a2 == 195;
}

TextureUVCoordinateSet* StainedGlassPaneTile::getTexture(int32_t face, int32_t data) {
	int index = data & 15;
	return &this->textures[index];
}

std::string StainedGlassPaneTile::getTypeDescriptionId(int32_t data) {
	const char* colors[] = {"black", "red", "green", "brown", "blue", "purple", "cyan", "silver", "gray", "pink", "lime", "yellow", "lightBlue", "magenta", "orange", "white"};
	return colors[(~data) & 0xF];
}
