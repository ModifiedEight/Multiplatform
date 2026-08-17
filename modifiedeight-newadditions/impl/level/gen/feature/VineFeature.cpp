#include <level/gen/feature/VineFeature.hpp>
#include <level/Level.hpp>
#include <tile/Tile.hpp>
#include <tile/VineTile.hpp>
#include <util/Random.hpp>

VineFeature::VineFeature()
	: Feature(0) {
}

VineFeature::~VineFeature() {
}

bool_t VineFeature::place(Level* level, Random* random, int32_t x, int32_t y, int32_t z) {
	if (!Tile::vine) return 0;
	for (int32_t i = 0; i < 64; ++i) {
		int32_t rx = x + (random->genrand_int32() & 7) - (random->genrand_int32() & 7);
		int32_t ry = y + (random->genrand_int32() & 3) - (random->genrand_int32() & 3);
		int32_t rz = z + (random->genrand_int32() & 7) - (random->genrand_int32() & 7);

		if (level->isEmptyTile(rx, ry, rz)) {
			int32_t meta = 0;
			if (VineTile::canAttachTo(level, rx, ry, rz - 1)) meta |= 1;
			if (VineTile::canAttachTo(level, rx + 1, ry, rz)) meta |= 2;
			if (VineTile::canAttachTo(level, rx, ry, rz + 1)) meta |= 4;
			if (VineTile::canAttachTo(level, rx - 1, ry, rz)) meta |= 8;
			if (meta != 0) {
				this->placeBlock(level, rx, ry, rz, Tile::vine->blockID, meta);
			}
		}
	}
	return 1;
}
