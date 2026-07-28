#include <tile/ColoredPlanksTile.hpp>
#include <tile/material/Material.hpp>

ColoredPlanksTile::ColoredPlanksTile(int32_t id)
	: Tile(id, Material::wood) {
	this->tex = *this->getTextureItem("colored_planks");
	this->textureUV = *this->tex.getUV(0);
}

ColoredPlanksTile::~ColoredPlanksTile() {
}

TextureUVCoordinateSet* ColoredPlanksTile::getTexture(int32_t a2, int32_t a3) {
	if(a3) return this->tex.getUV(a3);
	return &this->textureUV;
}

int32_t ColoredPlanksTile::getSpawnResourcesAuxValue(int32_t a2) {
	return a2;
}
