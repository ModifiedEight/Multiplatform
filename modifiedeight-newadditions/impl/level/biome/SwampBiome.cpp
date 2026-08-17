#include <level/biome/SwampBiome.hpp>
#include <level/gen/feature/TreeFeature.hpp>
#include <level/gen/feature/TallgrassFeature.hpp>
#include <tile/Tile.hpp>
#include <util/Random.hpp>

SwampBiome::SwampBiome() {
	this->leafColor = 0x6A7039;
	this->topBlock = Tile::grass->blockID;
	this->fillerBlock = Tile::dirt->blockID;
}

SwampBiome::~SwampBiome() {
}

Feature* SwampBiome::getTreeFeature(Random* rand) {
	return new TreeFeature(0, 4);
}

Feature* SwampBiome::getGrassFeature(Random* rand) {
	if (rand->genrand_int32() % 4 == 0) {
		return new TallgrassFeature(Tile::flowerOrchid ? Tile::flowerOrchid->blockID : Tile::tallgrass->blockID, 0);
	}
	return new TallgrassFeature(Tile::tallgrass->blockID, 1);
}
