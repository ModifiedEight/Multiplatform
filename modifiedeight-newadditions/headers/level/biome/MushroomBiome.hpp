#pragma once
#include <level/biome/Biome.hpp>

struct MushroomBiome : public Biome {
	MushroomBiome();
	virtual ~MushroomBiome();
	virtual Feature* getTreeFeature(Random* rand);
	virtual Feature* getGrassFeature(Random* rand);
};

