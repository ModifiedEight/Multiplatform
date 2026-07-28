#include <tile/ColoredLogTile.hpp>
#include <tile/material/Material.hpp>

ColoredLogTile::ColoredLogTile(int32_t id, int32_t colorIndex)
	: RotatedPillarTile(id, Material::wood), colorIndex(colorIndex) {
	this->texSide = *this->getTextureItem("colored_log_side");
	this->texTop = *this->getTextureItem("colored_log_top");
	this->textureUV = *this->texSide.getUV(colorIndex);
	this->setDestroyTime(2.0f);
	this->setExplodeable(5.0f);
	this->setSoundType(Tile::SOUND_WOOD);
}

ColoredLogTile::~ColoredLogTile() {}

int32_t ColoredLogTile::getResource(int32_t, Random*) {
	return this->blockID;
}

int32_t ColoredLogTile::getResourceCount(Random*) {
	return 1;
}

int32_t ColoredLogTile::getSpawnResourcesAuxValue(int32_t data) {
	return 0;
}

TextureUVCoordinateSet* ColoredLogTile::getDirTexture(int32_t dir, int32_t data) {
	if (dir == 0) {
		return this->texTop.getUV(colorIndex);
	} else {
		return this->texSide.getUV(colorIndex);
	}
}
