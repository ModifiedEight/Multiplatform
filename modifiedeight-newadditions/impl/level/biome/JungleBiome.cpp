#include <level/biome/JungleBiome.hpp>
#include <level/gen/feature/TreeFeature.hpp>
#include <util/Random.hpp>

JungleBiome::JungleBiome() : Biome() {
}

JungleBiome::~JungleBiome() {
}
Feature* JungleBiome::getTreeFeature(Random* a2) {
	// jungle uses jungle wood (meta 3)
	a2->genrand_int32();
	return new TreeFeature(0, 3);
}
