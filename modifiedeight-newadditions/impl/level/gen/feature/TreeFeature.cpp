#include <level/gen/feature/TreeFeature.hpp>
#include <level/Level.hpp>
#include <tile/Tile.hpp>
#include <math.h>

TreeFeature::TreeFeature(bool_t update, int32_t meta) : Feature(update){
	this->meta = meta;
}

TreeFeature::~TreeFeature() {
}
bool_t TreeFeature::place(Level* level, Random* random, int32_t x, int32_t y, int32_t z) {
	int32_t v9;		// r7
	bool_t v10; // r9
	int32_t v11;	// r8
	int32_t i;		// r11
	int32_t v13;	// r0
	int32_t j;		// r10
	int32_t v15;	// r0
	int32_t k;		// r7
	int32_t v17;	// r10
	int32_t m;		// r8
	int32_t a5;		// r9
	int32_t v20;	// r3
	int32_t v21;	// r7
	int32_t v22;	// r0
	int32_t v23;	// [sp+14h] [bp-4Ch]
	int32_t v24;	// [sp+14h] [bp-4Ch]
	int32_t v25;	// [sp+18h] [bp-48h]
	int32_t v27;	// [sp+20h] [bp-40h]
	int32_t v28;	// [sp+24h] [bp-3Ch]

	v23 = random->genrand_int32() % (this->meta == 3 ? 9 : 4);
	v25 = v23 + (this->meta == 3 ? 6 : 4);
	if(y <= 0) {
		return 0;
	}
	v28 = y + v25;
	if(y + v25 > 127) {
		return 0;
	}
	v9 = y;
	v10 = 1;
	v27 = y + 1 + v25;
	while(v9 <= v27) {
		if(v9 >= v27 - 2) {
			v11 = 2;
		} else {
			v11 = v9 != y;
		}
		for(i = x - v11; i <= x + v11 && v10; ++i) {
			for(j = z - v11; j <= z + v11 && v10; ++j) {
				if((uint32_t)v9 <= 0x7F) {
					v13 = level->getTile(i, v9, j);
					if(v13) {
						v10 = v13 == Tile::leaves->blockID;
					}
				} else {
					v10 = 0;
				}
			}
		}
		++v9;
	}
	if(!v10) {
		return 0;
	}
	v15 = level->getTile(x, y - 1, z);
	if (this->meta == 4 && (v15 == Tile::water->blockID || v15 == Tile::calmWater->blockID)) {
		int32_t belowWater = level->getTile(x, y - 2, z);
		if (belowWater != Tile::grass->blockID && belowWater != Tile::dirt->blockID) {
			return 0;
		}
	} else if (v15 != Tile::grass->blockID && v15 != Tile::dirt->blockID) {
		return 0;
	}
	int32_t woodMeta = (this->meta == 4) ? 0 : this->meta;
	this->placeBlock(level, x, y - 1, z, Tile::dirt->blockID, 0);
	for(k = y - 3 + v25; k <= v28; ++k) {
		v17 = 1 - (k - v28) / 2;
		for(m = x - v17; m <= x + v17; ++m) {
			a5 = z - v17;
			while(a5 <= z + v17) {
				if(abs(m - x) != v17 || abs(a5 - z) != v17 || (random->genrand_int32() & 1) != 0 && k != v28) {
					int32_t t = level->getTile(m, k, a5);
					if(t == 0 || !Tile::solid[t]) {
						this->placeBlock(level, m, k, a5, Tile::leaves->blockID, woodMeta);
					}
				}
				++a5;
			}
		}
	}
	v21 = 0;
	do {
		v22 = level->getTile(x, v21 + y, z);
		if(!v22 || v22 == Tile::leaves->blockID || v22 == Tile::water->blockID || v22 == Tile::calmWater->blockID) {
			this->placeBlock(level, x, v21 + y, z, Tile::treeTrunk->blockID, woodMeta);
		}
		++v21;
	} while(v21 < v25);

	if (this->meta == 3 || this->meta == 4) {
		int32_t genVer = (level && level->getLevelData()) ? level->getLevelData()->getGeneratorVersion() : -1;
		bool isOldWorld = (genVer == 0 || genVer == 4);
		int32_t canopyStart = y - 3 + v25;
		for (int32_t cy = y; cy < canopyStart; ++cy) {
			if (random->genrand_int32() % 3 == 0 && level->isEmptyTile(x, cy, z - 1) && (!isOldWorld || (x > 3 && x < 252 && z - 1 > 3 && z - 1 < 252))) this->placeBlock(level, x, cy, z - 1, Tile::vine ? Tile::vine->blockID : 0, 4);
			if (random->genrand_int32() % 3 == 0 && level->isEmptyTile(x, cy, z + 1) && (!isOldWorld || (x > 3 && x < 252 && z + 1 > 3 && z + 1 < 252))) this->placeBlock(level, x, cy, z + 1, Tile::vine ? Tile::vine->blockID : 0, 1);
			if (random->genrand_int32() % 3 == 0 && level->isEmptyTile(x - 1, cy, z) && (!isOldWorld || (x - 1 > 3 && x - 1 < 252 && z > 3 && z < 252))) this->placeBlock(level, x - 1, cy, z, Tile::vine ? Tile::vine->blockID : 0, 2);
			if (random->genrand_int32() % 3 == 0 && level->isEmptyTile(x + 1, cy, z) && (!isOldWorld || (x + 1 > 3 && x + 1 < 252 && z > 3 && z < 252))) this->placeBlock(level, x + 1, cy, z, Tile::vine ? Tile::vine->blockID : 0, 8);
		}
		for (int32_t ly = canopyStart; ly <= v28; ++ly) {
			int32_t v17 = 1 - (ly - v28) / 2;
			for (int32_t lx = x - v17; lx <= x + v17; ++lx) {
				for (int32_t lz = z - v17; lz <= z + v17; ++lz) {
					if (level->getTile(lx, ly, lz) == Tile::leaves->blockID) {
						if (random->genrand_int32() % 3 == 0) {
							int32_t vlen = 3 + (random->genrand_int32() % 4);
							if (level->isEmptyTile(lx, ly, lz - 1) && (!isOldWorld || (lx > 3 && lx < 252 && lz - 1 > 3 && lz - 1 < 252))) {
								for (int32_t dy = 0; dy < vlen; ++dy) {
									int32_t vy = ly - dy;
									if (vy <= 0 || !level->isEmptyTile(lx, vy, lz - 1)) break;
									this->placeBlock(level, lx, vy, lz - 1, Tile::vine ? Tile::vine->blockID : 0, 4);
								}
							}
							if (level->isEmptyTile(lx, ly, lz + 1) && (!isOldWorld || (lx > 3 && lx < 252 && lz + 1 > 3 && lz + 1 < 252))) {
								for (int32_t dy = 0; dy < vlen; ++dy) {
									int32_t vy = ly - dy;
									if (vy <= 0 || !level->isEmptyTile(lx, vy, lz + 1)) break;
									this->placeBlock(level, lx, vy, lz + 1, Tile::vine ? Tile::vine->blockID : 0, 1);
								}
							}
							if (level->isEmptyTile(lx - 1, ly, lz) && (!isOldWorld || (lx - 1 > 3 && lx - 1 < 252 && lz > 3 && lz < 252))) {
								for (int32_t dy = 0; dy < vlen; ++dy) {
									int32_t vy = ly - dy;
									if (vy <= 0 || !level->isEmptyTile(lx - 1, vy, lz)) break;
									this->placeBlock(level, lx - 1, vy, lz, Tile::vine ? Tile::vine->blockID : 0, 2);
								}
							}
							if (level->isEmptyTile(lx + 1, ly, lz) && (!isOldWorld || (lx + 1 > 3 && lx + 1 < 252 && lz > 3 && lz < 252))) {
								for (int32_t dy = 0; dy < vlen; ++dy) {
									int32_t vy = ly - dy;
									if (vy <= 0 || !level->isEmptyTile(lx + 1, vy, lz)) break;
									this->placeBlock(level, lx + 1, vy, lz, Tile::vine ? Tile::vine->blockID : 0, 8);
								}
							}
						}
					}
				}
			}
		}
	}
	return 1;
}
