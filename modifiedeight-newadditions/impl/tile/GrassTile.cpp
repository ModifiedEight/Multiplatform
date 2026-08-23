#include <tile/GrassTile.hpp>
#include <tile/BlockColorRegistry.hpp>
#include <level/Level.hpp>
#include <level/biome/Biome.hpp>
#include <tile/material/Material.hpp>
#include <rendering/TextureAtlasTextureItem.hpp>

GrassTile::GrassTile(int32_t id)
	: Tile(id, Material::dirt) {
	this->setTicking(1);
	TextureAtlasTextureItem texIt = *this->getTextureItem("grass");
	this->field_98 = *texIt.getUV(0);
	this->field_B0 = *texIt.getUV(1);
	this->field_C8 = *texIt.getUV(2);
	this->field_E0 = *texIt.getUV(3);
	this->field_80 = this->getTextureUVCoordinateSet("dirt", 0);
	this->field_5C = 3;
}
bool_t GrassTile::_randomWalk(Level* level, int32_t& x, int32_t& y, int32_t& z, int32_t a6) {
	Random* p_random; // r5
	int32_t i;		  // r10
	int32_t v11;	  // r3
	uint32_t v13;	  // [sp+0h] [bp-30h]

	p_random = &level->random;
	for(i = 0;; ++i) {
		if(i >= a6 / 16) {
			return a6 > 15;
		}
		x += p_random->genrand_int32() % 3 - 1;
		v13 = p_random->genrand_int32() % 3 - 1;
		y += (int)(p_random->genrand_int32() % 3 * v13) / 2;
		v11 = p_random->genrand_int32() % 3 - 1 + z;
		z = v11;
		if(level->getTile(x, y - 1, v11) != Tile::grass->blockID || level->isSolidBlockingTile(x, y, z)) {
			break;
		}
	}
	return 0;
}

GrassTile::~GrassTile() {
}
bool_t GrassTile::onFertilized(Level* level, int32_t x, int32_t y, int32_t z) {
	Random* p_random = &level->random;
	int32_t v15;
	int32_t v16;
	int32_t v17;

	for(int32_t i = 16; i != 64; ++i) {
		v15 = x;
		v16 = y + 1;
		v17 = z;
		if(this->_randomWalk(level, v15, v16, v17, i)) {
			if(level->isEmptyTile(v15, v16, v17)) {
				int32_t roll = p_random->genrand_int32() % 100;
				if(roll < 12 && Tile::doublePlant && level->isEmptyTile(v15, v16 + 1, v17) && Tile::doublePlant->canSurvive(level, v15, v16, v17)) {
					int32_t subtype = p_random->genrand_int32() % 4;
					level->setTileAndData(v15, v16, v17, Tile::doublePlant->blockID, subtype, 3);
					level->setTileAndData(v15, v16 + 1, v17, Tile::doublePlant->blockID, subtype | 8, 3);
				} else if(roll < 35) {
					Tile* flowers[] = {
						Tile::flower,
						Tile::rose,
						Tile::flowerRose,
						Tile::flowerPaeonia,
						Tile::flowerDaisy,
						Tile::flowerHoustonia,
						Tile::flowerOrchid,
						Tile::flowerAllium
					};
					int32_t flowerIdx = p_random->genrand_int32() % 8;
					Tile* fl = flowers[flowerIdx];
					if(fl && fl->canSurvive(level, v15, v16, v17)) {
						level->setTileAndData(v15, v16, v17, fl->blockID, 0, 3);
					}
				} else {
					if(Tile::tallgrass && Tile::tallgrass->canSurvive(level, v15, v16, v17)) {
						int32_t grassMeta = (p_random->genrand_int32() % 4 == 0) ? 2 : 1;
						level->setTileAndData(v15, v16, v17, Tile::tallgrass->blockID, grassMeta, 3);
					}
				}
			}
		}
	}
	return 1;
}
TextureUVCoordinateSet* GrassTile::getTexture(int32_t a2, int32_t a3) {
	if(a2 == 1) {
		return &this->field_98;
	}
	if(a2) {
		return &this->field_B0;
	}
	return &this->field_80;
}
#include <level/LevelSource.hpp>

static bool_t isSnowMaterialOrTile(LevelSource* level, int32_t x, int32_t y, int32_t z) {
	if (!level) return 0;
	Material* m = level->getMaterial(x, y, z);
	if (m == Material::topSnow || m == Material::snow) return 1;
	int32_t id = level->getTile(x, y, z);
	return id == 78 || id == 80;
}

static bool_t isGrassTileSnowy(LevelSource* level, int32_t x, int32_t y, int32_t z) {
	if (!level) return 0;
	return isSnowMaterialOrTile(level, x, y + 1, z);
}

TextureUVCoordinateSet* GrassTile::getTexture(LevelSource* level, int32_t x, int32_t y, int32_t z, int32_t a6) {
	if(!a6) {
		return &this->field_80;
	}
	if(a6 == 1) {
		return &this->field_98;
	}
	if(isGrassTileSnowy(level, x, y, z)) {
		return &this->field_C8;
	} else {
		return &this->field_B0;
	}
}
TextureUVCoordinateSet* GrassTile::getCarriedTexture(int32_t a2, int32_t a3) {
	if(a2 == 1) {
		return &this->field_E0;
	} else {
		return this->getTexture(a2, a3);
	}
}
void GrassTile::tick(Level* level, int32_t x, int32_t y, int32_t z, Random* random) {
	int32_t zr;
	Material* v10;
	int32_t xr;
	int32_t yr;
	int32_t v13;
	int32_t v14;
	Material* v15;
	Level* v16;
	int32_t v17;
	int32_t v18;
	int32_t za;

	zr = z;
	if(!level->isClientMaybe) {
		if(level->getRawBrightness(x, y + 1, z) <= 3) {
			v10 = level->getMaterial(x, y + 1, z);
			if(v10->blocksLight()) {
				if(random->genrand_int32() << 30) {
					return;
				}
				v16 = level;
				v17 = x;
				v18 = y;
				za = Tile::dirt->blockID;
				goto LABEL_13;
			}
		}
		if(level->getRawBrightness(x, y + 1, z) > 8) {
			xr = x + random->genrand_int32() % 3 - 1;
			yr = y + random->genrand_int32() % 5;
			v13 = yr - 3;
			zr = z + random->genrand_int32() % 3 - 1;
			if(level->getTile(xr, yr - 3, zr) == Tile::dirt->blockID) {
				v14 = yr - 2;
				if(level->getRawBrightness(xr, v14, zr) > 3) {
					v15 = level->getMaterial(xr, v14, zr);
					if(!v15->blocksLight()) {
						v16 = level;
						v17 = xr;
						v18 = v13;
						za = Tile::grass->blockID;
LABEL_13:
						v16->setTile(v17, v18, zr, za, 2);
					}
				}
			}
		}
	}
}
int32_t GrassTile::getResource(int32_t, Random* a3) {
	return Tile::dirt->getResource(0, a3);
}
int32_t GrassTile::getColor(int32_t) {
	return 0x71A74D;
}
int32_t GrassTile::getColor(LevelSource* level, int32_t x, int32_t y, int32_t z) {
	if (level) {
		int totalR = 0, totalG = 0, totalB = 0;
		for (int sx = -1; sx <= 1; ++sx) {
			for (int sz = -1; sz <= 1; ++sz) {
				Biome* b = level->getBiome(x + sx, z + sz);
				int c = 0x71A74D;
				if (b == Biome::swampland) c = 0x4C5325;
				else if (b == Biome::plains) c = 0x8EB971;
				else if (b == Biome::forest) c = 0x79C05A;
				else if (b == Biome::seasonalForest) c = 0x68B244;
				else if (b == Biome::birchForest) c = 0x88B35A;
				else if (b == Biome::jungle || b == Biome::rainForest) c = 0x59C93C;
				else if (b == Biome::taiga) c = 0x86B783;
				else if (b == Biome::tundra || b == Biome::icePeaks) c = 0x80B497;
				else if (b == Biome::savanna) c = 0xBFB755;
				else if (b == Biome::desert || b == Biome::iceDesert) c = 0xBFA243;
				totalR += (c >> 16) & 0xFF;
				totalG += (c >> 8) & 0xFF;
				totalB += c & 0xFF;
			}
		}
		return ((totalR / 9) << 16) | ((totalG / 9) << 8) | (totalB / 9);
	}
	return this->getColor(0);
}
