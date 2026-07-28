#include <tile/ColoredBricksTile.hpp>
#include <tile/material/Material.hpp>

ColoredBricksTile::ColoredBricksTile(int32_t id)
	: Tile(id, Material::stone) {
	this->tex = *this->getTextureItem("colored_bricks");
	this->textureUV = *this->tex.getUV(0);
}

ColoredBricksTile::~ColoredBricksTile() {
}

TextureUVCoordinateSet* ColoredBricksTile::getTexture(int32_t a2, int32_t a3) {
	if(a3) return this->tex.getUV(a3);
	return &this->textureUV;
}

int32_t ColoredBricksTile::getSpawnResourcesAuxValue(int32_t a2) {
	return a2;
}
