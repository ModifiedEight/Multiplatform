#include <level/biome/IcePeaksBiome.hpp>
#include <tile/Tile.hpp>

IcePeaksBiome::IcePeaksBiome() : Biome() {
	this->topBlock = Tile::snow->blockID;
	this->fillerBlock = Tile::snow->blockID;
}

IcePeaksBiome::~IcePeaksBiome() {
}
