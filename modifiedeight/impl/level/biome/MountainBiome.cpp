#include <level/biome/MountainBiome.hpp>
#include <level/biome/Biome.hpp>
#include <tile/Tile.hpp>

MountainBiome::MountainBiome() : Biome() {
	
	this->setName("Mountains");
	this->topBlock = Tile::rock->blockID;
	this->fillerBlock = Tile::rock->blockID;
	this->temperature = 0.3f;
}
MountainBiome::~MountainBiome() {}
