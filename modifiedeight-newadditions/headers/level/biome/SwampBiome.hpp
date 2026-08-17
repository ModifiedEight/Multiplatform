#pragma once
#include <level/biome/Biome.hpp>

struct SwampBiome : public Biome {
	SwampBiome();
	virtual ~SwampBiome();
	virtual Feature* getTreeFeature(Random*);
	virtual Feature* getGrassFeature(Random*);
};
