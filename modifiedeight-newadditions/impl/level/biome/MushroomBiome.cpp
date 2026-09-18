#include <level/biome/MushroomBiome.hpp>
#include <level/gen/feature/HugeMushroomFeature.hpp>
#include <tile/Tile.hpp>
#include <util/Random.hpp>

MushroomBiome::MushroomBiome()
	: Biome() {
	this->topBlock = Tile::mycelium ? Tile::mycelium->blockID : Tile::grass->blockID;
	this->fillerBlock = Tile::dirt->blockID;
	this->clearMobs(1, 0, 1);
	this->monsterVec.emplace_back(Biome::MobSpawnerData(100, 37, 2, 6));
	this->creatureVec.emplace_back(Biome::MobSpawnerData(100, 37, 2, 6));
}

MushroomBiome::~MushroomBiome() {
}

Feature* MushroomBiome::getTreeFeature(Random* rand) {
	return new HugeMushroomFeature(-1);
}

Feature* MushroomBiome::getGrassFeature(Random* rand) {
	return 0;
}

