#include <tile/MyceliumTile.hpp>
#include <level/Level.hpp>
#include <level/LevelSource.hpp>
#include <tile/material/Material.hpp>

MyceliumTile::MyceliumTile(int32_t id)
	: Tile(id, Material::dirt) {
	this->setTicking(1);
	this->topUV = this->getTextureUVCoordinateSet("mycelium_top", 0);
	this->sideUV = this->getTextureUVCoordinateSet("mycelium_side", 0);
	this->bottomUV = this->getTextureUVCoordinateSet("dirt", 0);
	this->snowSideUV = this->getTextureUVCoordinateSet("grass", 2);
	this->field_5C = 3;
}

MyceliumTile::~MyceliumTile() {
}

TextureUVCoordinateSet* MyceliumTile::getTexture(int32_t face, int32_t data) {
	if (face == 1) {
		return &this->topUV;
	}
	if (face == 0) {
		return &this->bottomUV;
	}
	return &this->sideUV;
}

TextureUVCoordinateSet* MyceliumTile::getTexture(LevelSource* level, int32_t x, int32_t y, int32_t z, int32_t face) {
	if (face == 1) {
		return &this->topUV;
	}
	if (face == 0) {
		return &this->bottomUV;
	}
	if (level) {
		Material* m = level->getMaterial(x, y + 1, z);
		if (m == Material::topSnow || m == Material::snow || level->getTile(x, y + 1, z) == 78 || level->getTile(x, y + 1, z) == 80) {
			return &this->snowSideUV;
		}
	}
	return &this->sideUV;
}

TextureUVCoordinateSet* MyceliumTile::getCarriedTexture(int32_t face, int32_t data) {
	if (face == 1) {
		return &this->topUV;
	}
	if (face == 0) {
		return &this->bottomUV;
	}
	return &this->sideUV;
}

void MyceliumTile::tick(Level* level, int32_t x, int32_t y, int32_t z, Random* random) {
	if (!level->isClientMaybe) {
		if (level->getRawBrightness(x, y + 1, z) <= 3) {
			Material* m = level->getMaterial(x, y + 1, z);
			if (m && m->blocksLight()) {
				if ((random->genrand_int32() << 30) != 0) {
					return;
				}
				level->setTile(x, y, z, Tile::dirt->blockID, 2);
				return;
			}
		}
		if (level->getRawBrightness(x, y + 1, z) >= 9) {
			for (int i = 0; i < 4; ++i) {
				int32_t xr = x + (random->genrand_int32() % 3) - 1;
				int32_t yr = y + (random->genrand_int32() % 5) - 3;
				int32_t zr = z + (random->genrand_int32() % 3) - 1;
				if (yr > 0 && yr < 127 && level->getTile(xr, yr, zr) == Tile::dirt->blockID) {
					if (level->getRawBrightness(xr, yr + 1, zr) >= 4) {
						Material* aboveM = level->getMaterial(xr, yr + 1, zr);
						if (aboveM && !aboveM->blocksLight()) {
							level->setTile(xr, yr, zr, this->blockID, 2);
						}
					}
				}
			}
		}
	}
}

int32_t MyceliumTile::getResource(int32_t data, Random* random) {
	return Tile::dirt->getResource(0, random);
}

int32_t MyceliumTile::getColor(int32_t data) {
	return 0xFFFFFF;
}

int32_t MyceliumTile::getColor(LevelSource* level, int32_t x, int32_t y, int32_t z) {
	return 0xFFFFFF;
}

bool_t MyceliumTile::onFertilized(Level* level, int32_t x, int32_t y, int32_t z) {
	Random* p_random = &level->random;
	bool placed = false;
	for (int i = 0; i < 16; ++i) {
		int nx = x + (p_random->genrand_int32() % 7) - 3;
		int ny = y + (p_random->genrand_int32() % 3) - 1;
		int nz = z + (p_random->genrand_int32() % 7) - 3;
		if (level->getTile(nx, ny, nz) == this->blockID && level->isEmptyTile(nx, ny + 1, nz)) {
			Tile* shroom = (p_random->genrand_int32() % 2 == 0) ? Tile::mushroom1 : Tile::mushroom2;
			if (shroom && shroom->canSurvive(level, nx, ny + 1, nz)) {
				level->setTileAndData(nx, ny + 1, nz, shroom->blockID, 0, 3);
				placed = true;
			}
		}
	}
	return placed;
}

