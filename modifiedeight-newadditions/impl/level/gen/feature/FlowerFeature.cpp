#include <level/gen/feature/FlowerFeature.hpp>
#include <util/Random.hpp>
#include <level/Level.hpp>
#include <tile/Tile.hpp>

FlowerFeature::FlowerFeature(int32_t id)
	: Feature(0) {
	this->id = id;
}

FlowerFeature::~FlowerFeature() {
}
bool_t FlowerFeature::place(Level* level, Random* random, int32_t x, int32_t y, int32_t z){
	int32_t v9;
	int32_t v10;
	int32_t v11;
	int32_t v12;
	int32_t v13;
	int32_t v14;
	int32_t v15;
	Tile* v16;

	v9 = 64;
	do {
		v10 = x + (random->genrand_int32() & 7);
		v11 = v10 - (random->genrand_int32() & 7);
		v12 = y + (random->genrand_int32() & 3);
		v13 = v12 - (random->genrand_int32() & 3);
		v14 = z + (random->genrand_int32() & 7);
		v15 = v14 - (random->genrand_int32() & 7);
		if(level->isEmptyTile(v11, v13, v15)) {
			v16 = Tile::tiles[this->id];
			if(v16->canSurvive(level, v11, v13, v15)) {
				int32_t r = random->genrand_int32() % 10;
				int32_t flowerData = 0;
				if (r < 4) flowerData = 0;
				else if (r < 7) flowerData = 1;
				else if (r < 9) flowerData = 2;
				else flowerData = 3;
				level->setTileAndDataNoUpdate(v11, v13, v15, this->id, flowerData);
			}
		}
		--v9;
	} while(v9);
	return 1;
}
