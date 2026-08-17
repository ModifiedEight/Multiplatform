#include <level/gen/feature/DoublePlantFeature.hpp>
#include <util/Random.hpp>
#include <level/Level.hpp>
#include <tile/Tile.hpp>

DoublePlantFeature::DoublePlantFeature(int32_t type)
	: Feature(0), type(type) {
}

DoublePlantFeature::~DoublePlantFeature() {
}

bool_t DoublePlantFeature::place(Level* level, Random* random, int32_t x, int32_t y, int32_t z) {
	bool_t placed = 0;
	for (int32_t i = 0; i < 64; ++i) {
		int32_t rx = x + (random->genrand_int32() & 7) - (random->genrand_int32() & 7);
		int32_t ry = y + (random->genrand_int32() & 3) - (random->genrand_int32() & 3);
		int32_t rz = z + (random->genrand_int32() & 7) - (random->genrand_int32() & 7);

		if (level->isEmptyTile(rx, ry, rz) && level->isEmptyTile(rx, ry + 1, rz) && Tile::doublePlant && Tile::doublePlant->canSurvive(level, rx, ry, rz)) {
			level->setTileAndData(rx, ry, rz, Tile::doublePlant->blockID, this->type, 2);
			level->setTileAndData(rx, ry + 1, rz, Tile::doublePlant->blockID, this->type | 8, 2);
			placed = 1;
		}
	}
	return placed;
}
