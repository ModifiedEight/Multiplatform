#include <level/gen/feature/MelonFeature.hpp>
#include <level/Level.hpp>
#include <tile/Tile.hpp>
#include <util/Random.hpp>

MelonFeature::MelonFeature()
	: Feature(0) {
}

MelonFeature::~MelonFeature() {
}

bool_t MelonFeature::place(Level* level, Random* random, int32_t x, int32_t y, int32_t z) {
	for (int32_t i = 0; i < 64; ++i) {
		int32_t rx = x + (random->genrand_int32() & 7) - (random->genrand_int32() & 7);
		int32_t ry = y + (random->genrand_int32() & 3) - (random->genrand_int32() & 3);
		int32_t rz = z + (random->genrand_int32() & 7) - (random->genrand_int32() & 7);

		if (level->isEmptyTile(rx, ry, rz) && level->getTile(rx, ry - 1, rz) == Tile::grass->blockID && Tile::melon) {
			this->placeBlock(level, rx, ry, rz, Tile::melon->blockID, 0);
		}
	}
	return 1;
}
