#pragma once
#include <_types.h>

/*
 * JavaIdMap - ID translation between Minecraft Java Edition 1.8 and m8.
 *
 * This is the single place that knows the two ID spaces are different, and it
 * has to be a real table rather than the identity: modifiedeight-newadditions
 * has claimed a lot of the higher vanilla block IDs for its own blocks.
 * Java 115-122 are nether wart / enchanting table / brewing stand / ... while
 * in m8 those same IDs are coloured logs; Java 174-189 are packed ice, double
 * plants, banners and red sandstone while in m8 they are coloured fences; and
 * so on for 125-137, 143-166, 159-162, 190-197. Feeding a Java block ID
 * straight into Tile would fill the world with coloured fences, so every ID is
 * mapped explicitly.
 *
 * Where m8 has no equivalent at all the mapping falls back to the closest
 * lookalike (packed ice -> ice, red sandstone -> sandstone, mycelium -> grass,
 * hopper -> iron block) and to air for purely technical or flat blocks
 * (redstone wire, tripwire, pistons extensions, banners).
 */
struct JavaIdMap
{
	// Java 1.8 block state -> m8 tile id + meta. Writes 0/0 for "nothing here".
	static void javaBlockToM8(int32_t javaId, int32_t javaMeta, int32_t* outId, int32_t* outMeta);
	// m8 tile -> Java 1.8 block state. Used when we have to name a block to the server.
	static void m8BlockToJava(int32_t m8Id, int32_t m8Meta, int32_t* outId, int32_t* outMeta);

	// Item ids. Both sides use 1..197 for block items and 256.. for real items.
	static int32_t javaItemToM8(int32_t javaItemId);
	static int32_t m8ItemToJava(int32_t m8ItemId);

	/*
	 * Mob types. m8 only implements nine mobs (chicken 10, cow 11, pig 12,
	 * sheep 13, zombie 32, creeper 33, skeleton 34, spider 35, pig zombie 36),
	 * so every Java mob is folded onto the nearest one - endermen and villagers
	 * become humanoids, slimes and magma cubes become creepers, bats and
	 * rabbits become chickens. Returns 0 when there is nothing sensible.
	 */
	static int32_t javaMobToM8(int32_t javaType);
	// Java "object" spawn types (SpawnObject) -> m8 EntityFactory types.
	static int32_t javaObjectToM8(int32_t javaObjectType);
};
