#include <tile/RedstoneLampTile.hpp>
#include <level/Level.hpp>
#include <level/LightLayer.hpp>
#include <tile/material/Material.hpp>

RedstoneLampTile::RedstoneLampTile(int32_t id, bool_t isOn)
    : Tile(id, isOn ? "redstone_lamp_on" : "redstone_lamp_off", Material::clay), isOn(isOn) {
    this->setLightBlock(0);
    if (isOn) {
        this->setLightEmission(1.0f);
    }
}

RedstoneLampTile::~RedstoneLampTile() {
}

void RedstoneLampTile::onPlace(Level* level, int32_t x, int32_t y, int32_t z) {
    bool_t hasSignal = level->hasNeighborSignal(x, y, z);
    if (this->isOn && !hasSignal) {
        level->setTileAndData(x, y, z, Tile::redstoneLampOff->blockID, 0, 3);
        level->updateLight(LightLayer::Block, x - 15, y - 15, z - 15, x + 15, y + 15, z + 15);
    } else if (!this->isOn && hasSignal) {
        level->setTileAndData(x, y, z, Tile::redstoneLampOn->blockID, 0, 3);
        level->updateLight(LightLayer::Block, x - 15, y - 15, z - 15, x + 15, y + 15, z + 15);
    }
}

void RedstoneLampTile::neighborChanged(Level* level, int32_t x, int32_t y, int32_t z, int32_t ox, int32_t oy, int32_t oz, int32_t tileId) {
    bool_t hasSignal = level->hasNeighborSignal(x, y, z);
    if (this->isOn && !hasSignal) {
        level->setTileAndData(x, y, z, Tile::redstoneLampOff->blockID, 0, 3);
        level->updateLight(LightLayer::Block, x - 15, y - 15, z - 15, x + 15, y + 15, z + 15);
    } else if (!this->isOn && hasSignal) {
        level->setTileAndData(x, y, z, Tile::redstoneLampOn->blockID, 0, 3);
        level->updateLight(LightLayer::Block, x - 15, y - 15, z - 15, x + 15, y + 15, z + 15);
    }
}

int32_t RedstoneLampTile::getResource(int32_t meta, Random* random) {
    return Tile::redstoneLampOff->blockID;
}

int32_t RedstoneLampTile::getResourceCount(Random* random) {
    return 1;
}
