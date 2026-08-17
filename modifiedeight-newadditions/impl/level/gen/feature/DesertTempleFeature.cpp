#include <level/gen/feature/DesertTempleFeature.hpp>
#include <level/Level.hpp>
#include <level/chunk/LevelChunk.hpp>
#include <level/LightLayer.hpp>
#include <tile/Tile.hpp>
#include <item/ItemInstance.hpp>
#include <item/Item.hpp>
#include <tile/entity/ChestTileEntity.hpp>
#include <math/Mth.hpp>
#include <util/Random.hpp>
#include <math.h>

DesertTempleFeature::DesertTempleFeature(bool_t update)
	: Feature(update) {
}

DesertTempleFeature::~DesertTempleFeature() {
}

static int32_t rotateChestData(int32_t data, int32_t rotation) {
	if (rotation == 1) {
		if (data == 2) return 5;
		if (data == 3) return 4;
		if (data == 4) return 2;
		if (data == 5) return 3;
	} else if (rotation == 2) {
		if (data == 2) return 3;
		if (data == 3) return 2;
		if (data == 4) return 5;
		if (data == 5) return 4;
	} else if (rotation == 3) {
		if (data == 2) return 4;
		if (data == 3) return 5;
		if (data == 4) return 3;
		if (data == 5) return 2;
	}
	return data;
}

void DesertTempleFeature::placeBlockRotated(Level* level, int32_t originX, int32_t originY, int32_t originZ, int32_t lx, int32_t ly, int32_t lz, int32_t blockId, int32_t data, int32_t rotation) {
	int32_t rx = lx;
	int32_t rz = lz;
	if (rotation == 1) {
		rx = 20 - lz;
		rz = lx;
	} else if (rotation == 2) {
		rx = 20 - lx;
		rz = 20 - lz;
	} else if (rotation == 3) {
		rx = lz;
		rz = 20 - lx;
	}

	int32_t wx = originX + rx;
	int32_t wy = originY + ly;
	int32_t wz = originZ + rz;

	if (wy > 0 && wy < 128) {
		if (blockId == Tile::chest->blockID) {
			data = rotateChestData(data, rotation);
		}
		this->placeBlock(level, wx, wy, wz, blockId, data);
	}
}

void DesertTempleFeature::fillLoot(Level* level, Random* random, int32_t x, int32_t y, int32_t z) {
	ChestTileEntity* te = (ChestTileEntity*)level->getTileEntity(x, y, z);
	if (!te) {
		te = new ChestTileEntity();
		level->setTileEntity(x, y, z, te);
	}

	int32_t numItems = 4 + (random->genrand_int32() % 5);
	for (int32_t i = 0; i < numItems; ++i) {
		int32_t slot = random->genrand_int32() % 27;
		int32_t itemType = random->genrand_int32() % 11;
		ItemInstance inst(Item::ironIngot, 2 + (random->genrand_int32() % 5), 0);

		if (itemType <= 2) {
			inst = ItemInstance(Item::ironIngot, 2 + (random->genrand_int32() % 6), 0);
		} else if (itemType == 3) {
			inst = ItemInstance(Item::emerald, 1 + (random->genrand_int32() % 2), 0);
		} else if (itemType == 4) {
			inst = ItemInstance(Item::goldIngot, 2 + (random->genrand_int32() % 5), 0);
		} else if (itemType == 5) {
			inst = ItemInstance(Tile::web ? Tile::web->blockID : Tile::sandStone->blockID, 1 + (random->genrand_int32() % 4), 0);
		} else if (itemType == 6) {
			inst = ItemInstance(Item::stick, 2 + (random->genrand_int32() % 5), 0);
		} else if (itemType == 7) {
			inst = ItemInstance(Item::bone, 2 + (random->genrand_int32() % 5), 0);
		} else if (itemType == 8) {
			inst = ItemInstance(Item::bread, 1 + (random->genrand_int32() % 3), 0);
		} else if (itemType == 9) {
			inst = ItemInstance(Tile::tnt, 1 + (random->genrand_int32() % 2), 0);
		} else if (itemType == 10) {
			inst = ItemInstance(Item::dye_powder, 2 + (random->genrand_int32() % 5), 0);
		}
		te->setItem(slot, &inst);
	}
}

bool_t DesertTempleFeature::place(Level* level, Random* random, int32_t x, int32_t y, int32_t z) {
	if (y < 60 || y > 110) return 0;

	int32_t minH = 255;
	int32_t maxH = 0;
	for (int32_t cx = x; cx <= x + 20; cx += 4) {
		for (int32_t cz = z; cz <= z + 20; cz += 4) {
			int32_t hm = level->getHeightmap(cx, cz);
			if (hm <= 0) hm = 65;
			int32_t gt = level->getTile(cx, hm - 1, cz);
			if (gt == Tile::water->blockID || gt == Tile::calmWater->blockID) {
				return 0;
			}
			if (hm < minH) minH = hm;
			if (hm > maxH) maxH = hm;
		}
	}
	if (maxH - minH > 18) {
		return 0;
	}

	int32_t rotation = random->genrand_int32() % 4;

	for (int32_t lx = 0; lx < 21; ++lx) {
		for (int32_t lz = 0; lz < 21; ++lz) {
			int32_t rx = lx;
			int32_t rz = lz;
			if (rotation == 1) { rx = 20 - lz; rz = lx; }
			else if (rotation == 2) { rx = 20 - lx; rz = 20 - lz; }
			else if (rotation == 3) { rx = lz; rz = 20 - lx; }
			int32_t wx = x + rx;
			int32_t wz = z + rz;
			for (int32_t fy = y - 1; fy >= y - 12; --fy) {
				if (fy <= 0) break;
				int32_t cur = level->getTile(wx, fy, wz);
				if (cur != 0 && cur != Tile::water->blockID && cur != Tile::calmWater->blockID && cur != Tile::tallgrass->blockID && cur != Tile::flower->blockID) break;
				this->placeBlock(level, wx, fy, wz, Tile::sandStone->blockID, 0);
			}
		}
	}

	for (int32_t ly = 0; ly <= 14; ++ly) {
		for (int32_t lx = 0; lx < 21; ++lx) {
			for (int32_t lz = 0; lz < 21; ++lz) {
				this->placeBlockRotated(level, x, y, z, lx, ly, lz, 0, 0, rotation);
			}
		}
	}

	for (int32_t lx = 0; lx < 21; ++lx) {
		for (int32_t lz = 0; lz < 21; ++lz) {
			this->placeBlockRotated(level, x, y, z, lx, 0, lz, Tile::sandStone->blockID, 2, rotation);
		}
	}

	for (int32_t step = 0; step < 9; ++step) {
		int32_t minC = step;
		int32_t maxC = 20 - step;
		int32_t curY = 1 + step;
		for (int32_t lx = minC; lx <= maxC; ++lx) {
			for (int32_t lz = minC; lz <= maxC; ++lz) {
				bool isWall = (lx == minC || lx == maxC || lz == minC || lz == maxC || step >= 7);
				int32_t blockId = isWall ? Tile::sandStone->blockID : 0;
				int32_t meta = 0;
				if (blockId != 0) {
					this->placeBlockRotated(level, x, y, z, lx, curY, lz, blockId, meta, rotation);
				}
			}
		}
	}

	for (int32_t t = 0; t < 2; ++t) {
		int32_t minTX = (t == 0) ? 0 : 16;
		int32_t maxTX = (t == 0) ? 4 : 20;
		int32_t minTZ = 0;
		int32_t maxTZ = 4;

		for (int32_t ty = 1; ty <= 13; ++ty) {
			for (int32_t tx = minTX; tx <= maxTX; ++tx) {
				for (int32_t tz = minTZ; tz <= maxTZ; ++tz) {
					bool isCorner = (tx == minTX || tx == maxTX) && (tz == minTZ || tz == maxTZ);
					bool isTowerWall = (tx == minTX || tx == maxTX || tz == minTZ || tz == maxTZ || ty == 13);
					int32_t bId = isTowerWall ? Tile::sandStone->blockID : 0;
					int32_t data = isCorner ? 2 : 0;

					if (tz == 0 && ty >= 3 && ty <= 9 && tx >= minTX + 1 && tx <= maxTX - 1) {
						if (ty == 6 && tx == minTX + 2) {
							bId = Tile::cloth->blockID;
							data = 11;
						} else if (ty == 3 || ty == 4 || ty == 8 || ty == 9 || tx == minTX + 1 || tx == maxTX - 1) {
							bId = Tile::cloth->blockID;
							data = 1;
						}
					}
					if (tz == 2 && (tx == minTX || tx == maxTX) && ty >= 5 && ty <= 7) {
						if (ty == 6) {
							bId = Tile::cloth->blockID;
							data = 11;
						} else {
							bId = Tile::cloth->blockID;
							data = 1;
						}
					}
					if (bId != 0) {
						this->placeBlockRotated(level, x, y, z, tx, ty, tz, bId, data, rotation);
					}
				}
			}
		}

		for (int32_t tx = minTX; tx <= maxTX; ++tx) {
			for (int32_t tz = minTZ; tz <= maxTZ; ++tz) {
				if ((tx + tz) % 2 == 0) {
					this->placeBlockRotated(level, x, y, z, tx, 14, tz, Tile::sandStone->blockID, 2, rotation);
				}
			}
		}
	}

	for (int32_t dy = 1; dy <= 3; ++dy) {
		this->placeBlockRotated(level, x, y, z, 2, dy, 4, 0, 0, rotation);
		this->placeBlockRotated(level, x, y, z, 3, dy, 4, 0, 0, rotation);
		this->placeBlockRotated(level, x, y, z, 17, dy, 4, 0, 0, rotation);
		this->placeBlockRotated(level, x, y, z, 18, dy, 4, 0, 0, rotation);
	}

	for (int32_t ez = 0; ez <= 3; ++ez) {
		for (int32_t ey = 0; ey <= 4; ++ey) {
			this->placeBlockRotated(level, x, y, z, 8, ey, ez, Tile::sandStone->blockID, 2, rotation);
			this->placeBlockRotated(level, x, y, z, 12, ey, ez, Tile::sandStone->blockID, 2, rotation);
		}
		for (int32_t ex = 9; ex <= 11; ++ex) {
			this->placeBlockRotated(level, x, y, z, ex, 4, ez, Tile::sandStone->blockID, 2, rotation);
		}
		for (int32_t ey = 1; ey <= 3; ++ey) {
			for (int32_t ex = 9; ex <= 11; ++ex) {
				this->placeBlockRotated(level, x, y, z, ex, ey, ez, 0, 0, rotation);
			}
		}
	}

	for (int32_t sx = 9; sx <= 11; ++sx) {
		for (int32_t sz = 9; sz <= 11; ++sz) {
			int32_t wColor = (sx == 10 && sz == 10) ? 11 : 1;
			this->placeBlockRotated(level, x, y, z, sx, 0, sz, Tile::cloth->blockID, wColor, rotation);
		}
	}

	for (int32_t sy = -1; sy >= -14; --sy) {
		for (int32_t sx = 8; sx <= 12; ++sx) {
			for (int32_t sz = 8; sz <= 12; ++sz) {
				bool isShaftWall = (sx == 8 || sx == 12 || sz == 8 || sz == 12);
				if (isShaftWall) {
					this->placeBlockRotated(level, x, y, z, sx, sy, sz, Tile::sandStone->blockID, 2, rotation);
				} else {
					this->placeBlockRotated(level, x, y, z, sx, sy, sz, 0, 0, rotation);
				}
			}
		}
	}

	for (int32_t cy = -11; cy >= -15; --cy) {
		for (int32_t cx = 7; cx <= 13; ++cx) {
			for (int32_t cz = 7; cz <= 13; ++cz) {
				bool isChamberWall = (cx == 7 || cx == 13 || cz == 7 || cz == 13 || cy == -15);
				int32_t bId = isChamberWall ? Tile::sandStone->blockID : 0;
				int32_t data = (isChamberWall && cy == -15) ? 2 : 0;
				if (bId != 0 || cy == -15) {
					this->placeBlockRotated(level, x, y, z, cx, cy, cz, bId, data, rotation);
				}
			}
		}
	}

	int32_t chestCoords[4][3] = {
		{10, 8, 3},
		{10, 12, 2},
		{8, 10, 5},
		{12, 10, 4}
	};

	for (int32_t i = 0; i < 4; ++i) {
		int32_t cx = chestCoords[i][0];
		int32_t cz = chestCoords[i][1];
		int32_t dir = chestCoords[i][2];

		for (int32_t ay = -13; ay <= -11; ++ay) {
			this->placeBlockRotated(level, x, y, z, cx, ay, cz, 0, 0, rotation);
		}

		this->placeBlockRotated(level, x, y, z, cx, -14, cz, Tile::chest->blockID, dir, rotation);

		int32_t rx = cx;
		int32_t rz = cz;
		if (rotation == 1) { rx = 20 - cz; rz = cx; }
		else if (rotation == 2) { rx = 20 - cx; rz = 20 - cz; }
		else if (rotation == 3) { rx = cz; rz = 20 - cx; }

		this->fillLoot(level, random, x + rx, y - 14, z + rz);
	}

	for (int32_t cx = (x - 2) >> 4; cx <= (x + 23) >> 4; ++cx) {
		for (int32_t cz = (z - 2) >> 4; cz <= (z + 23) >> 4; ++cz) {
			LevelChunk* c = level->getChunk(cx, cz);
			if (c) c->recalcHeightmap();
		}
	}
	level->updateLight(LightLayer::Sky, x - 2, y - 16, z - 2, x + 23, y + 16, z + 23, 1);
	level->updateLight(LightLayer::Block, x - 2, y - 16, z - 2, x + 23, y + 16, z + 23, 1);

	return 1;
}
