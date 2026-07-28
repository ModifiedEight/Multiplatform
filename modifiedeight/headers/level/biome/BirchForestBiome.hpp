#pragma once
#include <level/biome/Biome.hpp>
struct Random;
struct BirchForestBiome : public Biome {
	BirchForestBiome();
	~BirchForestBiome();
	Feature* getTreeFeature(Random*);
};
