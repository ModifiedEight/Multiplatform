#include <tile/JungleFenceTile.hpp>
#include <tile/material/Material.hpp>

JungleFenceTile::JungleFenceTile(int32_t id)
	: FenceTile(id, "fence_jungle", Material::wood) {
}

JungleFenceTile::~JungleFenceTile() {
}

TextureUVCoordinateSet* JungleFenceTile::getTexture(int32_t face) {
	return this->getTexture(face, 0);
}

TextureUVCoordinateSet* JungleFenceTile::getTexture(int32_t face, int32_t data) {
	return Tile::wood->getTexture(face, 3);
}

std::string JungleFenceTile::getTypeDescriptionId(int32_t data) {
	return "";
}
