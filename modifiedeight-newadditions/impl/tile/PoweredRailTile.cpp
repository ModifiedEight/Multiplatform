#include <tile/PoweredRailTile.hpp>
#include <level/Level.hpp>

PoweredRailTile::PoweredRailTile(int32_t id)
	: BaseRailTile(id, 1) {
	this->textureUV = this->getTextureUVCoordinateSet("rail_golden", 0);
	this->poweredRailTexture = this->getTextureUVCoordinateSet("rail_golden_powered", 0);
}
bool_t PoweredRailTile::findPoweredRailSignal(Level* level, int32_t x, int32_t y, int32_t z, bool_t forward, int32_t dir, int32_t step) {
	if(step >= 8) return 0;
	if(level->getTile(x, y, z) != this->blockID) return 0;
	int32_t data = level->getData(x, y, z);
	int32_t shape = data & 7;
	if(dir == 1 && (shape == 0 || shape == 4 || shape == 5)) return 0;
	if(dir == 0 && (shape == 1 || shape == 2 || shape == 3)) return 0;
	if((data & 8) != 0) {
		if(level->hasNeighborSignal(x, y, z)) return 1;
		return this->isSameRailWithPower(level, x, y, z, data, forward, step + 1);
	}
	return 0;
}
bool_t PoweredRailTile::isSameRailWithPower(Level* level, int32_t x, int32_t y, int32_t z, int32_t data, bool_t forward, int32_t step) {
	if(step >= 8) return 0;
	int32_t shape = data & 7;
	int32_t nx = x, ny = y, nz = z;
	bool slope = false;
	switch(shape) {
		case 0:
			if(forward) ++nz; else --nz;
			break;
		case 1:
			if(forward) --nx; else ++nx;
			break;
		case 2:
			if(forward) { --nx; } else { ++nx; ++ny; slope = true; }
			break;
		case 3:
			if(forward) { --nx; ++ny; slope = true; } else { ++nx; }
			break;
		case 4:
			if(forward) { ++nz; ++ny; slope = true; } else { --nz; }
			break;
		case 5:
			if(forward) { ++nz; } else { --nz; ++ny; slope = true; }
			break;
		default:
			return 0;
	}
	if(this->findPoweredRailSignal(level, nx, ny, nz, forward, (shape <= 1 ? shape : (shape <= 3 ? 1 : 0)), step)) return 1;
	if(slope && this->findPoweredRailSignal(level, nx, ny - 1, nz, forward, (shape <= 1 ? shape : (shape <= 3 ? 1 : 0)), step)) return 1;
	return 0;
}

PoweredRailTile::~PoweredRailTile() {
}
TextureUVCoordinateSet* PoweredRailTile::getTexture(int32_t) {
	return &this->poweredRailTexture;
}
TextureUVCoordinateSet* PoweredRailTile::getTexture(int32_t, int32_t) {
	return &this->poweredRailTexture;
}
void PoweredRailTile::updateState(Level* level, int32_t x, int32_t y, int32_t z, int32_t a6, int32_t a7, int32_t a8){
	bool powered = level->hasNeighborSignal(x, y, z) || this->findPoweredRailSignal(level, x, y, z, true, a7, 0) || this->findPoweredRailSignal(level, x, y, z, false, a7, 0);
	int32_t targetData = powered ? (a7 | 8) : a7;
	int32_t currentData = level->getData(x, y, z);
	if(currentData != targetData) {
		level->setData(x, y, z, targetData, 3);
		level->updateNeighborsAt(x, y - 1, z, this->blockID);
		if((uint32_t)(a7 - 2) <= 3) {
			level->updateNeighborsAt(x, y + 1, z, this->blockID);
		}
	}
}
