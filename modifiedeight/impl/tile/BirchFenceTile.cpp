#include <tile/BirchFenceTile.hpp>
#include <tile/material/Material.hpp>

BirchFenceTile::BirchFenceTile(int32_t id)
	: FenceTile(id, "fence_birch", Material::wood) {
}

BirchFenceTile::~BirchFenceTile() {
}

TextureUVCoordinateSet* BirchFenceTile::getTexture(int32_t face) {
	return this->getTexture(face, 0);
}

TextureUVCoordinateSet* BirchFenceTile::getTexture(int32_t face, int32_t data) {
	return Tile::wood->getTexture(face, 2); // 2 = Birch Wood
}

std::string BirchFenceTile::getTypeDescriptionId(int32_t data) {
	return "";
}
