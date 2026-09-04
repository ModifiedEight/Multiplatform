#include <level/biome/EquatorialRainforestBiome.hpp>
#include <level/gen/feature/EquatorialTreeFeature.hpp>
#include <level/gen/feature/TallgrassFeature.hpp>
#include <tile/Tile.hpp>
#include <util/Random.hpp>

EquatorialRainforestBiome::EquatorialRainforestBiome() {
	this->leafColor = 0x2A621E;
	this->topBlock = Tile::grass->blockID;
	this->fillerBlock = Tile::dirt->blockID;
	this->temperature = 1.0f;
	this->downfall = 1.0f;
	this->creatureVec.emplace_back(Biome::MobSpawnerData(12, 22, 1, 3));
}

EquatorialRainforestBiome::~EquatorialRainforestBiome() {
}

Feature* EquatorialRainforestBiome::getTreeFeature(Random* rand) {
	return new EquatorialTreeFeature(0);
}

Feature* EquatorialRainforestBiome::getGrassFeature(Random* rand) {
	int r = rand->genrand_int32() % 10;
	if (r < 5) {
		return new TallgrassFeature(Tile::tallgrass->blockID, 2);
	} else if (r < 7) {
		return new TallgrassFeature(Tile::tallgrass->blockID, 1);
	}
	return new TallgrassFeature(Tile::flowerOrchid ? Tile::flowerOrchid->blockID : Tile::tallgrass->blockID, 0);
}

float EquatorialRainforestBiome::adjustDepth(float d) {
	return -0.5f;
}

float EquatorialRainforestBiome::adjustScale(float s) {
	return 0.05f;
}
