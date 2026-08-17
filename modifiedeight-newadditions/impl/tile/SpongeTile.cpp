#include <tile/SpongeTile.hpp>
#include <tile/material/Material.hpp>
#include <level/Level.hpp>
#include <stdlib.h>

SpongeTile::SpongeTile(int32_t id)
	: Tile(id, "sponge", Material::sponge ? Material::sponge : Material::dirt) {
}

SpongeTile::~SpongeTile() {
}

void SpongeTile::onPlace(Level* level, int32_t x, int32_t y, int32_t z) {
	this->_absorbWater(level, x, y, z);
}

void SpongeTile::_absorbWater(Level* level, int32_t x, int32_t y, int32_t z) {
	if (!level) return;
	for (int32_t dx = -3; dx <= 3; ++dx) {
		for (int32_t dy = -3; dy <= 3; ++dy) {
			for (int32_t dz = -3; dz <= 3; ++dz) {
				if (abs(dx) + abs(dy) + abs(dz) <= 4) {
					int32_t tx = x + dx;
					int32_t ty = y + dy;
					int32_t tz = z + dz;
					if (ty >= 0 && ty < 128) {
						if (level->getMaterial(tx, ty, tz) == Material::water) {
							level->setTileAndData(tx, ty, tz, 0, 0, 3);
						}
					}
				}
			}
		}
	}
}
