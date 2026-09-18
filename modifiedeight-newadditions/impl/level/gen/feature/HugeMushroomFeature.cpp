#include <level/gen/feature/HugeMushroomFeature.hpp>
#include <level/Level.hpp>
#include <tile/Tile.hpp>
#include <util/Random.hpp>

HugeMushroomFeature::HugeMushroomFeature(int32_t type)
	: Feature(0), mushroomType(type) {
}

HugeMushroomFeature::~HugeMushroomFeature() {
}

bool_t HugeMushroomFeature::place(Level* level, Random* random, int32_t x, int32_t y, int32_t z) {
	int32_t type = this->mushroomType;
	if (type < 0) {
		type = random->genrand_int32() % 2;
	}

	int32_t height = 4 + (random->genrand_int32() % 4);
	if (y < 1 || y + height + 1 >= 128) {
		return 0;
	}

	int32_t ground = level->getTile(x, y - 1, z);
	if (ground != Tile::dirt->blockID && ground != Tile::grass->blockID && (!Tile::mycelium || ground != Tile::mycelium->blockID)) {
		return 0;
	}

	for (int32_t checkY = y; checkY <= y + height + 1; ++checkY) {
		int32_t radius = (checkY <= y + 2) ? 0 : 3;
		for (int32_t checkX = x - radius; checkX <= x + radius; ++checkX) {
			for (int32_t checkZ = z - radius; checkZ <= z + radius; ++checkZ) {
				int32_t t = level->getTile(checkX, checkY, checkZ);
				if (t != 0 && t != Tile::leaves->blockID) {
					return 0;
				}
			}
		}
	}

	this->placeBlock(level, x, y - 1, z, Tile::dirt->blockID, 0);

	int32_t capTileId = (type == 0) ? (Tile::mushroomBlockBrown ? Tile::mushroomBlockBrown->blockID : 99)
	                                : (Tile::mushroomBlockRed ? Tile::mushroomBlockRed->blockID : 100);

	for (int32_t dy = 0; dy < height; ++dy) {
		this->placeBlock(level, x, y + dy, z, capTileId, 10);
	}

	if (type == 0) {
		int32_t cy = y + height;
		for (int32_t dx = -3; dx <= 3; ++dx) {
			for (int32_t dz = -3; dz <= 3; ++dz) {
				bool edgeX_min = (dx == -3);
				bool edgeX_max = (dx == 3);
				bool edgeZ_min = (dz == -3);
				bool edgeZ_max = (dz == 3);
				if ((edgeX_min || edgeX_max) && (edgeZ_min || edgeZ_max)) {
					continue;
				}
				int32_t meta = 5;
				if (edgeX_min && edgeZ_min) meta = 1;
				else if (edgeZ_min && !edgeX_min && !edgeX_max) meta = 2;
				else if (edgeX_max && edgeZ_min) meta = 3;
				else if (edgeX_min && !edgeZ_min && !edgeZ_max) meta = 4;
				else if (edgeX_max && !edgeZ_min && !edgeZ_max) meta = 6;
				else if (edgeX_min && edgeZ_max) meta = 7;
				else if (edgeZ_max && !edgeX_min && !edgeX_max) meta = 8;
				else if (edgeX_max && edgeZ_max) meta = 9;

				this->placeBlock(level, x + dx, cy, z + dz, capTileId, meta);
			}
		}
	} else {
		for (int32_t cy = y + height - 2; cy <= y + height; ++cy) {
			int32_t radius = (cy < y + height) ? 2 : 1;
			for (int32_t dx = -radius; dx <= radius; ++dx) {
				for (int32_t dz = -radius; dz <= radius; ++dz) {
					bool edgeX_min = (dx == -radius);
					bool edgeX_max = (dx == radius);
					bool edgeZ_min = (dz == -radius);
					bool edgeZ_max = (dz == radius);

					if (radius == 2) {
						if ((edgeX_min || edgeX_max) && (edgeZ_min || edgeZ_max)) {
							continue;
						}
						if (!edgeX_min && !edgeX_max && !edgeZ_min && !edgeZ_max) {
							continue;
						}
					}

					int32_t meta = 5;
					if (edgeX_min && edgeZ_min) meta = 1;
					else if (edgeZ_min && !edgeX_min && !edgeX_max) meta = 2;
					else if (edgeX_max && edgeZ_min) meta = 3;
					else if (edgeX_min && !edgeZ_min && !edgeZ_max) meta = 4;
					else if (edgeX_max && !edgeZ_min && !edgeZ_max) meta = 6;
					else if (edgeX_min && edgeZ_max) meta = 7;
					else if (edgeZ_max && !edgeX_min && !edgeX_max) meta = 8;
					else if (edgeX_max && edgeZ_max) meta = 9;

					this->placeBlock(level, x + dx, cy, z + dz, capTileId, meta);
				}
			}
		}
	}

	return 1;
}

