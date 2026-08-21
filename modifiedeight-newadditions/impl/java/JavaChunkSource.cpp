#include <java/JavaChunkSource.hpp>
#include <level/LevelHeight.hpp>
#include <java/JavaY.hpp>
#include <level/Level.hpp>
#include <level/chunk/LevelChunk.hpp>
#include <level/chunk/EmptyLevelChunk.hpp>
#include <level/storage/DataLayer.hpp>
#include <level/dimension/Dimension.hpp>
#include <tile/Tile.hpp>
#include <tile/entity/TileEntity.hpp>
#include <string.h>

// A column the server has not described yet.  See the header for why this is
// not simply all air.
static void javaBlankColumn(LevelChunk* chunk, bool_t hasSky) {
	if(!chunk || !chunk->tiles) {
		return;
	}
	memset(chunk->tiles, 0, LevelHeight::tiles);
	memset(chunk->tileMeta.data, 0, LevelHeight::tiles / 2);
	memset(chunk->blockLight.data, 0, LevelHeight::tiles / 2);
	memset(chunk->skyLight.data, hasSky ? 0xFF : 0x00, LevelHeight::tiles / 2);

	int32_t bedrock = Tile::unbreakable ? Tile::unbreakable->blockID : 7;
	for(int32_t x = 0; x < 16; ++x) {
		for(int32_t z = 0; z < 16; ++z) {
			chunk->tiles[LevelHeight::index(x, 0, z)] = (uint8_t)bedrock;
			chunk->heightMap[x | (16 * z)] = 1;
			if(hasSky) {
				chunk->skyLight.set(x, 0, z, 0);
			}
		}
	}
	chunk->topBlockY = 1;
}

/*
 * Tile entities outlive their column otherwise.  Level::tick is what normally
 * frees them - it sweeps level->tileEntities and drops whatever is marked
 * removed - and a client level never runs it: MultiPlayerLevel::tick only moves
 * time and the sky along.  LevelRenderer walks that same list with no interest
 * in whether the column is still resident, so a recycled slot would leave signs
 * hanging in mid air at coordinates the server has long stopped talking about.
 */
static void javaReleaseTileEntities(Level* level, LevelChunk* chunk) {
	if(!chunk || chunk->tileEntities.empty()) {
		return;
	}
	for(auto&& entry: chunk->tileEntities) {
		TileEntity* te = entry.second;
		if(!te) {
			continue;
		}
		if(level) {
			for(size_t i = 0; i < level->tileEntities.size();) {
				if(level->tileEntities[i] == te) {
					level->tileEntities.erase(level->tileEntities.begin() + i);
				} else {
					++i;
				}
			}
			for(size_t i = 0; i < level->field_50.size();) {
				if(level->field_50[i] == te) {
					level->field_50.erase(level->field_50.begin() + i);
				} else {
					++i;
				}
			}
		}
		delete te;
	}
	chunk->tileEntities.clear();
}

JavaChunkSource::JavaChunkSource(Level* level) {
	this->level = level;
	this->emptyChunk = new EmptyLevelChunk(level);
	memset(this->chunks, 0, sizeof(this->chunks));
	this->lastChunkX = 0x7FFFFFFF;
	this->lastChunkZ = 0x7FFFFFFF;
	this->lastChunk = 0;
	this->residentCount = 0;
}

JavaChunkSource::~JavaChunkSource() {
	this->dropAll();
	if(this->emptyChunk) {
		delete this->emptyChunk;
		this->emptyChunk = 0;
	}
}

int32_t JavaChunkSource::slotOf(int32_t x, int32_t z) {
	return (x & JavaChunkSource::GRID_MASK) + JavaChunkSource::GRID * (z & JavaChunkSource::GRID_MASK);
}

LevelChunk* JavaChunkSource::find(int32_t x, int32_t z) {
	LevelChunk* chunk = this->chunks[JavaChunkSource::slotOf(x, z)];
	if(chunk && chunk->chunkX == x && chunk->chunkZ == z) {
		return chunk;
	}
	return 0;
}

LevelChunk* JavaChunkSource::obtain(int32_t x, int32_t z) {
	if(x == this->lastChunkX && z == this->lastChunkZ && this->lastChunk) {
		return this->lastChunk;
	}

	int32_t slot = JavaChunkSource::slotOf(x, z);
	LevelChunk* chunk = this->chunks[slot];
	if(chunk) {
		if(chunk->chunkX == x && chunk->chunkZ == z) {
			this->lastChunkX = x;
			this->lastChunkZ = z;
			this->lastChunk = chunk;
			return chunk;
		}
		// Slot collision: this is how ChunkCache recycles too.  The column is
		// server owned, so there is nothing to write back.
		chunk->unload();
		javaReleaseTileEntities(this->level, chunk);
		delete chunk;
		this->chunks[slot] = 0;
		--this->residentCount;
	}

	bool_t hasSky = this->level && this->level->dimensionPtr ? !this->level->dimensionPtr->hasNoSkyMaybe : 1;
	chunk = new LevelChunk(this->level, x, z);
	javaBlankColumn(chunk, hasSky);
	// Nothing here is ever generated or decorated locally, and nothing is ever
	// written to disk.
	chunk->decorated = 1;
	chunk->unsaved = 0;
	chunk->clearUpdateMap();
	chunk->load();

	this->chunks[slot] = chunk;
	++this->residentCount;
	this->lastChunkX = x;
	this->lastChunkZ = z;
	this->lastChunk = chunk;
	return chunk;
}

void JavaChunkSource::drop(int32_t x, int32_t z) {
	int32_t slot = JavaChunkSource::slotOf(x, z);
	LevelChunk* chunk = this->chunks[slot];
	if(!chunk || chunk->chunkX != x || chunk->chunkZ != z) {
		return;
	}
	if(this->lastChunk == chunk) {
		this->lastChunk = 0;
		this->lastChunkX = 0x7FFFFFFF;
		this->lastChunkZ = 0x7FFFFFFF;
	}
	chunk->unload();
	javaReleaseTileEntities(this->level, chunk);
	delete chunk;
	this->chunks[slot] = 0;
	--this->residentCount;
}

void JavaChunkSource::dropAll() {
	for(int32_t i = 0; i < JavaChunkSource::SLOTS; ++i) {
		LevelChunk* chunk = this->chunks[i];
		if(chunk) {
			chunk->unload();
			javaReleaseTileEntities(this->level, chunk);
			delete chunk;
			this->chunks[i] = 0;
		}
	}
	this->residentCount = 0;
	this->lastChunk = 0;
	this->lastChunkX = 0x7FFFFFFF;
	this->lastChunkZ = 0x7FFFFFFF;
}

// Level::hasChunkAt / hasChunksAt gate entity movement, tile writes and light
// updates on this.  Answering "no" for a column the server has not sent yet
// would freeze the player in place mid air, so every column inside the height
// range counts as present - getChunk() makes that true on demand.
bool_t JavaChunkSource::hasChunk(int32_t, int32_t) {
	return 1;
}

LevelChunk* JavaChunkSource::getChunk(int32_t x, int32_t z) {
	return this->obtain(x, z);
}

LevelChunk* JavaChunkSource::create(int32_t x, int32_t z) {
	return this->obtain(x, z);
}

void JavaChunkSource::postProcess(ChunkSource*, int32_t, int32_t) {
}

bool_t JavaChunkSource::tick() {
	return 0;
}

bool_t JavaChunkSource::shouldSave() {
	return 0;
}

void JavaChunkSource::saveAll(bool_t) {
}

std::vector<Biome::MobSpawnerData> JavaChunkSource::getMobsAt(const MobCategory&, int32_t, int32_t, int32_t) {
	return std::vector<Biome::MobSpawnerData>();
}

std::string JavaChunkSource::gatherStats() {
	return "JavaChunkSource";
}
