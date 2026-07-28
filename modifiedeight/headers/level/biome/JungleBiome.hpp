#pragma once
#include <level/biome/Biome.hpp>
struct Random;
struct JungleBiome : public Biome {
	JungleBiome();
	~JungleBiome();
	Feature* getTreeFeature(Random*);
};
