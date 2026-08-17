#include <level/gen/feature/JungleBushFeature.hpp>
#include <level/Level.hpp>
#include <tile/Tile.hpp>
#include <util/Random.hpp>

JungleBushFeature::JungleBushFeature(int32_t logMeta, int32_t leavesMeta)
	: Feature(0), logMeta(logMeta), leavesMeta(leavesMeta) {
}

JungleBushFeature::~JungleBushFeature() {
}

bool_t JungleBushFeature::place(Level* level, Random* random, int32_t x, int32_t y, int32_t z) {
	while (y > 0 && (level->isEmptyTile(x, y, z) || level->getTile(x, y, z) == Tile::leaves->blockID)) {
		--y;
	}
	int32_t ground = level->getTile(x, y, z);
	if (ground != Tile::grass->blockID && ground != Tile::dirt->blockID) {
		return 0;
	}
	++y;
	this->placeBlock(level, x, y, z, Tile::treeTrunk->blockID, this->logMeta);

	for (int32_t dy = y; dy <= y + 2; ++dy) {
		int32_t rad = 2 - (dy - y);
		for (int32_t dx = x - rad; dx <= x + rad; ++dx) {
			for (int32_t dz = z - rad; dz <= z + rad; ++dz) {
				if (dx == x && dz == z && dy == y) continue;
				if (abs(dx - x) != rad || abs(dz - z) != rad || random->genrand_int32() % 2 == 0) {
					if (level->isEmptyTile(dx, dy, dz)) {
						this->placeBlock(level, dx, dy, dz, Tile::leaves->blockID, this->leavesMeta);
					}
				}
			}
		}
	}
	return 1;
}
