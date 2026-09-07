#include <level/biome/RainforestBiome.hpp>
#include <level/gen/feature/TreeFeature.hpp>
#include <level/gen/feature/MegaJungleTreeFeature.hpp>
#include <util/Random.hpp>

RainforestBiome::RainforestBiome() : Biome() {
}

RainforestBiome::~RainforestBiome() {
}

Feature* RainforestBiome::getTreeFeature(Random*) {
	return new TreeFeature(0, 3);
}
