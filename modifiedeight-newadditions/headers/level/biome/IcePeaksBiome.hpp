#pragma once
#include <level/biome/Biome.hpp>
struct Random;
struct IcePeaksBiome : public Biome {
	IcePeaksBiome();
	~IcePeaksBiome();
	Feature* getTreeFeature(Random* rand) { return 0; }
};
