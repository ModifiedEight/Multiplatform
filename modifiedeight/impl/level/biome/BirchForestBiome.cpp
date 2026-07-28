#include <level/biome/BirchForestBiome.hpp>
#include <level/gen/feature/BirchFeature.hpp>
#include <level/gen/feature/TreeFeature.hpp>
#include <util/Random.hpp>

BirchForestBiome::BirchForestBiome() : Biome() {
}

BirchForestBiome::~BirchForestBiome() {
}
Feature* BirchForestBiome::getTreeFeature(Random* a2) {
	if(a2->genrand_int32() % 3) {
		return new BirchFeature(0);
	}
	return new TreeFeature(0, 2); // fallback meta 2
}
