#include <level/gen/feature/LargeCaveFeature.hpp>
#include <math.h>
#include <tile/Tile.hpp>

LargeCaveFeature::~LargeCaveFeature() {
}

void LargeCaveFeature::addFeature(Level* level, int32_t a3, int32_t a4, int32_t targetChunkX, int32_t targetChunkZ, uint8_t* blocks, int32_t a8) {
	Random* p_random = &this->random;
	int caveChance = p_random->genrand_int32() % 7;
	if (caveChance != 0) return;

	int maxCaves = (p_random->genrand_int32() % 15) + 1;
	int caveCount = (p_random->genrand_int32() % maxCaves) + 1;

	for (int i = 0; i < caveCount; ++i) {
		float originX = (float)(a3 * 16 + (p_random->genrand_int32() & 0xF));
		float originY = (float)(p_random->genrand_int32() % 110 + 10);
		if ((p_random->genrand_int32() & 1) == 0) {
			originY = (float)(p_random->genrand_int32() % 45 + 50);
		}
		float originZ = (float)(a4 * 16 + (p_random->genrand_int32() & 0xF));

		int numTunnels = 1;
		if ((p_random->genrand_int32() % 4) == 0) {
			float f = p_random->nextFloat();
			uint32_t tunnelSeed = p_random->genrand_int32();
			LargeCaveFeature::addTunnel(targetChunkX, targetChunkZ, blocks, originX, originY, originZ, (f * 6.0f) + 1.5f, 0.0f, 0.0f, -1, -1, 0.5f, tunnelSeed);
			numTunnels += (p_random->genrand_int32() % 4) + 1;
		}

		for (int t = 0; t < numTunnels; ++t) {
			float yaw = p_random->nextFloat() * 3.14159265f * 2.0f;
			float pitch = (p_random->nextFloat() - 0.5f) / 4.0f;
			float width = p_random->nextFloat() * 2.5f + p_random->nextFloat();
			if ((p_random->genrand_int32() % 8) == 0) {
				width *= (p_random->nextFloat() * p_random->nextFloat() * 3.0f + 1.0f);
			}
			uint32_t tunnelSeed = p_random->genrand_int32();
			LargeCaveFeature::addTunnel(targetChunkX, targetChunkZ, blocks, originX, originY, originZ, width, yaw, pitch, 0, 0, 1.0f, tunnelSeed);
		}
	}
}

void LargeCaveFeature::addTunnel(int32_t targetChunkX, int32_t targetChunkZ, uint8_t* blocks, float curX, float curY, float curZ, float radius, float yaw, float pitch, int32_t step, int32_t totalSteps, float heightScale, uint32_t seed) {
	float targetCenterX = (float)(targetChunkX * 16 + 8);
	float targetCenterZ = (float)(targetChunkZ * 16 + 8);

	float dYaw = 0.0f;
	float dPitch = 0.0f;
	Random random(seed);

	if (totalSteps <= 0) {
		int rangeInBlocks = this->range * 16;
		totalSteps = rangeInBlocks - (random.genrand_int32() % (rangeInBlocks / 4));
	}

	bool isCenterTunnel = false;
	if (step == -1) {
		step = totalSteps / 2;
		isCenterTunnel = true;
	}

	int branchStep = (random.genrand_int32() % (totalSteps / 2)) + (totalSteps / 4);
	bool extraPitchCurve = (random.genrand_int32() % 6) == 0;

	for (; step < totalSteps; ++step) {
		float caveScale = sinf((float)step * 3.14159265f / (float)totalSteps);
		float radX = (radius * caveScale + 1.5f);
		float radY = radX * heightScale;

		float cosPitch = cosf(pitch);
		float sinPitch = sinf(pitch);
		curX += cosf(yaw) * cosPitch;
		curY += sinPitch;
		curZ += sinf(yaw) * cosPitch;

		if (extraPitchCurve) {
			pitch *= 0.92f;
		} else {
			pitch *= 0.70f;
		}
		pitch += dPitch * 0.1f;
		yaw += dYaw * 0.1f;

		dPitch = dPitch * 0.9f + (random.nextFloat() - random.nextFloat()) * random.nextFloat() * 2.0f;
		dYaw = dYaw * 0.75f + (random.nextFloat() - random.nextFloat()) * random.nextFloat() * 4.0f;

		if (!isCenterTunnel && step == branchStep && radius > 1.0f && totalSteps > 0) {
			uint32_t subSeed1 = random.genrand_int32();
			uint32_t subSeed2 = random.genrand_int32();
			float r1 = random.nextFloat() * 0.5f + 0.5f;
			float r2 = random.nextFloat() * 0.5f + 0.5f;
			addTunnel(targetChunkX, targetChunkZ, blocks, curX, curY, curZ, r1, yaw - (3.14159265f * 0.5f), pitch / 3.0f, step, totalSteps, 1.0f, subSeed1);
			addTunnel(targetChunkX, targetChunkZ, blocks, curX, curY, curZ, r2, yaw + (3.14159265f * 0.5f), pitch / 3.0f, step, totalSteps, 1.0f, subSeed2);
			return;
		}

		if (isCenterTunnel || (random.genrand_int32() % 4) != 0) {
			float distSq = (curX - targetCenterX) * (curX - targetCenterX) + (curZ - targetCenterZ) * (curZ - targetCenterZ);
			float maxDist = (float)(totalSteps - step);
			float reach = radX + 2.0f + 16.0f;
			if (distSq - maxDist * maxDist > reach * reach) {
				return;
			}

			if (curX >= targetCenterX - 16.0f - radX * 2.0f &&
				curZ >= targetCenterZ - 16.0f - radX * 2.0f &&
				curX <= targetCenterX + 16.0f + radX * 2.0f &&
				curZ <= targetCenterZ + 16.0f + radX * 2.0f) {

				int minBlockX = (int)floorf(curX - radX) - targetChunkX * 16;
				int maxBlockX = (int)floorf(curX + radX) - targetChunkX * 16;
				int minBlockY = (int)floorf(curY - radY);
				int maxBlockY = (int)floorf(curY + radY);
				int minBlockZ = (int)floorf(curZ - radX) - targetChunkZ * 16;
				int maxBlockZ = (int)floorf(curZ + radX) - targetChunkZ * 16;

				if (minBlockX < 0) minBlockX = 0;
				if (maxBlockX > 16) maxBlockX = 16;
				if (minBlockY < 1) minBlockY = 1;
				if (maxBlockY > 120) maxBlockY = 120;
				if (minBlockZ < 0) minBlockZ = 0;
				if (maxBlockZ > 16) maxBlockZ = 16;

				int checkMinX = minBlockX > 0 ? minBlockX - 1 : 0;
				int checkMaxX = maxBlockX < 16 ? maxBlockX + 1 : 16;
				int checkMinY = minBlockY > 0 ? minBlockY - 1 : 0;
				int checkMaxY = maxBlockY < 127 ? maxBlockY + 1 : 127;
				int checkMinZ = minBlockZ > 0 ? minBlockZ - 1 : 0;
				int checkMaxZ = maxBlockZ < 16 ? maxBlockZ + 1 : 16;

				bool hitsWater = false;
				for (int bx = checkMinX; bx < checkMaxX && !hitsWater; ++bx) {
					for (int bz = checkMinZ; bz < checkMaxZ && !hitsWater; ++bz) {
						int baseIdx = (bx * 16 + bz) * 128;
						for (int by = maxBlockY; by >= minBlockY; --by) {
							uint8_t bId = blocks[baseIdx + by];
							if (bId == Tile::water->blockID || bId == Tile::calmWater->blockID) {
								hitsWater = true;
								break;
							}
						}
					}
				}

				if (!hitsWater) {
					for (int bx = minBlockX; bx < maxBlockX; ++bx) {
						float normX = ((float)(targetChunkX * 16 + bx) + 0.5f - curX) / radX;
						float normXSq = normX * normX;

						for (int bz = minBlockZ; bz < maxBlockZ; ++bz) {
							float normZ = ((float)(targetChunkZ * 16 + bz) + 0.5f - curZ) / radX;
							float normZSq = normZ * normZ;

							if (normXSq + normZSq < 1.0f) {
								int baseIndex = (bx * 16 + bz) * 128;
								bool hitGrass = false;

								for (int by = maxBlockY; by >= minBlockY; --by) {
									float normY = ((float)by + 0.5f - curY) / radY;
									if (normY > -0.7f && normXSq + normZSq + normY * normY < 1.0f) {
										uint8_t curTile = blocks[baseIndex + by];
										uint8_t aboveTile = (by < 127) ? blocks[baseIndex + by + 1] : 0;
										if (curTile == Tile::grass->blockID) {
											hitGrass = true;
										}
										if (curTile != 0 && curTile != Tile::unbreakable->blockID && curTile != Tile::water->blockID && curTile != Tile::calmWater->blockID && curTile != Tile::lava->blockID && curTile != Tile::calmLava->blockID && aboveTile != Tile::water->blockID && aboveTile != Tile::calmWater->blockID) {
											if (by <= 10) {
												blocks[baseIndex + by] = Tile::lava->blockID;
											} else {
												blocks[baseIndex + by] = 0;
												if (hitGrass && blocks[baseIndex + by - 1] == Tile::dirt->blockID) {
													blocks[baseIndex + by - 1] = Tile::grass->blockID;
												}
											}
										}
									}
								}
							}
						}
					}
					if (isCenterTunnel) return;
				}
			}
		}
	}
}
