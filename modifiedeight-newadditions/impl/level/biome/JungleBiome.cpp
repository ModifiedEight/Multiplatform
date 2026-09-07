#include <level/biome/JungleBiome.hpp>
#include <level/gen/feature/TreeFeature.hpp>
#include <level/gen/feature/MegaJungleTreeFeature.hpp>
#include <level/gen/feature/TallgrassFeature.hpp>
#include <tile/Tile.hpp>
#include <util/Random.hpp>

JungleBiome::JungleBiome() : Biome() {
	this->creatureVec.emplace_back(Biome::MobSpawnerData(45, 22, 1, 3));
	this->monsterVec.emplace_back(Biome::MobSpawnerData(15, 22, 1, 1));
}

JungleBiome::~JungleBiome() {
}

Feature* JungleBiome::getTreeFeature(Random*) {
	return new TreeFeature(0, 3);
}

Feature* JungleBiome::getGrassFeature(Random* rand) {
	if (rand->genrand_int32() % 4 == 0) {
		return new TallgrassFeature(Tile::tallgrass->blockID, 2);
	}
	return new TallgrassFeature(Tile::tallgrass->blockID, 1);
}
