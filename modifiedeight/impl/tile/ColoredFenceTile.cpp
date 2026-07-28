#include <tile/ColoredFenceTile.hpp>
#include <tile/material/Material.hpp>

ColoredFenceTile::ColoredFenceTile(int32_t id, int32_t colorIndex)
	: FenceTile(id, "colored_planks", Material::wood), colorIndex(colorIndex) {
	this->tex = *this->getTextureItem("colored_planks");
	this->textureUV = *this->tex.getUV(colorIndex);
	this->setDestroyTime(2.0f);
	this->setExplodeable(5.0f);
	this->setSoundType(Tile::SOUND_WOOD);
}

ColoredFenceTile::~ColoredFenceTile() {}

TextureUVCoordinateSet* ColoredFenceTile::getTexture(int32_t face, int32_t data) {
	return this->tex.getUV(colorIndex);
}

TextureUVCoordinateSet* ColoredFenceTile::getTexture(int32_t face) {
	return this->tex.getUV(colorIndex);
}
