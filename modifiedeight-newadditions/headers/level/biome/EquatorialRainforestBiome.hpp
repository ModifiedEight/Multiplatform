#pragma once
#include <level/biome/Biome.hpp>

struct EquatorialRainforestBiome : public Biome {
	EquatorialRainforestBiome();
	virtual ~EquatorialRainforestBiome();
	virtual Feature* getTreeFeature(Random*);
	virtual Feature* getGrassFeature(Random*);
	virtual float adjustDepth(float);
	virtual float adjustScale(float);
};
