#include <level/gen/feature/ReedsFeature.hpp>
#include <util/Random.hpp>
#include <tile/material/Material.hpp>
#include <level/Level.hpp>
#include <tile/Tile.hpp>

ReedsFeature::ReedsFeature() : Feature(0){
}

ReedsFeature::~ReedsFeature() {
}

bool_t ReedsFeature::place(Level* level, Random* random, int32_t x, int32_t y, int32_t z) {
	for(int32_t i = 0; i < 20; ++i) {
		int32_t px = x + (random->genrand_int32() & 3) - (random->genrand_int32() & 3);
		int32_t pz = z + (random->genrand_int32() & 3) - (random->genrand_int32() & 3);
		if(level->isEmptyTile(px, y, pz)) {
			int32_t ground = level->getTile(px, y - 1, pz);
			bool isWaterAdjacent = (Material::water == level->getMaterial(px - 1, y - 1, pz) ||
			                        Material::water == level->getMaterial(px + 1, y - 1, pz) ||
			                        Material::water == level->getMaterial(px, y - 1, pz - 1) ||
			                        Material::water == level->getMaterial(px, y - 1, pz + 1));
			if(isWaterAdjacent && (ground == Tile::sand->blockID || ground == Tile::dirt->blockID || ground == Tile::grass->blockID)) {
				int32_t maxHeight = 1 + (random->genrand_int32() % 3);
				if((random->genrand_int32() % 10) == 0) {
					maxHeight = 4;
				}
				for(int32_t h = 0; h < maxHeight; ++h) {
					if(level->isEmptyTile(px, y + h, pz) && Tile::reeds->canSurvive(level, px, y + h, pz)) {
						level->setTileNoUpdate(px, y + h, pz, Tile::reeds->blockID);
					}
				}
			}
		}
	}
	return 1;
}
