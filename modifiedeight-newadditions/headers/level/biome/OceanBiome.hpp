#pragma once
#include <level/biome/Biome.hpp>

struct Random;
struct OceanBiome : public Biome {
	OceanBiome();
	virtual ~OceanBiome();
	virtual Feature* getTreeFeature(Random*);
	virtual Feature* getGrassFeature(Random*);
};
