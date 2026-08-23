#pragma once
#include <_types.h>
#include <stddef.h>

struct Level;
struct LevelChunk;

/*
 * JavaChunkData - decodes a Java Edition 1.8 chunk column payload straight into
 * an m8 LevelChunk.
 *
 * The 1.8 payload is section major: for every bit set in the section mask, 4096
 * little endian u16 block states (id << 4 | meta), then for every set bit 2048
 * bytes of block light nibbles, then - on a dimension with sky - 2048 bytes of
 * sky light nibbles per set bit, and finally 256 biome bytes when the column is
 * a full "ground up" send.
 *
 * Neither the block array nor the nibble arrays can be memcpy'd across.  Java
 * indexes a section as (y << 8) | (z << 4) | x while m8 indexes a whole column
 * as y | (x << 11) | (z << 7), so every block and every nibble is transposed
 * individually, and every block state goes through JavaIdMap on the way.
 */
struct JavaChunkData
{
	// Bytes one column occupies on the wire, for slicing a Map Chunk Bulk blob.
	static size_t payloadSize(int32_t sectionMask, bool_t hasSkyLight, bool_t groundUp);

	/*
	 * Writes one column into `chunk`. Returns the number of bytes consumed, or 0
	 * if the payload was short (in which case nothing is written).
	 */
	static size_t apply(Level* level, LevelChunk* chunk, const uint8_t* data, size_t len,
	                    int32_t sectionMask, bool_t groundUp, bool_t hasSkyLight);

	// Recomputes heightMap[] and topBlockY the way LevelChunk::recalcHeightmap would,
	// without touching the sky light the server just gave us.
	static void recomputeHeights(LevelChunk* chunk);
};
