#include <tile/HugeMushroomTile.hpp>
#include <level/LevelSource.hpp>
#include <tile/material/Material.hpp>
#include <util/Random.hpp>

HugeMushroomTile::HugeMushroomTile(int32_t id, int32_t type)
	: Tile(id, Material::wood), mushroomType(type) {
	if (type == 0) {
		this->skinTexture = this->getTextureUVCoordinateSet("mushroom_block_skin_brown", 0);
	} else {
		this->skinTexture = this->getTextureUVCoordinateSet("mushroom_block_skin_red", 0);
	}
	this->stemTexture = this->getTextureUVCoordinateSet("mushroom_block_skin_stem", 0);
	this->insideTexture = this->getTextureUVCoordinateSet("mushroom_block_inside", 0);
}

HugeMushroomTile::~HugeMushroomTile() {
}

TextureUVCoordinateSet* HugeMushroomTile::getTexture(int32_t face, int32_t data) {
	if (data == 10) {
		if (face == 0 || face == 1) {
			return &this->insideTexture;
		}
		return &this->stemTexture;
	}
	if (data == 15) {
		return &this->stemTexture;
	}
	if (face == 0) {
		return &this->insideTexture;
	}
	return &this->skinTexture;
}

TextureUVCoordinateSet* HugeMushroomTile::getTexture(LevelSource* level, int32_t x, int32_t y, int32_t z, int32_t face) {
	int32_t data = level ? level->getData(x, y, z) : 0;
	return this->getTexture(face, data);
}

TextureUVCoordinateSet* HugeMushroomTile::getCarriedTexture(int32_t face, int32_t data) {
	if (data == 10 || data == 15) {
		return this->getTexture(face, 10);
	}
	if (face == 0) {
		return &this->insideTexture;
	}
	return &this->skinTexture;
}

int32_t HugeMushroomTile::getResource(int32_t data, Random* random) {
	return (this->mushroomType == 0) ? Tile::mushroom1->blockID : Tile::mushroom2->blockID;
}

int32_t HugeMushroomTile::getResourceCount(Random* random) {
	int32_t cnt = random->genrand_int32() % 3;
	return cnt;
}

