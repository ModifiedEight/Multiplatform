#include <level/gen/feature/EquatorialTreeFeature.hpp>
#include <level/Level.hpp>
#include <tile/Tile.hpp>
#include <util/Random.hpp>

EquatorialTreeFeature::EquatorialTreeFeature(bool_t update)
	: Feature(update) {
}

EquatorialTreeFeature::~EquatorialTreeFeature() {
}

static void placeLeafBlob(Level* level, Random* rand, int32_t cx, int32_t cy, int32_t cz, int32_t rx, int32_t ry, int32_t rz) {
	for (int32_t ox = -rx; ox <= rx; ++ox) {
		for (int32_t oy = -ry; oy <= ry; ++oy) {
			for (int32_t oz = -rz; oz <= rz; ++oz) {
				float distSq = (float)(ox * ox) / (float)(rx * rx + 0.1f) +
				               (float)(oy * oy) / (float)(ry * ry + 0.1f) +
				               (float)(oz * oz) / (float)(rz * rz + 0.1f);
				if (distSq <= 1.05f) {
					if (ox * ox + oz * oz == rx * rx + rz * rz && rand->genrand_int32() % 3 == 0) continue;
					int32_t bx = cx + ox, by = cy + oy, bz = cz + oz;
					if (by > 0 && by < 127 && (level->isEmptyTile(bx, by, bz) || level->getTile(bx, by, bz) == Tile::vine->blockID)) {
						level->setTileAndData(bx, by, bz, Tile::leaves->blockID, 3, 2);
						if (oy <= -ry + 1 && (rand->genrand_int32() % 3 == 0) && Tile::vine) {
							if (level->isEmptyTile(bx, by - 1, bz)) {
								level->setTileAndData(bx, by - 1, bz, Tile::vine->blockID, 15, 2);
							}
						}
					}
				}
			}
		}
	}
}

static void placeRootStilt(Level* level, Random* rand, int32_t startX, int32_t startY, int32_t startZ, int32_t dirX, int32_t dirZ, int32_t maxDist) {
	int32_t cx = startX, cy = startY, cz = startZ;
	for (int step = 0; step < maxDist; ++step) {
		if (step > 0) {
			if (rand->genrand_int32() % 2 == 0) cx += dirX;
			if (rand->genrand_int32() % 2 == 0) cz += dirZ;
			cy -= 1;
		}
		if (cy < 1) break;
		int32_t cur = level->getTile(cx, cy, cz);
		if (cur == Tile::grass->blockID || cur == Tile::dirt->blockID || cur == Tile::sand->blockID || cur == Tile::rock->blockID) {
			break;
		}
		level->setTileAndData(cx, cy, cz, Tile::treeTrunk->blockID, 3, 2);
	}
	while (cy > 1) {
		int32_t below = level->getTile(cx, cy - 1, cz);
		if (below == Tile::grass->blockID || below == Tile::dirt->blockID || below == Tile::sand->blockID || below == Tile::rock->blockID) {
			break;
		}
		cy--;
		level->setTileAndData(cx, cy, cz, Tile::treeTrunk->blockID, 3, 2);
	}
}

bool_t EquatorialTreeFeature::place(Level* level, Random* rand, int32_t x, int32_t y, int32_t z) {
	int32_t groundY = y;
	while (groundY > 1 && (level->getTile(x, groundY - 1, z) == Tile::water->blockID || 
	                       level->getTile(x, groundY - 1, z) == Tile::calmWater->blockID || 
	                       level->isEmptyTile(x, groundY - 1, z) ||
	                       level->getTile(x, groundY - 1, z) == Tile::tallgrass->blockID ||
	                       level->getTile(x, groundY - 1, z) == Tile::vine->blockID ||
	                       level->getTile(x, groundY - 1, z) == Tile::leaves->blockID)) {
		groundY--;
	}

	int32_t ground = level->getTile(x, groundY - 1, z);
	if (ground != Tile::grass->blockID && ground != Tile::dirt->blockID && ground != Tile::sand->blockID && ground != Tile::rock->blockID) {
		return 0;
	}

	int archetype = rand->genrand_int32() % 4;
	int32_t trunkHeight;
	int32_t rootHubHeight;
	int32_t rootSpan;

	if (archetype == 0) {
		trunkHeight = 8 + (rand->genrand_int32() % 5);
		rootHubHeight = 1;
		rootSpan = 1;
	} else if (archetype == 1) {
		trunkHeight = 12 + (rand->genrand_int32() % 6);
		rootHubHeight = 1 + (rand->genrand_int32() % 2);
		rootSpan = 2;
	} else if (archetype == 2) {
		trunkHeight = 16 + (rand->genrand_int32() % 7);
		rootHubHeight = 2;
		rootSpan = 2;
	} else {
		trunkHeight = 19 + (rand->genrand_int32() % 8);
		rootHubHeight = 2 + (rand->genrand_int32() % 2);
		rootSpan = 3;
	}

	if (groundY + trunkHeight + 6 >= 127) return 0;

	int32_t hubY = groundY + rootHubHeight;
	for (int32_t py = groundY; py <= hubY; ++py) {
		level->setTileAndData(x, py, z, Tile::treeTrunk->blockID, 3, 2);
	}

	int rootDirs[8][2] = {{1,0},{-1,0},{0,1},{0,-1},{1,1},{-1,1},{1,-1},{-1,-1}};
	int numRoots = 2 + (rand->genrand_int32() % 3);
	for (int r = 0; r < numRoots; ++r) {
		int d = (r * 3 + (rand->genrand_int32() % 2)) % 8;
		int rStartY = hubY;
		placeRootStilt(level, rand, x, rStartY, z, rootDirs[d][0], rootDirs[d][1], rootSpan);
	}

	int32_t curX = x, curZ = z;
	int32_t curveDirX = (rand->genrand_int32() % 3) - 1;
	int32_t curveDirZ = (rand->genrand_int32() % 3) - 1;

	for (int32_t dy = rootHubHeight; dy <= trunkHeight; ++dy) {
		int32_t cy = groundY + dy;
		if (dy > rootHubHeight + 2 && (dy % 4 == 0)) {
			curX += curveDirX;
			curZ += curveDirZ;
		}
		level->setTileAndData(curX, cy, curZ, Tile::treeTrunk->blockID, 3, 2);

		if (Tile::vine && (rand->genrand_int32() % 2 == 0) && dy > rootHubHeight) {
			for (int side = 0; side < 4; ++side) {
				int vx = curX + (side == 0 ? 1 : (side == 1 ? -1 : 0));
				int vz = curZ + (side == 2 ? 1 : (side == 3 ? -1 : 0));
				if (level->isEmptyTile(vx, cy, vz)) {
					int vdata = (side == 0 ? 8 : (side == 1 ? 2 : (side == 2 ? 1 : 4)));
					level->setTileAndData(vx, cy, vz, Tile::vine->blockID, vdata, 2);
				}
			}
		}
	}

	int numBranches = (archetype == 0 ? 2 : (archetype == 1 ? 3 : (archetype == 2 ? 4 : 5))) + (rand->genrand_int32() % 2);
	for (int b = 0; b < numBranches; ++b) {
		int32_t bStartY = groundY + trunkHeight - (rand->genrand_int32() % (trunkHeight / 3 + 2));
		int32_t bDirX = rootDirs[(b * 8 / numBranches + (rand->genrand_int32() % 2)) % 8][0];
		int32_t bDirZ = rootDirs[(b * 8 / numBranches + (rand->genrand_int32() % 2)) % 8][1];
		int32_t bLen = 3 + (rand->genrand_int32() % (archetype + 3));

		int32_t bx = curX, by = bStartY, bz = curZ;
		for (int step = 1; step <= bLen; ++step) {
			bx += bDirX;
			bz += bDirZ;
			if (step % 2 == 1 && step > 1) by += 1;
			if (by > 0 && by < 127) {
				level->setTileAndData(bx, by, bz, Tile::treeTrunk->blockID, 3, 2);
			}
		}

		int clusterRx = 2 + (rand->genrand_int32() % 2);
		int clusterRy = 1 + (rand->genrand_int32() % 2);
		int clusterRz = 2 + (rand->genrand_int32() % 2);
		placeLeafBlob(level, rand, bx, by + 1, bz, clusterRx, clusterRy, clusterRz);
	}

	int topRx = (archetype >= 2 ? 3 : 2) + (rand->genrand_int32() % 2);
	int topRy = 2;
	int topRz = (archetype >= 2 ? 3 : 2) + (rand->genrand_int32() % 2);
	placeLeafBlob(level, rand, curX, groundY + trunkHeight + 1, curZ, topRx, topRy, topRz);

	return 1;
}
