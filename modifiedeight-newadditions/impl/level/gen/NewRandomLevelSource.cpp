#include <level/gen/NewRandomLevelSource.hpp>
#include <level/BiomeSource.hpp>
#include <level/LevelHeight.hpp>
#include <level/Level.hpp>
#include <level/MobSpawner.hpp>
#include <level/chunk/LevelChunk.hpp>
#include <level/gen/feature/CactusFeature.hpp>
#include <level/gen/feature/ClayFeature.hpp>
#include <level/gen/feature/FlowerFeature.hpp>
#include <level/gen/feature/DoublePlantFeature.hpp>
#include <level/gen/feature/TallgrassFeature.hpp>
#include <level/gen/feature/MegaJungleTreeFeature.hpp>
#include <level/gen/feature/JungleBushFeature.hpp>
#include <level/gen/feature/MelonFeature.hpp>
#include <level/gen/feature/VineFeature.hpp>
#include <level/gen/feature/VillageFeature.hpp>
#include <level/gen/feature/DesertTempleFeature.hpp>
#include <level/gen/feature/OreFeature.hpp>
#include <level/gen/feature/ReedsFeature.hpp>
#include <level/gen/feature/SpringFeature.hpp>
#include <level/gen/feature/WaterLilyFeature.hpp>
#include <entity/Frog.hpp>
#include <math.h>
#include <tile/HeavyTile.hpp>
#include <tile/material/Material.hpp>
#include <utils.h>
#include <string.h>

NewRandomLevelSource::NewRandomLevelSource(struct Level* a2, int32_t a3, int32_t a4, bool a5) 
	: random(a3),
	octave16noise_1(&this->random, 16)
	, octave16noise_2(&this->random, 16)
	, octave8noise_1(&this->random, 8)
	, octave4noise_1(&this->random, 4)
	, octave4noise_2(&this->random, 4)
	, octave10noise_1(&this->random, 10)
	, octave16noise_3(&this->random, 16)
	, treeNoise(&this->random, 8) {
	this->field_4 = 0;

	this->level = a2;
	this->field_72CC = a5;
	this->interpolationNoises = 0;
	this->upperInterpolationNoises = 0;
	this->lowerInterpolationNoises = 0;
	this->biomeNoises = 0;
	this->depthNoises = 0;
	this->field_7EE8 = 0;
	this->field_7EEC = 0;
	for(int32_t v8 = 0; v8 != 32; ++v8) {
		for(int32_t i = 0; i != 32; ++i) {
			this->field_9E0[v8 * 32 + i] = 0;
		}
	}
	this->field_72D0 = new float[1024];
	Random v12 = this->random;
	printf("random.get : %d\n", v12.genrand_int32() >> 1);
}
void NewRandomLevelSource::buildSurfaces(int32_t a2, int32_t a3, uint8_t* a4, struct Biome** a5) {
	float v5;			 
	float v8;			 
	int32_t v9;			 
	float v10;			 
	int32_t v11;		 
	int32_t v12;		 
	float f;			 
	float v14;			 
	float v15;			 
	float f1;			 
	float v17;			 
	float v18;			 
	float f2;			 
	int32_t fillerBlock; 
	int32_t topBlock;	 
	uint8_t* v22;		 
	int32_t v23;		 
	int32_t blockID_low; 
	int8_t v25;			 
	int32_t v26;		 
	int32_t v27;		 
	int32_t v28;		 
	int32_t v29;		 
	float* v30;			 
	Biome* v31;			 
	int32_t v32;		 
	int32_t v33;		 
	Biome** v34;		 

	v5 = (float)(16 * a2);
	v8 = (float)(16 * a3);
	this->octave4noise_1.getRegion(this->field_72D4, v5, v8, 0.0, 16, 16, 1, 0.03125, 0.03125, 1.0);
	this->octave4noise_1.getRegion(this->field_76D4, v5, 109.01, v8, 16, 1, 16, 0.03125, 1.0, 0.03125);
	this->octave4noise_2.getRegion(this->field_7AD4, v5, v8, 0.0, 16, 16, 1, 0.0625, 0.0625, 0.0625);

	
	
	
	for(int blockX = 0; blockX < 16; ++blockX) {
		for(int blockZ = 0; blockZ < 16; ++blockZ) {
			Biome* biome = a5[blockX * 16 + blockZ];
			bool z = this->field_72D4[blockX * 16 + blockZ] + (this->random.nextFloat() * 0.2f) > 0.0f;
			bool z2 = this->field_76D4[blockX * 16 + blockZ] + (this->random.nextFloat() * 0.2f) > 3.0f;
			int nextFloat = (int)((this->field_7AD4[blockX * 16 + blockZ] / 3.0f) + 3.0f + (this->random.nextFloat() * 0.25f));
			int i = -1;
			int b = biome->topBlock;
			int b2 = biome->fillerBlock;
			for(int blockY = 127; blockY >= 0; --blockY) {
				int index = (blockX * 16 + blockZ) * 128 + blockY;
				if((this->random.genrand_int32() % 5) >= blockY) {
					a4[index] = Tile::unbreakable->blockID;
				} else {
					int b3 = a4[index];
					if(b3 == 0) {
						i = -1;
					} else if(b3 == Tile::rock->blockID) {
						if(i == -1) {
							if(nextFloat > 0) {
								if(blockY >= 61 && blockY <= 63) {
									b = biome->topBlock;
									b2 = biome->fillerBlock;
									if(z2) {
										b = 0;
										b2 = Tile::gravel->blockID;
									}
									if(z) {
										b = Tile::sand->blockID;
										b2 = Tile::sand->blockID;
									}
								}

							} else {
								b = 0;
								b2 = Tile::rock->blockID;
							}

							if (blockY < 64 && b == 0) {
								b = Tile::calmWater->blockID;
							}
							i = nextFloat;
							if (blockY >= 63) {
								a4[index] = b;
							} else {
								a4[index] = b2;
							}
						} else if (i > 0) {
							--i;
							a4[index] = b2;
							if (i == 0 && b2 == Tile::sand->blockID) {
								i = (this->random.genrand_int32() % 4);
								b2 = Tile::sandStone->blockID;
							}
						}

					}
				}
			}
			if (biome == Biome::equatorialRainforest) {
				for (int blockY = 64; blockY >= 61; --blockY) {
					int index = (blockX * 16 + blockZ) * 128 + blockY;
					if (blockY >= 63 && a4[index] == Tile::grass->blockID) {
						if (((blockX * 7 + blockZ * 13) % 10) < 8) {
							a4[index] = Tile::calmWater->blockID;
							if (blockY > 0) a4[index - 1] = Tile::dirt->blockID;
						}
					} else if (blockY <= 63 && a4[index] == 0) {
						a4[index] = Tile::calmWater->blockID;
					}
				}
			}
		}
	}
}
void NewRandomLevelSource::calcWaterDepths(struct ChunkSource* a2, int32_t a3, int32_t a4) {
	Level* level;  
	int32_t x;	   
	int32_t z;	   
	int32_t y;	   
	int32_t v8;	   
	int32_t v9;	   
	int32_t v10;   
	int32_t i;	   
	int32_t v12;   
	int32_t v13;   
	int32_t v14;   
	int32_t v15;   
	int32_t v16;   
	int32_t v17;   
	int32_t v18;   
	int32_t xsub2; 
	uint32_t v20;  
	int32_t v21;   
	uint32_t v22;  

	level = this->level;
	x = 16 * a3 + 8;
	v21 = 16 * a4;
	v17 = 16;
	do {
		z = v21;
		v15 = 0;
		xsub2 = x - 2;
		y = level->getSeaLevel();
		do {
			v8 = z + 7;
			if(level->getHeightmap(x - 1, z + 7) <= 0 && (level->getHeightmap(xsub2, z + 7) > 0 || level->getHeightmap(x, z + 7) > 0 || level->getHeightmap(x - 1, z + 6) > 0 || level->getHeightmap(x - 1, z + 8) > 0) && (level->getTile(xsub2, y, z + 7) == Tile::calmWater->blockID && level->getData(xsub2, y, z + 7) <= 6 || level->getTile(x, y, z + 7) == Tile::calmWater->blockID && level->getData(x, y, z + 7) <= 6 || level->getTile(x - 1, y, z + 6) == Tile::calmWater->blockID && level->getData(x - 1, y, z + 6) <= 6 || level->getTile(x - 1, y, z + 8) == Tile::calmWater->blockID && level->getData(x - 1, y, z + 8) <= 6)) {
				for(i = -5; i != 6; ++i) {
					v14 = -5;
					v22 = abs(i); 
					v18 = x - 1 + i;
					do {
						v9 = v14;
						if(v14 < 0) {
							v9 = -v14;
						}
						v20 = v22 + v9;
						if((int32_t)(v22 + v9) <= 5) {
							v16 = v8 + v14;
							if(level->getTile(v18, y, v8 + v14) == Tile::calmWater->blockID) {
								v10 = level->getData(v18, y, v16);
								if(v10 <= 6 && v10 < (int32_t)(6 - v20)) {
									level->setData(v18, y, v16, 6 - v20, 4);
								}
							}
						}
						++v14;
					} while(v14 != 6);
				}
				v12 = 0;
				level->setTileAndDataNoUpdate(x - 1, y, z + 7, Tile::calmWater->blockID, 7);
				while(v12 < y) {
					v13 = v12++;
					level->setTileAndDataNoUpdate(x - 1, v13, z + 7, Tile::calmWater->blockID, 8);
				}
			}
			++z;
			++v15;
		} while(v15 != 16);
		++x;
		--v17;
	} while(v17);
}
float* NewRandomLevelSource::getHeights(float* heights, int32_t chunkX, int32_t chunkY, int32_t chunkZ, int32_t scaleX, int32_t scaleY, int32_t scaleZ) {
	float* rainfallNoises;	  
	float* temperatureNoises; 
	float v14;				  
	int32_t v15;			  
	int32_t v16;			  
	int32_t v17;			  
	int32_t v18;			  
	int32_t v19;			  
	int32_t v20;			  
	int32_t i;				  
	float v22;				  
	int32_t v23;			  
	float v24;				  
	float v25;				  
	float v26;				  
	float v27;				  
	float v28;				  
	int32_t v29;			  
	int32_t v30;			  
	float v31;				  
	float v32;				  
	float v33;				  
	float v34;				  
	float v35;				  
	float v36;				  
	float* v37;				  
	int32_t v39;			  
	int32_t v40;			  
	float* v42;				  
	float* v43;				  

	rainfallNoises = this->level->getBiomeSource()->rainfallNoises;
	temperatureNoises = this->level->getBiomeSource()->temperatureNoises;
	this->biomeNoises = this->octave10noise_1.getRegion(this->biomeNoises, chunkX, chunkZ, scaleX, scaleZ, 1.121, 1.121, 0.5);
	v14 = (float)chunkY;
	this->depthNoises = this->octave16noise_3.getRegion(this->depthNoises, chunkX, chunkZ, scaleX, scaleZ, 200.0, 200.0, 0.5);
	this->interpolationNoises = this->octave8noise_1.getRegion(this->interpolationNoises, (float)chunkX, v14, (float)chunkZ, scaleX, scaleY, scaleZ, 8.5552, 4.2776, 8.5552);
	this->upperInterpolationNoises = this->octave16noise_1.getRegion(this->upperInterpolationNoises, (float)chunkX, v14, (float)chunkZ, scaleX, scaleY, scaleZ, 684.41, 684.41, 684.41);
	this->lowerInterpolationNoises = this->octave16noise_2.getRegion(this->lowerInterpolationNoises, (float)chunkX, v14, (float)chunkZ, scaleX, scaleY, scaleZ, 684.41, 684.41, 684.41);
	v40 = scaleY & ~(scaleY >> 31);
	v15 = 17 * (16 / scaleX / 2);
	v16 = 0;
	v17 = 0;
	v39 = 0;
	v42 = &rainfallNoises[v15];
	v43 = &temperatureNoises[v15];
	v18 = 0;
	while(v17 < scaleX) {
		v19 = 0;
		v20 = v18;
		for(i = 0; i < scaleZ; ++i) {
			v22 = 1.0 - (float)(v43[v16 + v19] * v42[v16 + v19]);
			v23 = i + v39;
			v24 = this->depthNoises[v23] / 8000.0;
			v25 = (float)((float)(this->biomeNoises[v23] + 256.0) * 0.0019531) * (float)(1.0 - (float)((float)(v22 * v22) * (float)(v22 * v22)));
			if(v25 > 1.0) {
				v25 = 1.0;
			}
			if(v24 < 0.0) {
				v24 = -(float)(v24 * 0.3);
			}
			v26 = (float)(v24 * 3.0) - 2.0;
			if(v26 >= 0.0) {
				if(v26 > 1.0) {
					v26 = 1.0;
				}
				v28 = v26 * 0.125;
				if(v25 < 0.0) {
					v25 = 0.0;
				}
			} else {
				v27 = v26 * 0.5;
				if(v27 < -1.0) {
					v27 = -1.0;
				}
				v25 = 0.0;
				v28 = (float)(v27 / 1.4) * 0.5;
			}
			v29 = v20;
			v30 = 0;
			v31 = v25 + 0.5;
			v32 = (float)((float)((float)(v28 * (float)scaleY) * 0.0625) * 4.0) + (float)((float)scaleY * 0.5);
			while(v30 < scaleY) {
				v33 = (float)((float)((float)v30 - v32) * 12.0) / v31;
				v34 = (float)((float)(this->interpolationNoises[v29] / 10.0) + 1.0) * 0.5;
				if(v33 < 0.0) {
					v33 = v33 * 4.0;
				}
				v35 = this->upperInterpolationNoises[v29] * 0.0019531;
				if(v34 >= 0.0) {
					if(v34 > 1.0) {
						v35 = this->lowerInterpolationNoises[v29] * 0.0019531;
					} else {
						v35 = v35 + (float)((float)((float)(this->lowerInterpolationNoises[v29] * 0.0019531) - v35) * v34);
					}
				}
				v36 = v35 - v33;
				if(scaleY - 3 <= v30) {
					v36 = (float)((float)((float)(4 - scaleY + v30) / 3.0) * -10.0) + (float)(v36 * (float)(1.0 - (float)((float)(4 - scaleY + v30) / 3.0)));
				}
				++v30;
				v37 = &heights[v29++];
				*v37 = v36;
			}
			v20 += v40;
			v19 += (16 / scaleX);
		}
		++v17;
		v18 += (scaleZ & ~(scaleZ >> 31)) * v40;
		v16 += ((16 / scaleX) << 6) / 4;
		v39 += scaleZ & ~(scaleZ >> 31);
	}
	return heights;
}

void NewRandomLevelSource::prepareHeights(int32_t a2, int32_t a3, uint8_t* a4, void* a5, float* a6) {
	
	
	float* heights;	  
	float* v7;		  
	float* v8;		  
	float* v9;		  
	float v10;		  
	int v11;		  
	float v12;		  
	int v13;		  
	float v14;		  
	uint8_t* v15;	  
	float v16;		  
	int v17;		  
	int v18;		  
	int v19;		  
	int v20;		  
	uint8_t* v21;	  
	float v22;		  
	int v23;		  
	int v24;		  
	uint8_t* v25;	  
	float v26;		  
	int v27;		  
	int v29;		  
	int v30;		  
	int v31;		  
	float v32;		  
	float v33;		  
	int v34;		  
	float v35;		  
	float v36;		  
	int v37;		  
	float v38;		  
	float v39;		  
	float v40;		  
	float v41;		  
	float* v42;		  
	unsigned int v43; 
	int v44;		  
	int v45;		  
	int v46;		  

	this->field_72D0 = heights = this->getHeights(this->field_72D0, 4 * a2, 0, 4 * a3, 5, 17, 5);
	v34 = 0;
	while(2) {
		v44 = 85 * v34;
		v45 = 85 * v34 + 340;
		v42 = a6;
		v46 = 0;
		while(2) {
			v43 = 0;
			while(2) {
				v7 = &heights[(v44 + v43) / 4];
				v37 = 0;
				v32 = *v7;
				v33 = heights[(v44 + v43 + 68) / 4];
				v8 = &heights[v43 / 4 + v45 / 4];
				v35 = *v8;
				v36 = heights[(v45 + v43 + 68)/4];
				v31 = 2 * v43;
				v38 = 0.125 * (float)(v7[1] - *v7);
				v39 = 0.125 * (float)(v7[18] - v33);
				v40 = 0.125 * (float)(v8[1] - *v8);
				v41 = 0.125 * (float)(v8[18] - v36);
				do {
					v9 = v42;
					v10 = v33;
					v11 = 0;
					v12 = v32;
					do {
						v13 = (v46 << 7) | v31 | ((v11 + v34) << 11);
						v14 = (float)(v10 - v12) * 0.25;
						if(v31 > 63) {
							v15 = &a4[v13];
							v16 = v12;
							v17 = 0;
							v18 = v11;
							do {
								v19 = 0;
								if(v16 > 0.0) v19 = Tile::rock->blockID;
								++v17;
								*v15 = v19;
								v15 += 128;
								v16 = v16 + v14;
							} while(v17 != 4);
LABEL_11:
							v20 = v18;
							goto LABEL_12;
						}
						if(v31 == 63) {
							v25 = &a4[v13];
							v26 = v12;
							v27 = 0;
							v18 = v11;
							do {
								v29 = Tile::calmWater->blockID;
								if(v26 > 0.0) v29 = Tile::rock->blockID;
								++v27;
								*v25 = v29;
								v25 += 128;
								v26 = v26 + v14;
							} while(v27 != 4);
							goto LABEL_11;
						}
						v21 = &a4[v13];
						v22 = v12;
						v23 = 0;
						v30 = v11;
						do {
							v24 = Tile::calmWater->blockID;
							if(v22 > 0.0) v24 = Tile::rock->blockID;
							++v23;
							*v21 = v24;
							v21 += 128;
							v22 = v22 + v14;
						} while(v23 != 4);
						v20 = v30;
LABEL_12:
						v11 = v20 + 1;
						v9 += 16;
						v12 = v12 + (float)((float)(v35 - v32) * 0.25);
						v10 = v10 + (float)((float)(v36 - v33) * 0.25);
					} while(v11 != 4);
					++v37;
					++v31;
					v32 = v32 + v38;
					v33 = v33 + v39;
					v35 = v35 + v40;
					v36 = v36 + v41;
				} while(v37 != 8);
				v43 += 4;
				if(v43 != 64) {
					heights = this->field_72D0;
					continue;
				}
				break;
			}
			v46 += 4;
			v42 += 4;
			if(v46 != 16) {
				v45 += 68;
				v44 += 68;
				heights = this->field_72D0;
				continue;
			}
			break;
		}
		v34 += 4;
		a6 += 64;
		if(v34 != 16) {
			heights = this->field_72D0;
			continue;
		}
		break;
	}
}

NewRandomLevelSource::~NewRandomLevelSource() {
	if(this->field_72D0) {
		delete[] this->field_72D0;
	}
	if(this->interpolationNoises) {
		delete[] this->interpolationNoises;
	}

	if(this->upperInterpolationNoises) {
		delete[] this->upperInterpolationNoises;
	}

	if(this->lowerInterpolationNoises) {
		delete[] this->lowerInterpolationNoises;
	}

	if(this->biomeNoises) {
		delete[] this->biomeNoises;
	}

	if(this->depthNoises) {
		delete[] this->depthNoises;
	}

	if(this->field_7EE8) {
		delete[] this->field_7EE8;
	}

	if(this->field_7EEC) {
		delete[] this->field_7EEC;
	}
}
bool_t NewRandomLevelSource::hasChunk(int32_t x, int32_t z) {
	return 1;
}
struct LevelChunk* NewRandomLevelSource::getChunk(int32_t chunkX, int32_t chunkZ) {
	this->random.setSeed(132899541 * chunkZ + 341872712 * chunkX);
	/*
	 * LevelChunk indexes whatever buffer it is handed with LevelHeight::index, so
	 * the size has to come from the same place - a fixed 0x8000 here would be a
	 * 32K heap overflow the moment the two disagreed.  The terrain below is
	 * written with the 128 tall layout on purpose: this generator has no taller
	 * shape to produce, and a Java session replaces every column it is given
	 * anyway.  Allocating for the real height just means it cannot be overrun.
	 */
	uint8_t* chunkData = new uint8_t[LevelHeight::tiles];
	memset(chunkData, 0, LevelHeight::tiles);
	LevelChunk* chunk = new LevelChunk(this->level, chunkData, chunkX, chunkZ);

	if (this->level->getLevelData()->getGeneratorVersion() == 2) {
		for (int32_t bx = 0; bx < 16; ++bx) {
			for (int32_t bz = 0; bz < 16; ++bz) {
				int32_t baseIdx = LevelHeight::index(bx, 0, bz);
				chunkData[baseIdx + 0] = Tile::unbreakable->blockID;
				chunkData[baseIdx + 1] = Tile::dirt->blockID;
				chunkData[baseIdx + 2] = Tile::dirt->blockID;
				chunkData[baseIdx + 3] = Tile::grass->blockID;
			}
		}
		chunk->recalcHeightmap();
		return chunk;
	}

	Biome** v16 = this->level->getBiomeSource()->getBiomeBlock(16 * chunkX, 16 * chunkZ, 16, 16);
	this->prepareHeights(chunkX, chunkZ, chunkData, 0, this->level->getBiomeSource()->rainfallNoises);
	this->buildSurfaces(chunkX, chunkZ, chunkData, v16);
	if (this->level && this->level->getLevelData() && this->level->getLevelData()->generateCaves) {
		this->caveGenerator.apply(this, this->level, chunkX, chunkZ, chunkData, 0);
	}
	chunk->recalcHeightmap();
	return chunk;
}

struct LevelChunk* NewRandomLevelSource::create(int32_t x, int32_t z) {
	return this->getChunk(x, z);
}
static double _D6E52C58 = 0;
void NewRandomLevelSource::postProcess(struct ChunkSource* a2, int32_t chunkX, int32_t chunkZ) {
	this->level->field_12 = 1;
	int32_t chunkXStart = 16 * chunkX;
	int32_t chunkZStart = 16 * chunkZ;
	HeavyTile::instaFall = 1;

	BiomeSource* bs = this->level->getBiomeSource();
	Biome* biomeAtChunk = bs->getBiome(chunkXStart + 16, chunkZStart + 16);
	this->random.setSeed(this->level->getSeed());
	int v11 = this->random.genrand_int32();
	int v12 = (2 * (this->random.genrand_int32() >> 2) + 1) * chunkZ;
	this->random.setSeed((v12 + (2 * (v11 >> 2) + 1) * chunkX) ^ this->level->getSeed());
	double timeS = getTimeS();
	Random* a8 = &this->random;
	
	if (this->level->getLevelData()->getGeneratorVersion() != 2) {
		for(int32_t v14 = 0; v14 < 10; ++v14) {
			int32_t v16 = chunkXStart + (a8->genrand_int32() & 0xF);
			int8_t v17 = a8->genrand_int32();
			int32_t v18 = chunkZStart + (a8->genrand_int32() & 0xF);
			ClayFeature f;
			f.place(this->level, a8, v16, v17 & 0x7F, v18);
		}
	for(int32_t v20 = 0; v20 < 20; ++v20) {
		int32_t v21 = chunkXStart + (a8->genrand_int32() & 0xF);
		int8_t v22 = a8->genrand_int32();
		int8_t v23 = a8->genrand_int32();
		int32_t z = chunkZStart + (v23 & 0xF);
		OreFeature f(Tile::dirt->blockID, 32);
		f.place(this->level, a8, v21, v22 & 0x7F, z);
	}
	for(int32_t v25 = 0; v25 < 10; ++v25) {
		int32_t v26 = chunkXStart + (a8->genrand_int32() & 0xF);
		int8_t v27 = a8->genrand_int32();
		int8_t v28 = a8->genrand_int32();
		int32_t v29 = chunkZStart + (v28 & 0xF);
		OreFeature f(Tile::gravel->blockID, 32);
		f.place(this->level, a8, v26, v27 & 0x7F, v29);
	}
	for(int v30 = 0; v30 < 20; ++v30) {
		int32_t v31 = chunkXStart + (a8->genrand_int32() & 0xF);
		int8_t v32 = a8->genrand_int32();
		int8_t v33 = a8->genrand_int32();
		int32_t v34 = chunkZStart + (v33 & 0xF);
		OreFeature f(Tile::coalOre->blockID, 16);
		f.place(this->level, a8, v31, v32 & 0x7F, v34);
	}
	for(int v36 = 0; v36 < 20; ++v36) {
		int32_t v37 = chunkXStart + (a8->genrand_int32() & 0xF);
		int8_t v38 = a8->genrand_int32();
		int8_t v39 = a8->genrand_int32();
		int32_t v40 = chunkZStart + (v39 & 0xF);
		OreFeature f(Tile::ironOre->blockID, 8);
		f.place(this->level, a8, v37, v38 & 0x3F, v40);
	}
	if (Tile::copperOre) {
		for(int v36 = 0; v36 < 16; ++v36) {
			int32_t v37 = chunkXStart + (a8->genrand_int32() & 0xF);
			int32_t v38 = a8->genrand_int32() % 96;
			int32_t v40 = chunkZStart + (a8->genrand_int32() & 0xF);
			OreFeature f(Tile::copperOre->blockID, 8);
			f.place(this->level, a8, v37, v38, v40);
		}
	}
	for(int v42 = 0; v42 < 2; ++v42) {
		int32_t v43 = chunkXStart + (a8->genrand_int32() & 0xF);
		int8_t v44 = a8->genrand_int32();
		int8_t v45 = a8->genrand_int32();
		int32_t v46 = chunkZStart + (v45 & 0xF);
		OreFeature f(Tile::goldOre->blockID, 8);
		f.place(this->level, a8, v43, v44 & 0x1F, v46);
	}
	for(int v48 = 0; v48 < 8; ++v48) {
		int32_t v49 = chunkXStart + (a8->genrand_int32() & 0xF);
		int8_t v50 = a8->genrand_int32();
		int8_t v51 = a8->genrand_int32();
		int32_t v52 = chunkZStart + (v51 & 0xF);
		OreFeature f(Tile::redStoneOre->blockID, 7);
		f.place(this->level, a8, v49, v50 & 0xF, v52);
	}
	{
		int32_t v54 = chunkXStart + (a8->genrand_int32() & 0xF);
		int8_t v55 = a8->genrand_int32();
		int8_t v56 = a8->genrand_int32();
		int32_t v57 = chunkZStart + (v56 & 0xF);
		OreFeature f(Tile::emeraldOre->blockID, 7);
		f.place(this->level, a8, v54, v55 & 0xF, v57);
	}
	{
		int32_t v59 = chunkXStart + (a8->genrand_int32() & 0xF);
		int8_t v60 = a8->genrand_int32();
		int32_t v61 = (v60 & 0xF) + (a8->genrand_int32() & 0xF);
		int8_t v62 = a8->genrand_int32();
		int32_t v63 = chunkZStart + (v62 & 0xF);
		OreFeature f(Tile::lapisOre->blockID, 6);
		f.place(this->level, a8, v59, v61, v63);
	}

	float v = this->treeNoise.getValue((float)chunkXStart * 0.5, (float)chunkZStart * 0.5);
	int v67 = (int)(float)((float)((float)((float)(a8->nextFloat() * 4.0) + (float)(v * 0.125)) + 4.0) / 3.0);
	int v68 = v67;
	int v69 = a8->genrand_int32() % 0xA;
	int32_t amountOfTrees = v69 == 0;
	if(biomeAtChunk == Biome::forest) {
		amountOfTrees += v67 + 2;
	}
	if(biomeAtChunk == Biome::rainForest) {
		amountOfTrees += v67 + 2;
	}
	if(biomeAtChunk == Biome::birchForest) {
		amountOfTrees += v67 + 3;
	}
	if (biomeAtChunk == Biome::jungle || biomeAtChunk == Biome::equatorialRainforest) {
		amountOfTrees += v67 + 5;
	}
	if(biomeAtChunk == Biome::seasonalForest) {
		amountOfTrees += v67 + 1;
	}
	if(biomeAtChunk == Biome::swampland) {
		amountOfTrees += v67 + 2;
	}
	if(biomeAtChunk == Biome::taiga) {
		v68 = v67 + 1;
	}
	if(biomeAtChunk == Biome::taiga) {
		amountOfTrees += v68;
	}
	if(biomeAtChunk == Biome::desert) {
		amountOfTrees -= 20;
	}
	if(biomeAtChunk == Biome::tundra) {
		amountOfTrees -= 20;
	}
	if(biomeAtChunk == Biome::plains) {
		amountOfTrees -= 20;
	}
	if(this->level->getLevelData()->getGeneratorVersion() >= 1) {
		amountOfTrees += 4;
	}
	for(int32_t v65 = 0; v65 < amountOfTrees; ++v65) {
		int32_t xx = chunkXStart + (a8->genrand_int32() & 0xF) + 8;
		int32_t zz = chunkZStart + (a8->genrand_int32() & 0xF) + 8;
		int32_t hm = this->level->getHeightmap(xx, zz);
		Feature* v74 = biomeAtChunk->getTreeFeature(a8);
		if(v74) {
			v74->init(1, 1, 1);
			v74->place(this->level, a8, xx, hm, zz);
			delete v74;
		}
	}
	if (biomeAtChunk == Biome::plains && (a8->genrand_int32() % 4) == 0) {
		int32_t gx = chunkXStart + (a8->genrand_int32() & 0xF) + 8;
		int32_t gz = chunkZStart + (a8->genrand_int32() & 0xF) + 8;
		int treeCount = 2 + (a8->genrand_int32() % 4);
		for (int t = 0; t < treeCount; ++t) {
			int32_t tx = gx + (a8->genrand_int32() % 9) - 4;
			int32_t tz = gz + (a8->genrand_int32() % 9) - 4;
			int32_t thm = this->level->getHeightmap(tx, tz);
			if (thm > 0 && this->level->getTile(tx, thm - 1, tz) == Tile::grass->blockID) {
				Feature* tf = biomeAtChunk->getTreeFeature(a8);
				if (tf) {
					tf->init(1, 1, 1);
					tf->place(this->level, a8, tx, thm, tz);
					delete tf;
				}
			}
		}
	}
	if (biomeAtChunk == Biome::birchForest && (a8->genrand_int32() % 5) == 0) {
		int32_t xx = chunkXStart + (a8->genrand_int32() & 0xF) + 8;
		int32_t zz = chunkZStart + (a8->genrand_int32() & 0xF) + 8;
		int32_t hm = this->level->getHeightmap(xx, zz);
		int len = 3 + (a8->genrand_int32() % 3);
		int dx = (a8->genrand_int32() % 2 == 0) ? 1 : 0;
		int dz = 1 - dx;
		int meta = (dx == 1) ? 6 : 10;
		bool flatGround = true;
		for (int i = 0; i < len; ++i) {
			int tx = xx + i * dx;
			int tz = zz + i * dz;
			int thm = this->level->getHeightmap(tx, tz);
			if (thm != hm || !this->level->isEmptyTile(tx, hm, tz)) {
				flatGround = false;
				break;
			}
			int ground = this->level->getTile(tx, hm - 1, tz);
			if (ground != Tile::grass->blockID && ground != Tile::dirt->blockID) {
				flatGround = false;
				break;
			}
		}
		if (flatGround) {
			for (int i = 0; i < len; ++i) {
				this->level->setTileAndDataNoUpdate(xx + i * dx, hm, zz + i * dz, Tile::treeTrunk->blockID, meta);
			}
			if ((a8->genrand_int32() % 2) == 0 && Tile::mushroom1 && Tile::mushroom2) {
				int mIdx = a8->genrand_int32() % len;
				int mx = xx + mIdx * dx;
				int mz = zz + mIdx * dz;
				if (this->level->isEmptyTile(mx, hm + 1, mz)) {
					int mTile = (a8->genrand_int32() % 2 == 0) ? Tile::mushroom1->blockID : Tile::mushroom2->blockID;
					this->level->setTileAndDataNoUpdate(mx, hm + 1, mz, mTile, 0);
				}
			}
		}
	}
	if ((biomeAtChunk == Biome::forest || biomeAtChunk == Biome::birchForest || biomeAtChunk == Biome::jungle || biomeAtChunk == Biome::rainForest) && (a8->genrand_int32() % 5 == 0) && Tile::mushroom1 && Tile::mushroom2) {
		int32_t mx = chunkXStart + (a8->genrand_int32() & 0xF) + 8;
		int32_t mz = chunkZStart + (a8->genrand_int32() & 0xF) + 8;
		int32_t my = this->level->getHeightmap(mx, mz);
		int32_t mTile = (a8->genrand_int32() % 2 == 0) ? Tile::mushroom1->blockID : Tile::mushroom2->blockID;
		int mCount = 1 + (a8->genrand_int32() % 3);
		for (int m = 0; m < mCount; ++m) {
			int32_t smx = mx + (a8->genrand_int32() % 3) - 1;
			int32_t smz = mz + (a8->genrand_int32() % 3) - 1;
			int32_t smy = this->level->getHeightmap(smx, smz);
			int ground = this->level->getTile(smx, smy - 1, smz);
			if (this->level->isEmptyTile(smx, smy, smz) && (ground == Tile::grass->blockID || ground == Tile::dirt->blockID)) {
				this->level->setTileAndDataNoUpdate(smx, smy, smz, mTile, 0);
			}
		}
	}
	if (biomeAtChunk == Biome::jungle || biomeAtChunk == Biome::rainForest) {
		if ((a8->genrand_int32() % 8) == 0) {
			int32_t xx = chunkXStart + (a8->genrand_int32() & 0xF) + 8;
			int32_t zz = chunkZStart + (a8->genrand_int32() & 0xF) + 8;
			int32_t hm = this->level->getHeightmap(xx, zz);
			MelonFeature mf;
			mf.place(this->level, a8, xx, hm, zz);
		}

		for (int v = 0; v < 40; ++v) {
			int32_t vx = chunkXStart + (a8->genrand_int32() & 0xF) + 8;
			int32_t vz = chunkZStart + (a8->genrand_int32() & 0xF) + 8;
			int32_t vy = 64 + (a8->genrand_int32() % 60);
			VineFeature vf;
			vf.place(this->level, a8, vx, vy, vz);
		}

		if ((a8->genrand_int32() % 3) == 0) {
			int32_t mx = chunkXStart + (a8->genrand_int32() & 0xF) + 8;
			int32_t mz = chunkZStart + (a8->genrand_int32() & 0xF) + 8;
			int32_t my = this->level->getHeightmap(mx, mz);
			MegaJungleTreeFeature mjf(0, 14, 3, 3);
			mjf.place(this->level, a8, mx, my, mz);
		}

		for (int b = 0; b < 4; ++b) {
			int32_t bx = chunkXStart + (a8->genrand_int32() & 0xF) + 8;
			int32_t bz = chunkZStart + (a8->genrand_int32() & 0xF) + 8;
			int32_t by = this->level->getHeightmap(bx, bz);
			JungleBushFeature jbf(3, 3);
			jbf.place(this->level, a8, bx, by, bz);
		}
	}
	if (biomeAtChunk == Biome::swampland) {
		for (int i = 0; i < 4; ++i) {
			int32_t wx = chunkXStart + (a8->genrand_int32() & 0xF) + 8;
			int32_t wz = chunkZStart + (a8->genrand_int32() & 0xF) + 8;
			int32_t wy = this->level->getHeightmap(wx, wz);
			WaterLilyFeature wlf;
			wlf.place(this->level, a8, wx, wy, wz);
		}
		for (int i = 0; i < 8; ++i) {
			int32_t rx = chunkXStart + (a8->genrand_int32() & 0xF) + 8;
			int32_t rz = chunkZStart + (a8->genrand_int32() & 0xF) + 8;
			int32_t ry = this->level->getHeightmap(rx, rz);
			ReedsFeature rf;
			rf.place(this->level, a8, rx, ry, rz);
		}
		if (Tile::mushroom1 && Tile::mushroom2) {
			for (int m = 0; m < 8; ++m) {
				int32_t mx = chunkXStart + (a8->genrand_int32() & 0xF) + 8;
				int32_t mz = chunkZStart + (a8->genrand_int32() & 0xF) + 8;
				int32_t my = this->level->getHeightmap(mx, mz);
				if (my > 0 && my < 127) {
					int32_t ground = this->level->getTile(mx, my - 1, mz);
					if ((ground == Tile::grass->blockID || ground == Tile::dirt->blockID || ground == Tile::rock->blockID) && this->level->isEmptyTile(mx, my, mz)) {
						int32_t mTile = (a8->genrand_int32() % 2 == 0) ? Tile::mushroom1->blockID : Tile::mushroom2->blockID;
						this->level->setTileAndData(mx, my, mz, mTile, 0, 2);
					}
				}
			}
		}
		if (Tile::pumpkin && (a8->genrand_int32() % 6 == 0)) {
			int32_t px = chunkXStart + (a8->genrand_int32() & 0xF) + 8;
			int32_t pz = chunkZStart + (a8->genrand_int32() & 0xF) + 8;
			int32_t py = this->level->getHeightmap(px, pz);
			if (py > 0 && py < 127 && this->level->isEmptyTile(px, py, pz) && this->level->getTile(px, py - 1, pz) == Tile::grass->blockID) {
				int32_t pCount = 1 + (a8->genrand_int32() % 4);
				for (int p = 0; p < pCount; ++p) {
					int32_t spx = px + (a8->genrand_int32() % 7) - 3;
					int32_t spz = pz + (a8->genrand_int32() % 7) - 3;
					int32_t spy = this->level->getHeightmap(spx, spz);
					if (spy > 0 && spy < 127 && this->level->isEmptyTile(spx, spy, spz) && this->level->getTile(spx, spy - 1, spz) == Tile::grass->blockID) {
						this->level->setTileAndData(spx, spy, spz, Tile::pumpkin->blockID, a8->genrand_int32() & 3, 2);
					}
				}
			}
		}
	}
	if (biomeAtChunk == Biome::equatorialRainforest) {
		for (int i = 0; i < 25; ++i) {
			int32_t rx = chunkXStart + (a8->genrand_int32() & 0xF) + 8;
			int32_t rz = chunkZStart + (a8->genrand_int32() & 0xF) + 8;
			int32_t ry = this->level->getHeightmap(rx, rz);
			ReedsFeature rf;
			rf.place(this->level, a8, rx, ry, rz);
		}
		for (int i = 0; i < 14; ++i) {
			int32_t bx = chunkXStart + (a8->genrand_int32() & 0xF) + 8;
			int32_t bz = chunkZStart + (a8->genrand_int32() & 0xF) + 8;
			int32_t by = this->level->getHeightmap(bx, bz);
			JungleBushFeature jbf(3, 3);
			jbf.place(this->level, a8, bx, by, bz);
		}
		for (int i = 0; i < 28; ++i) {
			int32_t fx = chunkXStart + (a8->genrand_int32() & 0xF) + 8;
			int32_t fz = chunkZStart + (a8->genrand_int32() & 0xF) + 8;
			int32_t fy = this->level->getHeightmap(fx, fz);
			while (fy > 62 && (this->level->getTile(fx, fy, fz) == Tile::leaves->blockID || 
			                   this->level->getTile(fx, fy, fz) == Tile::treeTrunk->blockID || 
			                   this->level->getTile(fx, fy, fz) == Tile::vine->blockID || 
			                   this->level->isEmptyTile(fx, fy, fz))) {
				fy--;
			}
			if (fy > 0 && fy < 127 && this->level->isEmptyTile(fx, fy, fz)) {
				int32_t g = this->level->getTile(fx, fy - 1, fz);
				if (g == Tile::grass->blockID || g == Tile::dirt->blockID) {
					if ((a8->genrand_int32() % 3 == 0) && Tile::doublePlant) {
						DoublePlantFeature dpf(3);
						dpf.place(this->level, a8, fx, fy, fz);
					} else {
						this->level->setTileAndData(fx, fy, fz, Tile::tallgrass->blockID, 2, 2);
					}
				}
			}
		}
		if (Tile::flowerOrchid) {
			for (int i = 0; i < 16; ++i) {
				int32_t ox = chunkXStart + (a8->genrand_int32() & 0xF) + 8;
				int32_t oz = chunkZStart + (a8->genrand_int32() & 0xF) + 8;
				int32_t oy = this->level->getHeightmap(ox, oz);
				while (oy > 62 && (this->level->getTile(ox, oy, oz) == Tile::leaves->blockID || 
				                   this->level->getTile(ox, oy, oz) == Tile::treeTrunk->blockID || 
				                   this->level->getTile(ox, oy, oz) == Tile::vine->blockID || 
				                   this->level->isEmptyTile(ox, oy, oz))) {
					oy--;
				}
				if (oy > 0 && oy < 127 && this->level->isEmptyTile(ox, oy, oz)) {
					int32_t g = this->level->getTile(ox, oy - 1, oz);
					if (g == Tile::grass->blockID || g == Tile::dirt->blockID) {
						this->level->setTileAndData(ox, oy, oz, Tile::flowerOrchid->blockID, 0, 2);
					}
				}
			}
		}
		for (int wx = 0; wx < 16; ++wx) {
			for (int wz = 0; wz < 16; ++wz) {
				int32_t px = chunkXStart + wx;
				int32_t pz = chunkZStart + wz;
				int32_t py = this->level->getHeightmap(px, pz);
				while (py > 62 && (this->level->getTile(px, py, pz) == Tile::leaves->blockID || 
				                   this->level->getTile(px, py, pz) == Tile::treeTrunk->blockID || 
				                   this->level->getTile(px, py, pz) == Tile::vine->blockID || 
				                   this->level->isEmptyTile(px, py, pz))) {
					py--;
				}
				if (py == 63 || py == 64) {
					if (((wx * 7 + wz * 13) % 10) < 7) {
						int t = this->level->getTile(px, py - 1, pz);
						if (t == Tile::grass->blockID || t == Tile::dirt->blockID) {
							this->level->setTileAndData(px, py - 1, pz, Tile::dirt->blockID, 0, 2);
							this->level->setTileAndData(px, py, pz, Tile::calmWater->blockID, 0, 2);
						}
					}
				}
			}
		}
		if (Tile::mushroom1 && Tile::mushroom2) {
			for (int m = 0; m < 12; ++m) {
				int32_t mx = chunkXStart + (a8->genrand_int32() & 0xF) + 8;
				int32_t mz = chunkZStart + (a8->genrand_int32() & 0xF) + 8;
				int32_t my = this->level->getHeightmap(mx, mz);
				if (my > 0 && my < 127) {
					int32_t ground = this->level->getTile(mx, my - 1, mz);
					if ((ground == Tile::grass->blockID || ground == Tile::dirt->blockID || ground == Tile::rock->blockID) && this->level->isEmptyTile(mx, my, mz)) {
						int32_t mTile = (a8->genrand_int32() % 2 == 0) ? Tile::mushroom1->blockID : Tile::mushroom2->blockID;
						this->level->setTileAndData(mx, my, mz, mTile, 0, 2);
					}
				}
			}
		}
		for (int i = 0; i < 6; ++i) {
			int32_t lx = chunkXStart + (a8->genrand_int32() & 0xF) + 8;
			int32_t lz = chunkZStart + (a8->genrand_int32() & 0xF) + 8;
			int len = 4 + (a8->genrand_int32() % 4);
			int dir = a8->genrand_int32() % 2;
			for (int step = 0; step < len; ++step) {
				int sx = lx + (dir == 0 ? step : 0);
				int sz = lz + (dir == 1 ? step : 0);
				int sy = this->level->getHeightmap(sx, sz);
				while (sy > 62 && (this->level->getTile(sx, sy, sz) == Tile::leaves->blockID || 
				                   this->level->getTile(sx, sy, sz) == Tile::treeTrunk->blockID || 
				                   this->level->getTile(sx, sy, sz) == Tile::vine->blockID || 
				                   this->level->isEmptyTile(sx, sy, sz))) {
					sy--;
				}
				if (sy < 1) continue;
				int t = this->level->getTile(sx, sy, sz);
				int tBelow = this->level->getTile(sx, sy - 1, sz);
				if (t == 0 || t == Tile::water->blockID || t == Tile::calmWater->blockID || t == Tile::tallgrass->blockID || t == Tile::vine->blockID) {
					int placeY = (sy > 62 ? sy : 63);
					this->level->setTileAndData(sx, placeY, sz, Tile::treeTrunk->blockID, (dir == 0 ? 7 : 11), 2);
					if (tBelow == Tile::water->blockID || tBelow == Tile::calmWater->blockID) {
						for (int d = placeY - 1; d >= 1; --d) {
							int subT = this->level->getTile(sx, d, sz);
							if (subT == Tile::water->blockID || subT == Tile::calmWater->blockID || subT == 0) {
								if (a8->genrand_int32() % 2 == 0) {
									this->level->setTileAndData(sx, d, sz, Tile::treeTrunk->blockID, 3, 2);
								}
							} else break;
						}
					}
				}
			}
		}
		for (int i = 0; i < 8; ++i) {
			int32_t wx = chunkXStart + (a8->genrand_int32() & 0xF) + 8;
			int32_t wz = chunkZStart + (a8->genrand_int32() & 0xF) + 8;
			int32_t wy = this->level->getHeightmap(wx, wz);
			while (wy > 62 && (this->level->getTile(wx, wy, wz) == Tile::leaves->blockID || 
			                   this->level->getTile(wx, wy, wz) == Tile::treeTrunk->blockID || 
			                   this->level->getTile(wx, wy, wz) == Tile::vine->blockID || 
			                   this->level->isEmptyTile(wx, wy, wz))) {
				wy--;
			}
			WaterLilyFeature wlf;
			wlf.place(this->level, a8, wx, wy, wz);
		}
		if (!this->level->isClientMaybe) {
			int frogCount = 2 + (a8->genrand_int32() % 3);
			for (int f = 0; f < frogCount; ++f) {
				int32_t fx = chunkXStart + (a8->genrand_int32() & 0xF) + 8;
				int32_t fz = chunkZStart + (a8->genrand_int32() & 0xF) + 8;
				int32_t fy = this->level->getHeightmap(fx, fz);
				while (fy > 62 && (this->level->getTile(fx, fy, fz) == Tile::leaves->blockID || 
				                   this->level->getTile(fx, fy, fz) == Tile::treeTrunk->blockID || 
				                   this->level->getTile(fx, fy, fz) == Tile::vine->blockID || 
				                   this->level->isEmptyTile(fx, fy, fz))) {
					fy--;
				}
				if (fy > 0 && fy < 126) {
					int32_t ground = this->level->getTile(fx, fy, fz);
					if (ground == Tile::water->blockID || ground == Tile::calmWater->blockID || ground == Tile::grass->blockID || ground == Tile::dirt->blockID || ground == Tile::waterLily->blockID) {
						Frog* frog = new Frog(this->level);
						frog->moveTo((float)fx + 0.5f, (float)(fy + 1), (float)fz + 0.5f, 0.0f, 0.0f);
						MobSpawner::finalizeMobSettings(frog, this->level, 0.0, 0.0, 0.0);
						this->level->addEntity(frog);
					}
				}
			}
		}
	}
	if (biomeAtChunk == Biome::taiga) {
		if ((a8->genrand_int32() % 4) == 0) {
			int32_t sx = chunkXStart + (a8->genrand_int32() & 0xF) + 8;
			int32_t sz = chunkZStart + (a8->genrand_int32() & 0xF) + 8;
			int32_t sy = this->level->getHeightmap(sx, sz);
			if (sy > 62 && sy < 80) {
				for (int l = 0; l < 3; ++l) {
					int32_t tx = sx + (a8->genrand_int32() % 5) - 2;
					int32_t tz = sz + (a8->genrand_int32() % 5) - 2;
					int32_t ty = this->level->getHeightmap(tx, tz);
					if (ty > 62 && ty < 80 && this->level->getTile(tx, ty - 1, tz) == Tile::grass->blockID) {
						this->level->setTileAndData(tx, ty - 1, tz, Tile::water->blockID, 0, 3);
						this->level->addToTickNextTick(tx, ty - 1, tz, Tile::water->blockID, 5);
						this->level->updateNeighborsAt(tx, ty - 1, tz, Tile::water->blockID);
					}
				}
			}
		}
		if (Tile::mossStone && (a8->genrand_int32() % 4) == 0) {
			int32_t bx = chunkXStart + (a8->genrand_int32() & 0xF) + 8;
			int32_t bz = chunkZStart + (a8->genrand_int32() & 0xF) + 8;
			int32_t surfaceY = this->level->getHeightmap(bx, bz);
			while (surfaceY > 64 && (this->level->getTile(bx, surfaceY, bz) == Tile::leaves->blockID || 
			                         this->level->getTile(bx, surfaceY, bz) == Tile::treeTrunk->blockID || 
			                         this->level->isEmptyTile(bx, surfaceY, bz) || 
			                         this->level->getTile(bx, surfaceY, bz) == Tile::topSnow->blockID || 
			                         this->level->getTile(bx, surfaceY, bz) == Tile::tallgrass->blockID)) {
				surfaceY--;
			}
			int32_t ground = this->level->getTile(bx, surfaceY, bz);
			if (surfaceY >= 68 && (ground == Tile::grass->blockID || ground == Tile::dirt->blockID || ground == Tile::rock->blockID)) {
				static const int32_t OFFSETS[6][3] = {
					{0, 0, 0}, {0, -1, 0}, {1, 0, 0}, {0, 0, 1}, {-1, 0, 0}, {0, 0, -1}
				};
				int32_t totalPlaced = 0;
				int32_t targetCount = 4 + (a8->genrand_int32() % 2);
				for (int32_t o = 0; o < 6 && totalPlaced < targetCount; ++o) {
					int32_t px = bx + OFFSETS[o][0];
					int32_t py = surfaceY + OFFSETS[o][1];
					int32_t pz = bz + OFFSETS[o][2];
					if (py > 1 && py < 127) {
						int32_t cur = this->level->getTile(px, py, pz);
						if (cur == Tile::grass->blockID || cur == Tile::dirt->blockID || cur == Tile::rock->blockID || cur == Tile::gravel->blockID || cur == Tile::sand->blockID || cur == Tile::topSnow->blockID || cur == 0) {
							this->level->setTileAndData(px, py, pz, Tile::mossStone->blockID, 0, 2);
							totalPlaced++;
						}
					}
				}
			}
		}
	}
	if (Tile::sweetBerryBush && (biomeAtChunk == Biome::taiga || biomeAtChunk == Biome::tundra || biomeAtChunk == Biome::icePeaks)) {
		if ((a8->genrand_int32() % 5) == 0) {
			int32_t bx = chunkXStart + (a8->genrand_int32() & 0xF) + 8;
			int32_t bz = chunkZStart + (a8->genrand_int32() & 0xF) + 8;
			for (int l = 0; l < 8; ++l) {
				int32_t px = bx + (a8->genrand_int32() % 7) - (a8->genrand_int32() % 7);
				int32_t pz = bz + (a8->genrand_int32() % 7) - (a8->genrand_int32() % 7);
				int32_t py = this->level->getHeightmap(px, pz);
				if (py > 0 && py < 127 && (this->level->isEmptyTile(px, py, pz) || this->level->getTile(px, py, pz) == Tile::topSnow->blockID)) {
					int32_t below = this->level->getTile(px, py - 1, pz);
					if (below == Tile::grass->blockID || below == Tile::dirt->blockID) {
						int32_t stage = 1 + (a8->genrand_int32() % 3);
						this->level->setTileAndData(px, py, pz, Tile::sweetBerryBush->blockID, stage, 2);
					}
				}
			}
		}
	}
	
	int grassCount = 0;
	if (biomeAtChunk == Biome::plains || biomeAtChunk == Biome::forest || biomeAtChunk == Biome::birchForest || biomeAtChunk == Biome::jungle || biomeAtChunk == Biome::savanna) {
		grassCount = 1;
	} else if (biomeAtChunk == Biome::taiga) {
		grassCount = 1;
	}
	if (this->level->getLevelData()->getGeneratorVersion() >= 1) grassCount = 2;
	for (int i = 0; i < grassCount; ++i) {
		int32_t xx = chunkXStart + (a8->genrand_int32() & 0xF) + 8;
		int32_t zz = chunkZStart + (a8->genrand_int32() & 0xF) + 8;
		int32_t hm = this->level->getHeightmap(xx, zz);
		Feature* f = biomeAtChunk->getGrassFeature(a8);
		if (f) {
			f->place(this->level, a8, xx, hm, zz);
			delete f;
		}
	}
	if (biomeAtChunk != Biome::desert && biomeAtChunk != Biome::iceDesert && biomeAtChunk != Biome::icePeaks) {
		if (biomeAtChunk == Biome::jungle || biomeAtChunk == Biome::rainForest) {
			for (int j = 0; j < 12; ++j) {
				int32_t fx = chunkXStart + (a8->genrand_int32() & 0xF) + 8;
				int32_t fz = chunkZStart + (a8->genrand_int32() & 0xF) + 8;
				int32_t fy = this->level->getHeightmap(fx, fz);
				if (a8->genrand_int32() % 3 == 0) {
					DoublePlantFeature dpf(1);
					dpf.place(this->level, a8, fx, fy, fz);
				} else if (a8->genrand_int32() % 4 == 0) {
					DoublePlantFeature dpf(0);
					dpf.place(this->level, a8, fx, fy, fz);
				} else {
					int32_t gmeta = (a8->genrand_int32() % 4 == 0) ? 2 : 1;
					TallgrassFeature tgf(Tile::tallgrass->blockID, gmeta);
					tgf.place(this->level, a8, fx, fy, fz);
				}
			}
		} else {
			if ((a8->genrand_int32() % 4) == 0) {
				int32_t fx = chunkXStart + (a8->genrand_int32() & 0xF) + 8;
				int32_t fz = chunkZStart + (a8->genrand_int32() & 0xF) + 8;
				int32_t fy = this->level->getHeightmap(fx, fz);
				DoublePlantFeature dpf(0);
				dpf.place(this->level, a8, fx, fy, fz);
			}
			if ((a8->genrand_int32() % 6) == 0) {
				int32_t fx = chunkXStart + (a8->genrand_int32() & 0xF) + 8;
				int32_t fz = chunkZStart + (a8->genrand_int32() & 0xF) + 8;
				int32_t fy = this->level->getHeightmap(fx, fz);
				DoublePlantFeature dpf(1);
				dpf.place(this->level, a8, fx, fy, fz);
			}
		}

		bool isWarmBiome = (biomeAtChunk == Biome::plains || biomeAtChunk == Biome::forest || biomeAtChunk == Biome::birchForest || biomeAtChunk == Biome::seasonalForest || biomeAtChunk == Biome::savanna);
		if (isWarmBiome) {
			int bigFlowerChance = (biomeAtChunk == Biome::plains) ? 6 : 14;
			if ((a8->genrand_int32() % bigFlowerChance) == 0) {
				int32_t fx = chunkXStart + (a8->genrand_int32() & 0xF) + 8;
				int32_t fz = chunkZStart + (a8->genrand_int32() & 0xF) + 8;
				int32_t fy = this->level->getHeightmap(fx, fz);
				int32_t doubleFlowerSubtype = (a8->genrand_int32() % 2 == 0) ? 2 : 3;
				DoublePlantFeature dpf(doubleFlowerSubtype);
				dpf.place(this->level, a8, fx, fy, fz);
			}

			int smallFlowerChance = (biomeAtChunk == Biome::plains) ? 2 : 4;
			if ((a8->genrand_int32() % smallFlowerChance) == 0) {
				int32_t fx = chunkXStart + (a8->genrand_int32() & 0xF) + 8;
				int32_t fz = chunkZStart + (a8->genrand_int32() & 0xF) + 8;
				int32_t fy = this->level->getHeightmap(fx, fz);
				Tile* flowers[] = {
					Tile::flower,
					Tile::rose,
					Tile::flowerRose,
					Tile::flowerPaeonia,
					Tile::flowerDaisy,
					Tile::flowerHoustonia,
					Tile::flowerOrchid,
					Tile::flowerAllium
				};
				int32_t flowerIdx = a8->genrand_int32() % 8;
				Tile* fl = flowers[flowerIdx];
				if (fl) {
					int count = 1 + (a8->genrand_int32() % 2);
					for (int c = 0; c < count; ++c) {
						int32_t sfx = fx + (a8->genrand_int32() % 3) - 1;
						int32_t sfz = fz + (a8->genrand_int32() % 3) - 1;
						int32_t sfy = this->level->getHeightmap(sfx, sfz);
						if (this->level->isEmptyTile(sfx, sfy, sfz) && fl->canSurvive(this->level, sfx, sfy, sfz)) {
							this->level->setTileAndData(sfx, sfy, sfz, fl->blockID, 0, 2);
						}
					}
				}
			}
		}
	}
	if((a8->genrand_int32() & 3) == 0) {
		int8_t v84 = a8->genrand_int32();
		int8_t v85 = a8->genrand_int32();
		int8_t v86 = a8->genrand_int32();
		FlowerFeature f(Tile::mushroom1->blockID);
		f.place(this->level, a8, chunkXStart + (v84 & 0xF) + 8, v85 & 0x7F, chunkZStart + (v86 & 0xF) + 8);
	}
	if((a8->genrand_int32() & 7) == 0) {
		int8_t v89 = a8->genrand_int32();
		int8_t v90 = a8->genrand_int32();
		int8_t v91 = a8->genrand_int32();
		FlowerFeature f(Tile::mushroom2->blockID);
		f.place(this->level, a8, chunkXStart + (v89 & 0xF) + 8, v90 & 0x7F, chunkZStart + (v91 & 0xF) + 8);
	}
	int32_t reedsCnt = 10;
	if (biomeAtChunk == Biome::desert || biomeAtChunk == Biome::iceDesert) {
		reedsCnt = 40;
	} else if (biomeAtChunk == Biome::swampland) {
		reedsCnt = 25;
	}
	for(int32_t v95 = 0; v95 < reedsCnt; ++v95) {
		int8_t v96 = a8->genrand_int32();
		int8_t v97 = a8->genrand_int32();
		int8_t v98 = a8->genrand_int32();
		ReedsFeature f;
		f.place(this->level, a8, chunkXStart + (v96 & 0xF) + 8, v97 & 0x7F, chunkZStart + (v98 & 0xF) + 8);
	}
	int32_t cactiCnt;
	if(biomeAtChunk == Biome::desert) {
		cactiCnt = 5;
	} else {
		cactiCnt = 0;
	}
	for(int32_t v95 = 0; v95 < cactiCnt; ++v95) {
		int8_t v100 = a8->genrand_int32();
		int8_t v101 = a8->genrand_int32();
		int8_t v102 = a8->genrand_int32();
		CactusFeature f;
		f.place(this->level, a8, chunkXStart + (v100 & 0xF) + 8, v101 & 0x7F, chunkZStart + (v102 & 0xF) + 8);
	}
	for(int32_t v103 = 0; v103 < 50; ++v103) {
		int8_t v104 = a8->genrand_int32();
		int v137 = a8->genrand_int32();
		int v135 = a8->genrand_int32();
		int8_t v105 = a8->genrand_int32();
		SpringFeature f(Tile::water->blockID);
		f.place(this->level, a8, (v104 & 0xF) + chunkXStart + 8, v135 % (v137 % 0x78u + 8), chunkZStart + (v105 & 0xF) + 8);
	}
	for(int32_t v103 = 0; v103 < 20; ++v103) {
		int8_t v108 = a8->genrand_int32();
		int v138 = a8->genrand_int32();
		int v136 = a8->genrand_int32();
		int v109 = a8->genrand_int32();
		int8_t v110 = a8->genrand_int32();
		SpringFeature f(Tile::lava->blockID);
		f.place(this->level, a8, (v108 & 0xF) + chunkXStart + 8, v109 % (v136 % (v138 % 0x70u + 8) + 8), chunkZStart + (v110 & 0xF) + 8);
	}
	}
	if (Tile::seagrass) {
		for (int32_t ox = 0; ox < 16; ++ox) {
			for (int32_t oz = 0; oz < 16; ++oz) {
				if ((a8->genrand_int32() % 100) < 65) {
					int32_t sx = chunkXStart + ox;
					int32_t sz = chunkZStart + oz;
					for (int32_t sy = 62; sy >= 30; --sy) {
						int32_t cur = this->level->getTile(sx, sy, sz);
						int32_t below = this->level->getTile(sx, sy - 1, sz);
						if ((cur == Tile::water->blockID || cur == Tile::calmWater->blockID) &&
						    (below == Tile::dirt->blockID || below == Tile::sand->blockID || below == Tile::gravel->blockID || below == Tile::clay->blockID)) {
							int32_t above = this->level->getTile(sx, sy + 1, sz);
							int32_t roll = a8->genrand_int32() % 100;
							if (roll < 60 || (above != Tile::water->blockID && above != Tile::calmWater->blockID)) {
								this->level->setTileAndData(sx, sy, sz, Tile::seagrass->blockID, 0, 2);
							} else {
								this->level->setTileAndData(sx, sy, sz, Tile::seagrass->blockID, 1, 2);
								this->level->setTileAndData(sx, sy + 1, sz, Tile::seagrass->blockID, 2, 2);
							}
							break;
						}
					}
				}
			}
		}
	}
	if (this->level->getLevelData()->getGeneratorVersion() >= 1) {
		int32_t vChunkX = (int32_t)floorf((float)chunkX / 24.0f);
		int32_t vChunkZ = (int32_t)floorf((float)chunkZ / 24.0f);
		uint64_t vSeed = ((uint64_t)vChunkX * 341873128712ULL + (uint64_t)vChunkZ * 132897987541ULL) ^ (uint64_t)this->level->getSeed();
		Random vRand(vSeed);
		int32_t targetChunkX = vChunkX * 24 + (vRand.genrand_int32() % 16);
		int32_t targetChunkZ = vChunkZ * 24 + (vRand.genrand_int32() % 16);
		if (chunkX == targetChunkX && chunkZ == targetChunkZ) {
			if (biomeAtChunk == Biome::plains || biomeAtChunk == Biome::desert || biomeAtChunk == Biome::taiga || biomeAtChunk == Biome::tundra || biomeAtChunk == Biome::icePeaks) {
				int32_t vx = chunkXStart + 8;
				int32_t vz = chunkZStart + 8;
				int32_t vy = this->level->getHeightmap(vx, vz);
				if (vy <= 0) vy = 65;
				VillageFeature vf;
				vf.place(this->level, a8, vx, vy, vz);
			}
		}

		int32_t tChunkX = (int32_t)floorf((float)chunkX / 20.0f);
		int32_t tChunkZ = (int32_t)floorf((float)chunkZ / 20.0f);
		uint64_t tSeed = ((uint64_t)tChunkX * 498721981ULL + (uint64_t)tChunkZ * 987162534ULL) ^ (uint64_t)this->level->getSeed() ^ 0x5DEECE66DLL;
		Random tRand(tSeed);
		int32_t targetTChunkX = tChunkX * 20 + (tRand.genrand_int32() % 12);
		int32_t targetTChunkZ = tChunkZ * 20 + (tRand.genrand_int32() % 12);
		if (chunkX == targetTChunkX && chunkZ == targetTChunkZ && biomeAtChunk == Biome::desert) {
			int32_t tx = chunkXStart + 4;
			int32_t tz = chunkZStart + 4;
			int32_t ty = this->level->getHeightmap(tx + 10, tz + 10);
			if (ty <= 0) ty = 65;
			DesertTempleFeature dtf;
			dtf.place(this->level, a8, tx, ty, tz);
		}
	}
	if(this->field_72CC) {
		MobSpawner::postProcessSpawnMobs(this->level, biomeAtChunk, chunkXStart + 8, chunkZStart + 8, 16, 16, a8);
	}

	for (int32_t bx = 0; bx < 16; ++bx) {
		for (int32_t bz = 0; bz < 16; ++bz) {
			int32_t wx = chunkXStart + bx;
			int32_t wz = chunkZStart + bz;
			Biome* localBiome = this->level->getBiome(wx, wz);
			if (localBiome == Biome::tundra || localBiome == Biome::icePeaks || localBiome == Biome::taiga || localBiome == Biome::iceDesert) {
				int32_t topy = this->level->getTopSolidBlock(wx, wz);
				if (topy > 0 && topy < 128) {
					int32_t belowTile = this->level->getTile(wx, topy - 1, wz);
					if (belowTile == Tile::calmWater->blockID || belowTile == Tile::water->blockID) {
						if (localBiome == Biome::tundra || localBiome == Biome::icePeaks) {
							this->level->setTile(wx, topy - 1, wz, Tile::ice->blockID, 2);
						}
					} else if (this->level->isEmptyTile(wx, topy, wz)) {
						Material* v118 = this->level->getMaterial(wx, topy - 1, wz);
						if (v118 && v118->blocksMotion() && Material::ice != v118) {
							this->level->setTile(wx, topy, wz, Tile::topSnow->blockID, 2);
						}
					}
				}
			}
		}
	}
	_D6E52C58 = _D6E52C58 + getTimeS() - timeS;
	HeavyTile::instaFall = 0;
	this->level->field_12 = 0;

	LevelChunk* chunk = this->level->getChunk(chunkX, chunkZ);
	if (chunk) {
		chunk->recalcHeightmap();
	}
	while (this->level->updateLights()) {
	}
	this->field_19E0.clear();
}
bool_t NewRandomLevelSource::tick() {
	return 0;
}
bool_t NewRandomLevelSource::shouldSave() {
	return 1;
}
std::vector<Biome::MobSpawnerData> NewRandomLevelSource::getMobsAt(const struct MobCategory& a3, int32_t a4, int32_t a5, int32_t a6) {
	BiomeSource* bs = this->level->getBiomeSource();
	if(bs) {
		Biome* v10 = bs->getBiome(a4, a6);
		if(v10) {
			return std::vector<Biome::MobSpawnerData>(*v10->getMobs(a3));
		}
	}
	return std::vector<Biome::MobSpawnerData>();
}
std::string NewRandomLevelSource::gatherStats() {
	return "NewRandomLevelSource";
}
