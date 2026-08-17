#include <level/gen/feature/SpringFeature.hpp>
#include <level/Level.hpp>
#include <tile/Tile.hpp>

SpringFeature::SpringFeature(int32_t id)
	: Feature(0) {
	this->id = id;
}
SpringFeature::~SpringFeature() {
}
bool_t SpringFeature::place(Level* level, Random* random, int32_t x, int32_t y, int32_t z){
	int32_t solidCount = 0;
	int32_t airCount = 0;
	Tile* tileObj;

	if (y <= 1 || y >= 126) return 0;
	int32_t tUp = level->getTile(x, y + 1, z);
	int32_t tDown = level->getTile(x, y - 1, z);
	if (!Tile::tiles[tUp] || !Tile::tiles[tUp]->isSolidRender() || !Tile::tiles[tDown] || !Tile::tiles[tDown]->isSolidRender()) {
		return 0;
	}

	int32_t cur = level->getTile(x, y, z);
	if (cur != 0 && (!Tile::tiles[cur] || !Tile::tiles[cur]->isSolidRender())) {
		return 0;
	}

	int32_t neighbors[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
	for (int i = 0; i < 4; ++i) {
		int32_t nx = x + neighbors[i][0];
		int32_t nz = z + neighbors[i][1];
		int32_t nt = level->getTile(nx, y, nz);
		if (Tile::tiles[nt] && Tile::tiles[nt]->isSolidRender()) {
			++solidCount;
		} else if (level->isEmptyTile(nx, y, nz)) {
			++airCount;
		}
	}

	if (solidCount >= 3 && airCount == 1) {
		level->setTileAndData(x, y, z, this->id, 0, 3);
		tileObj = Tile::tiles[this->id];
		if (tileObj) {
			tileObj->onPlace(level, x, y, z);
			level->addToTickNextTick(x, y, z, this->id, tileObj->getTickDelay());
			level->updateNeighborsAt(x, y, z, this->id);
		}
	}
	return 1;
}
