#include <level/biome/RainforestBiome.hpp>
#include <level/gen/feature/TreeFeature.hpp>
#include <level/gen/feature/MegaJungleTreeFeature.hpp>
#include <level/gen/feature/JungleBushFeature.hpp>
#include <util/Random.hpp>

RainforestBiome::RainforestBiome() : Biome() {
}

RainforestBiome::~RainforestBiome() {
}

Feature* RainforestBiome::getTreeFeature(Random* a2) {
	int32_t r = a2->genrand_int32() % 10;
	if (r < 2) {
		return new MegaJungleTreeFeature(0, 12, 3, 3);
	}
	if (r < 6) {
		return new JungleBushFeature(3, 3);
	}
	return new TreeFeature(0, 3);
}
