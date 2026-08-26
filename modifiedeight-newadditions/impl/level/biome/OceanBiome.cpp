#include <level/biome/OceanBiome.hpp>
#include <tile/Tile.hpp>

OceanBiome::OceanBiome() {
	this->topBlock = Tile::sand->blockID;
	this->fillerBlock = Tile::sand->blockID;
	this->clearMobs(1, 1, 0);
}

OceanBiome::~OceanBiome() {}

Feature* OceanBiome::getTreeFeature(Random*) {
	return 0;
}

Feature* OceanBiome::getGrassFeature(Random*) {
	return 0;
}
