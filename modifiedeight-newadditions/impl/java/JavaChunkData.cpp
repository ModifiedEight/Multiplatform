#include <java/JavaChunkData.hpp>
#include <level/LevelHeight.hpp>
#include <java/JavaIdMap.hpp>
#include <java/JavaY.hpp>
#include <level/Level.hpp>
#include <level/chunk/LevelChunk.hpp>
#include <level/storage/DataLayer.hpp>
#include <tile/Tile.hpp>
#include <string.h>

static const size_t JAVA_SECTION_BLOCK_BYTES = 4096 * 2;
static const size_t JAVA_SECTION_NIBBLE_BYTES = 2048;
static const size_t JAVA_BIOME_BYTES = 256;

static int32_t javaSectionCount(int32_t sectionMask) {
	int32_t n = 0;
	for(int32_t i = 0; i < 16; ++i) {
		if(sectionMask & (1 << i)) ++n;
	}
	return n;
}

// m8 column index: y in the low bits, then z, then x - one bit wider each for
// x and z in a 256 tall level.  See LevelHeight.
static inline int32_t m8Index(int32_t x, int32_t y, int32_t z) {
	return LevelHeight::index(x, y, z);
}

static inline void nibbleSet(uint8_t* data, int32_t index, int32_t value) {
	uint8_t b = data[index >> 1];
	if((index & 1) != 0) {
		data[index >> 1] = (uint8_t)((b & 0x0F) | ((value & 0x0F) << 4));
	} else {
		data[index >> 1] = (uint8_t)((b & 0xF0) | (value & 0x0F));
	}
}

static void writeSectionBlocks(LevelChunk* chunk, int32_t section, const uint8_t* src) {
	uint8_t* tiles = chunk->tiles;
	uint8_t* meta = chunk->tileMeta.data;
	int32_t sectionBase = section * 16;

	for(int32_t yl = 0; yl < 16; ++yl) {
		int32_t javaY = sectionBase + yl;
		if(!JavaY::inRange(javaY)) continue;
		int32_t m8y = JavaY::toM8(javaY);

		for(int32_t z = 0; z < 16; ++z) {
			for(int32_t x = 0; x < 16; ++x) {
				int32_t ji = (yl << 8) | (z << 4) | x;
				int32_t state = (int32_t)src[ji * 2] | ((int32_t)src[ji * 2 + 1] << 8);
				int32_t id = 0, mt = 0;
				if(state) {
					JavaIdMap::javaBlockToM8(state >> 4, state & 0xF, &id, &mt);
				}
				int32_t mi = m8Index(x, m8y, z);
				tiles[mi] = (uint8_t)id;
				nibbleSet(meta, mi, mt);
			}
		}
	}
}

static void writeSectionNibbles(DataLayer* layer, int32_t section, const uint8_t* src) {
	uint8_t* dst = layer->data;
	int32_t sectionBase = section * 16;

	for(int32_t yl = 0; yl < 16; ++yl) {
		int32_t javaY = sectionBase + yl;
		if(!JavaY::inRange(javaY)) continue;
		int32_t m8y = JavaY::toM8(javaY);

		for(int32_t z = 0; z < 16; ++z) {
			for(int32_t x = 0; x < 16; ++x) {
				int32_t ji = (yl << 8) | (z << 4) | x;
				int32_t v;
				if((ji & 1) != 0) {
					v = (src[ji >> 1] >> 4) & 0x0F;
				} else {
					v = src[ji >> 1] & 0x0F;
				}
				nibbleSet(dst, m8Index(x, m8y, z), v);
			}
		}
	}
}

/*
 * A "ground up" send means the sections missing from the mask are empty, not
 * unchanged, so they have to be cleared. Sky light there goes to full: those
 * sections are above the terrain, and leaving them at 0 would put the player
 * under a black ceiling.
 */
static void clearSection(LevelChunk* chunk, int32_t section, bool_t hasSkyLight) {
	int32_t sectionBase = section * 16;
	for(int32_t yl = 0; yl < 16; ++yl) {
		int32_t javaY = sectionBase + yl;
		if(!JavaY::inRange(javaY)) continue;
		int32_t m8y = JavaY::toM8(javaY);

		for(int32_t z = 0; z < 16; ++z) {
			for(int32_t x = 0; x < 16; ++x) {
				int32_t mi = m8Index(x, m8y, z);
				chunk->tiles[mi] = 0;
				nibbleSet(chunk->tileMeta.data, mi, 0);
				nibbleSet(chunk->blockLight.data, mi, 0);
				nibbleSet(chunk->skyLight.data, mi, hasSkyLight ? 15 : 0);
			}
		}
	}
}

size_t JavaChunkData::payloadSize(int32_t sectionMask, bool_t hasSkyLight, bool_t groundUp) {
	size_t bits = (size_t)javaSectionCount(sectionMask);
	size_t n = bits * JAVA_SECTION_BLOCK_BYTES + bits * JAVA_SECTION_NIBBLE_BYTES;
	if(hasSkyLight) n += bits * JAVA_SECTION_NIBBLE_BYTES;
	if(groundUp) n += JAVA_BIOME_BYTES;
	return n;
}

size_t JavaChunkData::apply(Level* level, LevelChunk* chunk, const uint8_t* data, size_t len,
                            int32_t sectionMask, bool_t groundUp, bool_t hasSkyLight) {
	sectionMask &= 0xFFFF;
	size_t need = JavaChunkData::payloadSize(sectionMask, hasSkyLight, groundUp);
	if(need > len) return 0;
	if(!chunk || !chunk->tiles) return need;

	size_t off = 0;
	int32_t s;

	for(s = 0; s < 16; ++s) {
		if(sectionMask & (1 << s)) {
			if(JavaY::sectionVisible(s)) {
				writeSectionBlocks(chunk, s, data + off);
			}
			off += JAVA_SECTION_BLOCK_BYTES;
		} else if(groundUp && JavaY::sectionVisible(s)) {
			clearSection(chunk, s, hasSkyLight);
		}
	}
	for(s = 0; s < 16; ++s) {
		if(!(sectionMask & (1 << s))) continue;
		if(JavaY::sectionVisible(s)) {
			writeSectionNibbles(&chunk->blockLight, s, data + off);
		}
		off += JAVA_SECTION_NIBBLE_BYTES;
	}
	if(hasSkyLight) {
		for(s = 0; s < 16; ++s) {
			if(!(sectionMask & (1 << s))) continue;
			if(JavaY::sectionVisible(s)) {
				writeSectionNibbles(&chunk->skyLight, s, data + off);
			}
			off += JAVA_SECTION_NIBBLE_BYTES;
		}
	}
	// The 256 biome bytes are the tail of a ground up send. m8's LevelChunk has
	// nowhere to put them - biomes only ever reach it through its own generator -
	// so they are consumed and dropped.
	if(groundUp) off += JAVA_BIOME_BYTES;

	JavaChunkData::recomputeHeights(chunk);
	return off;
}

/*
 * LevelChunk::recalcHeightmap would do this, but it also overwrites skyLight
 * with its own straight-down attenuation, which would throw away the light the
 * server computed for overhangs and caves, and it calls lightGaps() 256 times,
 * which walks into Level::updateLight for neighbours that may not be resident.
 * heightMap[x | 16*z] is the lowest y with nothing but transparent blocks above
 * it, i.e. one past the highest light blocking block; topBlockY is the minimum
 * across the column.
 */
void JavaChunkData::recomputeHeights(LevelChunk* chunk) {
	if(!chunk || !chunk->tiles) return;

	int32_t lowest = LevelHeight::maxY();
	for(int32_t x = 0; x < 16; ++x) {
		for(int32_t z = 0; z < 16; ++z) {
			int32_t base = m8Index(x, 0, z);
			int32_t y = LevelHeight::maxY();
			while(y > 0 && !Tile::lightBlock[chunk->tiles[base + y - 1]]) {
				--y;
			}
			chunk->heightMap[x | (16 * z)] = (uint16_t)y;
			if(y < lowest) lowest = y;
		}
	}
	chunk->topBlockY = lowest;
}
