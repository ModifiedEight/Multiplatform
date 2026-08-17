#include <level/gen/feature/WaterLilyFeature.hpp>
#include <level/Level.hpp>
#include <tile/Tile.hpp>
#include <tile/material/Material.hpp>

WaterLilyFeature::WaterLilyFeature() : Feature(0) {
}

WaterLilyFeature::~WaterLilyFeature() {
}

bool_t WaterLilyFeature::place(Level* level, Random* random, int32_t x, int32_t y, int32_t z) {
	for (int i = 0; i < 10; ++i) {
		int32_t rx = x + (random->genrand_int32() % 8) - (random->genrand_int32() % 8);
		int32_t rz = z + (random->genrand_int32() % 8) - (random->genrand_int32() % 8);
		int32_t ry = y + (random->genrand_int32() % 4) - (random->genrand_int32() % 4);

		if (ry > 0 && ry < 127 && level->isEmptyTile(rx, ry, rz)) {
			if (Tile::waterLily && Tile::waterLily->canSurvive(level, rx, ry, rz)) {
				level->setTileAndData(rx, ry, rz, Tile::waterLily->blockID, 0, 2);
			}
		}
	}
	return 1;
}
