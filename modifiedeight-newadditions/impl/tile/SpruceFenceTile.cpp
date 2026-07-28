#include <tile/SpruceFenceTile.hpp>
#include <tile/material/Material.hpp>

SpruceFenceTile::SpruceFenceTile(int32_t id)
	: FenceTile(id, "fence_spruce", Material::wood) {
}

SpruceFenceTile::~SpruceFenceTile() {
}

TextureUVCoordinateSet* SpruceFenceTile::getTexture(int32_t face) {
	return this->getTexture(face, 0);
}

TextureUVCoordinateSet* SpruceFenceTile::getTexture(int32_t face, int32_t data) {
	return Tile::wood->getTexture(face, 1); // 1 = Spruce Wood
}

std::string SpruceFenceTile::getTypeDescriptionId(int32_t data) {
	return "";
}
