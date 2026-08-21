#pragma once
#include <_types.h>

struct LevelSettings
{
	int32_t seed, gameType;
	int32_t generatorType; // 0 = Old (finite 256x256), 1 = Infinite, 2 = Flat, 3 = New Inf, 4 = New Old
	bool_t generateCaves;
	bool_t spawnMonsters;
	bool_t spawnAnimals;
	bool_t timeFreeze;

	LevelSettings(int32_t s = 0, int32_t gt = 0, int32_t gen = 0, bool_t caves = 1, bool_t monsters = 1, bool_t animals = 1, bool_t tf = 0)
		: seed(s), gameType(gt), generatorType(gen),
		  generateCaves(caves), spawnMonsters(monsters), spawnAnimals(animals), timeFreeze(tf) {}
};
