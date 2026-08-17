#include <level/ChunkCache.hpp>
#include <level/Level.hpp>
#include <level/chunk/LevelChunk.hpp>
#include <level/storage/chunk/ChunkStorage.hpp>
#include <level/storage/LevelData.hpp>

ChunkCache::~ChunkCache() {
	if(this->generatorSource) {
		delete this->generatorSource;
	}
	if(this->emptyChunk) {
		delete this->emptyChunk;
	}
	for(int32_t i = 0; i != 4096; ++i) {
		LevelChunk* lc = this->chunks[i];
		if(lc) {
			lc->deleteBlockData();
			lc = this->chunks[i];
			if(lc) {
				delete lc;
			}
		}
	}
}

static bool_t sub_D66664FE(int a1, int a2) {
	return a1 >= 0 && a2 >= 0 && a1 <= 15 && a2 <= 15;
}
// Returns true if chunk coords are 'valid' for this level (always true for infinite worlds)
static bool_t chunkInRange(struct Level* level, int x, int z) {
	if(level && level->getLevelData() && (level->getLevelData()->getGeneratorVersion() != 0 && level->getLevelData()->getGeneratorVersion() != 4))
		return 1;
	return sub_D66664FE(x, z);
}

bool_t ChunkCache::hasChunk(int32_t x, int32_t z) {
	LevelChunk* result;

	bool_t isInfinite = (this->level && this->level->getLevelData() && (this->level->getLevelData()->getGeneratorVersion() != 0 && this->level->getLevelData()->getGeneratorVersion() != 4));
	if((!isInfinite && !sub_D66664FE(x, z)) || (x == this->lastChunkX && z == this->lastChunkZ && this->lastChunk && this->lastChunk != this->emptyChunk)) {
		return 1;
	}
	result = this->chunks[64 * (z & 0x3F) + (x & 0x3F)];
	if(!result || result == this->emptyChunk) {
		return 0;
	}
	return result->isAt(x, z);
}
LevelChunk* ChunkCache::getChunk(int32_t x, int32_t z) {
	LevelChunk* result;
	int32_t v7;
	LevelChunk* v9;
	LevelChunk* v10;
	ChunkStorage* chunkStorage;
	ChunkStorage* v12;
	LevelChunk* emptyChunk;
	ChunkSource* generatorSource;
	LevelChunk* v16;

	LevelChunk* curC;
	LevelChunk* cWest;
	LevelChunk* cNorth;
	LevelChunk* cNW;

	if(x != this->lastChunkX || z != this->lastChunkZ || (result = this->lastChunk) == 0 || result == this->emptyChunk) {
		if(!chunkInRange(this->level, x, z)) {
			return this->emptyChunk;
		}
		v7 = (x & 0x3F) + 64 * (z & 0x3F);
		if(this->hasChunk(x, z)) {
			goto LABEL_48;
		}
		v9 = this->chunks[v7];
		if(v9 && v9 != this->emptyChunk) {
			v9->unload();
			v10 = this->chunks[v7];
			if(this->chunkStorage) {
				v10->field_250 = this->level->getTime();
				this->chunkStorage->save(this->level, v10);
			}
			chunkStorage = this->chunkStorage;
			if(chunkStorage) {
				chunkStorage->saveEntities(this->level, this->chunks[v7]);
			}
		}
		v12 = this->chunkStorage;
		emptyChunk = 0;
		if(v12 && (chunkInRange(this->level, x, z))) {
			emptyChunk = (LevelChunk*)v12->load(this->level, x, z);
			if(emptyChunk) {
				emptyChunk->field_250 = this->level->getTime();
			}
		}
		if(!emptyChunk) {
			generatorSource = this->generatorSource;
			if(generatorSource) {
				emptyChunk = (LevelChunk*)generatorSource->getChunk(x, z);
			} else {
				emptyChunk = this->emptyChunk;
			}
		}
		this->chunks[v7] = emptyChunk;
		if(emptyChunk && emptyChunk != this->emptyChunk) {
			emptyChunk->lightLava();
			emptyChunk->load();
		}
		curC = this->chunks[v7];
		if(curC && curC != this->emptyChunk) {
			if(!curC->decorated && this->hasChunk(x + 1, z + 1) && this->hasChunk(x, z + 1) && this->hasChunk(x + 1, z)) {
				this->postProcess(this, x, z);
			}
			if(this->hasChunk(x - 1, z) && this->hasChunk(x - 1, z + 1) && this->hasChunk(x, z + 1)) {
				cWest = this->chunks[((x - 1) & 0x3F) + 64 * (z & 0x3F)];
				if(cWest && cWest != this->emptyChunk && cWest->isAt(x - 1, z) && !cWest->decorated) {
					this->postProcess(this, x - 1, z);
				}
			}
			if(this->hasChunk(x, z - 1) && this->hasChunk(x + 1, z - 1) && this->hasChunk(x + 1, z)) {
				cNorth = this->chunks[(x & 0x3F) + 64 * ((z - 1) & 0x3F)];
				if(cNorth && cNorth != this->emptyChunk && cNorth->isAt(x, z - 1) && !cNorth->decorated) {
					this->postProcess(this, x, z - 1);
				}
			}
			if(this->hasChunk(x - 1, z - 1) && this->hasChunk(x - 1, z) && this->hasChunk(x, z - 1)) {
				cNW = this->chunks[((x - 1) & 0x3F) + 64 * ((z - 1) & 0x3F)];
				if(cNW && cNW != this->emptyChunk && cNW->isAt(x - 1, z - 1) && !cNW->decorated) {
					this->postProcess(this, x - 1, z - 1);
				}
			}
		}
LABEL_48:
		this->lastChunkX = x;
		this->lastChunkZ = z;
		result = this->chunks[v7];
		this->lastChunk = result;
	}
	return result;
}
LevelChunk* ChunkCache::create(int32_t x, int32_t z) {
	return this->getChunk(x, z);
}
void ChunkCache::postProcess(ChunkSource* a2, int32_t x, int32_t z) {
	LevelChunk* v8;		  // r0
	LevelChunk* v9;		  // r5
	Level* level;		  // r2
	bool_t isClientMaybe; // r9

	if(chunkInRange(this->level, x, z)) {
		v8 = this->getChunk(x, z);
		v9 = v8;
		if(!v8->decorated) {
			v8->decorated = 1;
			if(this->generatorSource) {
				level = this->level;
				isClientMaybe = level->isClientMaybe;
				level->isClientMaybe = 0;
				this->generatorSource->postProcess(a2, x, z);
				v9->clearUpdateMap();
				this->level->isClientMaybe = isClientMaybe;
			}
		}
	}
}
bool_t ChunkCache::tick() {
	if(this->chunkStorage) {
		this->chunkStorage->tick();
	}
	return this->generatorSource->tick();
}
bool_t ChunkCache::shouldSave() {
	return 1;
}
void ChunkCache::saveAll(bool_t a2) {
	if(this->chunkStorage) {
		std::vector<LevelChunk*> v8;
		for(int32_t i = 0; i < 4096; ++i) {
			LevelChunk* chunk = this->chunks[i];
			if(chunk && chunk != this->emptyChunk) {
				v8.emplace_back(chunk);
			}
		}
		this->chunkStorage->saveAll(this->level, v8);
	}
}
std::vector<Biome::MobSpawnerData> ChunkCache::getMobsAt(const MobCategory& a3, int32_t a4, int32_t a5, int32_t a6) {
	return this->generatorSource->getMobsAt(a3, a4, a5, a6);
}
std::string ChunkCache::gatherStats() {
	return "ChunkCache: 1024";
}
