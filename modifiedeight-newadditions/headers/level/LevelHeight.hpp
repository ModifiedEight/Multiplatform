#pragma once
#include <_types.h>

/*
 * World height, and the shape of a chunk column.
 *
 * m8 is a 128 tall game: its region files, its MCPE network protocol and its
 * level generators all deal in 16x16x128 columns, and a pile of tile and
 * lighting code has 127 written straight into it.  A Minecraft Java 1.8
 * server, on the other hand, always sends 256 tall columns.
 *
 * So the height is a runtime value, and it is 128 for absolutely everything
 * except a live crossplay session against a Java server:
 *
 *   height    128                          256
 *   used by   single player worlds,         a Java (1.8.x) session only
 *             MCPE servers, MCPE clients
 *   layout    y | (x << 11) | (z << 7)     y | (x << 12) | (z << 8)
 *   column    32768 tiles, 16384 nibbles   65536 tiles, 32768 nibbles
 *
 * At 128 that is byte for byte the layout the game has always used, which is
 * what keeps saved worlds safe: the region file format is the raw column
 * buffer, and in a saved world the buffer is exactly the shape it always was.
 * Nothing is converted, nothing is padded, and a world saved by this build is
 * the same file a world saved by any other build is.  The tall layout only ever
 * exists in memory, for a level that is never written to disk.
 *
 * One value for the whole process is enough: m8 holds at most one level at a
 * time, and set() is called immediately before that level is built.
 */

// ---- the MCPE column: what the region files and the MCPE packets speak ------
#define MCPE_HEIGHT  128
#define MCPE_X_SHIFT 11
#define MCPE_Z_SHIFT 7
#define MCPE_TILES   (16 * 16 * MCPE_HEIGHT) /* 32768 */
#define MCPE_NIBBLES (MCPE_TILES / 2)        /* 16384 */

// ---- the tall column a Java server sends ------------------------------------
#define JAVA_HEIGHT  256
#define JAVA_X_SHIFT 12
#define JAVA_Z_SHIFT 8
#define JAVA_TILES   (16 * 16 * JAVA_HEIGHT) /* 65536 */

// LevelChunk::miniChunkEntities is bucketed per 16 blocks of height and is
// sized for the tall case either way: eight spare empty vectors per chunk is
// not worth making the bucket count dynamic over.
#define CHUNK_MINIS (JAVA_HEIGHT / 16) /* 16 */

struct LevelHeight {
	// 128 (everything) or 256 (a Java session). Never anything else.
	static int32_t height;
	// The x and z shifts of the current column layout: 11/7 at 128, 12/8 at 256.
	static int32_t xShift;
	static int32_t zShift;
	// Tiles per chunk, and the DataLayer size that goes with it.
	static int32_t tiles;

	static void set(int32_t h);

	// The highest y a block may occupy.
	static int32_t maxY() {
		return LevelHeight::height - 1;
	}
	static bool_t inRange(int32_t y) {
		return (uint32_t)y < (uint32_t)LevelHeight::height;
	}
	static bool_t isTall() {
		return LevelHeight::height > MCPE_HEIGHT;
	}
	static int32_t index(int32_t x, int32_t y, int32_t z) {
		return y | (x << LevelHeight::xShift) | (z << LevelHeight::zShift);
	}
};
