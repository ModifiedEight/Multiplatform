#include <level/gen/feature/MegaJungleTreeFeature.hpp>
#include <level/Level.hpp>
#include <tile/Tile.hpp>
#include <tile/VineTile.hpp>
#include <util/Random.hpp>
#include <math.h>

MegaJungleTreeFeature::MegaJungleTreeFeature(bool_t update, int32_t baseHeight, int32_t logMeta, int32_t leavesMeta)
	: Feature(update), logMeta(logMeta), leavesMeta(leavesMeta), baseHeight(baseHeight) {
}

MegaJungleTreeFeature::~MegaJungleTreeFeature() {
}

void MegaJungleTreeFeature::addVine(Level* level, int32_t x, int32_t y, int32_t z, int32_t meta) {
	if (Tile::vine && level->isEmptyTile(x, y, z)) {
		this->placeBlock(level, x, y, z, Tile::vine->blockID, meta);
	}
}

void MegaJungleTreeFeature::addHangingVine(Level* level, Random* random, int32_t x, int32_t y, int32_t z, int32_t meta, int32_t length) {
	for (int32_t dy = 0; dy < length; ++dy) {
		int32_t vy = y - dy;
		if (vy <= 0 || !level->isEmptyTile(x, vy, z)) break;
		this->addVine(level, x, vy, z, meta);
	}
}

void MegaJungleTreeFeature::placeLeafCluster(Level* level, int32_t cx, int32_t cy, int32_t cz, int32_t radius) {
	float rSq = ((float)radius + 0.3f) * ((float)radius + 0.3f);
	for (int32_t dx = -radius; dx <= radius; ++dx) {
		for (int32_t dz = -radius; dz <= radius; ++dz) {
			for (int32_t dy = -radius; dy <= radius; ++dy) {
				float distSq = (float)(dx * dx + dz * dz + dy * dy);
				if (distSq <= rSq) {
					int32_t lx = cx + dx;
					int32_t ly = cy + dy;
					int32_t lz = cz + dz;
					if (ly > 0 && ly < 128) {
						int32_t cur = level->getTile(lx, ly, lz);
						if (cur == 0 || cur == Tile::leaves->blockID) {
							this->placeBlock(level, lx, ly, lz, Tile::leaves->blockID, this->leavesMeta);
						}
					}
				}
			}
		}
	}
}

bool_t MegaJungleTreeFeature::place(Level* level, Random* random, int32_t x, int32_t y, int32_t z) {
	int32_t height = this->baseHeight + (random->genrand_int32() % 14);
	if (y < 1 || y + height + 4 >= 128) {
		return 0;
	}

	int32_t b00 = level->getTile(x, y - 1, z);
	int32_t b10 = level->getTile(x + 1, y - 1, z);
	int32_t b01 = level->getTile(x, y - 1, z + 1);
	int32_t b11 = level->getTile(x + 1, y - 1, z + 1);

	bool_t groundOk = (b00 == Tile::grass->blockID || b00 == Tile::dirt->blockID)
		&& (b10 == Tile::grass->blockID || b10 == Tile::dirt->blockID)
		&& (b01 == Tile::grass->blockID || b01 == Tile::dirt->blockID)
		&& (b11 == Tile::grass->blockID || b11 == Tile::dirt->blockID);

	if (!groundOk) {
		return 0;
	}

	this->placeBlock(level, x, y - 1, z, Tile::dirt->blockID);
	this->placeBlock(level, x + 1, y - 1, z, Tile::dirt->blockID);
	this->placeBlock(level, x, y - 1, z + 1, Tile::dirt->blockID);
	this->placeBlock(level, x + 1, y - 1, z + 1, Tile::dirt->blockID);

	if (level->isEmptyTile(x - 1, y, z)) this->placeBlock(level, x - 1, y, z, Tile::treeTrunk->blockID, this->logMeta);
	if (level->isEmptyTile(x + 2, y, z + 1)) this->placeBlock(level, x + 2, y, z + 1, Tile::treeTrunk->blockID, this->logMeta);
	if (level->isEmptyTile(x, y, z - 1)) this->placeBlock(level, x, y, z - 1, Tile::treeTrunk->blockID, this->logMeta);
	if (level->isEmptyTile(x + 1, y, z + 2)) this->placeBlock(level, x + 1, y, z + 2, Tile::treeTrunk->blockID, this->logMeta);

	int32_t topY = y + height;
	int32_t canopyBase = topY - 3;

	for (int32_t dy = 0; dy < height; ++dy) {
		int32_t cy = y + dy;
		this->placeBlock(level, x, cy, z, Tile::treeTrunk->blockID, this->logMeta);
		this->placeBlock(level, x + 1, cy, z, Tile::treeTrunk->blockID, this->logMeta);
		this->placeBlock(level, x, cy, z + 1, Tile::treeTrunk->blockID, this->logMeta);
		this->placeBlock(level, x + 1, cy, z + 1, Tile::treeTrunk->blockID, this->logMeta);

		if (cy < canopyBase) {
			if (random->genrand_int32() % 3 != 0) this->addVine(level, x, cy, z - 1, 4);
			if (random->genrand_int32() % 3 != 0) this->addVine(level, x + 1, cy, z - 1, 4);
			if (random->genrand_int32() % 3 != 0) this->addVine(level, x, cy, z + 2, 1);
			if (random->genrand_int32() % 3 != 0) this->addVine(level, x + 1, cy, z + 2, 1);
			if (random->genrand_int32() % 3 != 0) this->addVine(level, x - 1, cy, z, 2);
			if (random->genrand_int32() % 3 != 0) this->addVine(level, x - 1, cy, z + 1, 2);
			if (random->genrand_int32() % 3 != 0) this->addVine(level, x + 2, cy, z, 8);
			if (random->genrand_int32() % 3 != 0) this->addVine(level, x + 2, cy, z + 1, 8);
		}
	}

	float centerX = (float)x + 0.5f;
	float centerZ = (float)z + 0.5f;

	for (int32_t ly = topY - 3; ly <= topY + 2; ++ly) {
		float layerDist = (float)abs(ly - topY);
		float maxRadius = 4.2f - layerDist * 0.7f;
		float rSq = maxRadius * maxRadius;

		int32_t minX = (int32_t)floor(centerX - maxRadius);
		int32_t maxX = (int32_t)ceil(centerX + maxRadius);
		int32_t minZ = (int32_t)floor(centerZ - maxRadius);
		int32_t maxZ = (int32_t)ceil(centerZ + maxRadius);

		for (int32_t lx = minX; lx <= maxX; ++lx) {
			for (int32_t lz = minZ; lz <= maxZ; ++lz) {
				float distSq = ((float)lx - centerX) * ((float)lx - centerX) + ((float)lz - centerZ) * ((float)lz - centerZ);
				if (distSq <= rSq) {
					if (distSq > rSq * 0.8f && (random->genrand_int32() % 4 == 0)) {
						continue;
					}
					if (ly > 0 && ly < 128) {
						int32_t cur = level->getTile(lx, ly, lz);
						if (cur == 0 || cur == Tile::leaves->blockID) {
							this->placeBlock(level, lx, ly, lz, Tile::leaves->blockID, this->leavesMeta);
						}
					}
				}
			}
		}
	}

	int32_t numBranches = 3 + (random->genrand_int32() % 3);
	for (int32_t b = 0; b < numBranches; ++b) {
		int32_t by = topY - 2 - (b * 3) - (random->genrand_int32() % 2);
		if (by <= y + 6) continue;

		float angle = (float)b * (6.283185f / (float)numBranches) + ((float)(random->genrand_int32() % 100) / 100.0f) * 0.5f;
		float cosA = cosf(angle);
		float sinA = sinf(angle);

		int32_t branchLength = 3 + (random->genrand_int32() % 3);
		int32_t endX = x, endY = by, endZ = z;

		for (int32_t step = 1; step <= branchLength; ++step) {
			int32_t curBx = (int32_t)roundf(centerX + cosA * (float)step);
			int32_t curBz = (int32_t)roundf(centerZ + sinA * (float)step);
			int32_t curBy = by + (step / 2);
			this->placeBlock(level, curBx, curBy, curBz, Tile::treeTrunk->blockID, this->logMeta);
			endX = curBx;
			endY = curBy;
			endZ = curBz;
		}

		this->placeLeafCluster(level, endX, endY, endZ, 2);
	}

	for (int32_t lx = x - 6; lx <= x + 7; ++lx) {
		for (int32_t lz = z - 6; lz <= z + 7; ++lz) {
			for (int32_t ly = topY - 5; ly <= topY + 2; ++ly) {
				if (ly > 0 && ly < 128 && level->getTile(lx, ly, lz) == Tile::leaves->blockID) {
					if (random->genrand_int32() % 3 == 0) {
						int32_t vlen = 4 + (random->genrand_int32() % 8);
						if (level->isEmptyTile(lx, ly, lz - 1)) this->addHangingVine(level, random, lx, ly, lz - 1, 4, vlen);
						if (level->isEmptyTile(lx, ly, lz + 1)) this->addHangingVine(level, random, lx, ly, lz + 1, 1, vlen);
						if (level->isEmptyTile(lx - 1, ly, lz)) this->addHangingVine(level, random, lx - 1, ly, lz, 2, vlen);
						if (level->isEmptyTile(lx + 1, ly, lz)) this->addHangingVine(level, random, lx + 1, ly, lz, 8, vlen);
					}
				}
			}
		}
	}

	return 1;
}
