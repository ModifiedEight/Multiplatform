#pragma once
#include <_types.h>
#include <level/gen/ChunkSource.hpp>
#include <string>
#include <vector>

struct LevelChunk;
struct Level;

/*
 * A server authoritative ChunkSource used only by Java (1.8.x) sessions.
 *
 * The stock ChunkCache pulls columns out of a local terrain generator and a
 * local chunk file.  For a Java session neither of those exist: every column we
 * are ever allowed to show comes straight out of the socket.  This source
 * therefore never generates and never saves - it hands out blank columns and
 * JavaChunkData fills them in when the matching Chunk Data packet arrives.
 *
 * Residency uses the same bounded 64x64 grid ChunkCache uses
 * (`(x & 0x3F) + 64 * (z & 0x3F)`), so walking a long way recycles slots
 * instead of growing without bound, and it does it through the same code shape
 * the engine already trusts for entity/renderer bookkeeping.
 *
 * A blank column is not entirely empty: it carries one layer of bedrock at
 * m8 y 0.  That mirrors a real Java overworld (which always has bedrock at
 * y 0, so the first Chunk Data packet overwrites it with the same thing), it
 * stops the player falling out of the world through a column the server has
 * not sent yet, and - critically - it keeps Level::validateSpawn() from
 * spinning forever: that loop runs before any chunk data can arrive and only
 * terminates once getTopTile() finds something that is neither air nor
 * invisible_bedrock.
 */
struct JavaChunkSource: ChunkSource {
	static const int32_t GRID = 64;
	static const int32_t GRID_MASK = 63;
	static const int32_t SLOTS = 4096;

	Level* level;
	LevelChunk* emptyChunk;
	LevelChunk* chunks[JavaChunkSource::SLOTS];
	int32_t lastChunkX, lastChunkZ;
	LevelChunk* lastChunk;
	int32_t residentCount;

	JavaChunkSource(Level* level);

	static int32_t slotOf(int32_t x, int32_t z);

	// Lookup that never allocates. Returns 0 when the column is not resident.
	LevelChunk* find(int32_t x, int32_t z);
	// Lookup that allocates a blank column (bedrock floor) when missing.
	LevelChunk* obtain(int32_t x, int32_t z);
	// Server told us to forget this column.
	void drop(int32_t x, int32_t z);
	void dropAll();

	virtual ~JavaChunkSource();
	virtual bool_t hasChunk(int32_t, int32_t);
	virtual LevelChunk* getChunk(int32_t, int32_t);
	virtual LevelChunk* create(int32_t, int32_t);
	virtual void postProcess(ChunkSource*, int32_t, int32_t);
	virtual bool_t tick();
	virtual bool_t shouldSave();
	virtual void saveAll(bool_t);
	virtual std::vector<Biome::MobSpawnerData> getMobsAt(const MobCategory&, int32_t, int32_t, int32_t);
	virtual std::string gatherStats();
};
