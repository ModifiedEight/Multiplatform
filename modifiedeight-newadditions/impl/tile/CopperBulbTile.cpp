#include <tile/CopperBulbTile.hpp>
#include <level/Level.hpp>
#include <level/LightLayer.hpp>
#include <tile/material/Material.hpp>

CopperBulbTile::CopperBulbTile(int32_t id, bool_t isOn)
    : Tile(id, isOn ? "copper_bulb_lit" : "copper_bulb", Material::metal), isOn(isOn) {
    this->setLightBlock(0);
    if (isOn) {
        this->setLightEmission(1.0f);
    }
}

CopperBulbTile::~CopperBulbTile() {
}

void CopperBulbTile::onPlace(Level* level, int32_t x, int32_t y, int32_t z) {
    bool_t hasSignal = level->hasNeighborSignal(x, y, z);
    if (this->isOn && !hasSignal) {
        level->setTileAndData(x, y, z, Tile::copperBulb->blockID, 0, 3);
        level->updateLight(LightLayer::Block, x - 15, y - 15, z - 15, x + 15, y + 15, z + 15);
    } else if (!this->isOn && hasSignal) {
        level->setTileAndData(x, y, z, Tile::copperBulbLit->blockID, 0, 3);
        level->updateLight(LightLayer::Block, x - 15, y - 15, z - 15, x + 15, y + 15, z + 15);
    }
}

void CopperBulbTile::neighborChanged(Level* level, int32_t x, int32_t y, int32_t z, int32_t ox, int32_t oy, int32_t oz, int32_t tileId) {
    bool_t hasSignal = level->hasNeighborSignal(x, y, z);
    if (this->isOn && !hasSignal) {
        level->setTileAndData(x, y, z, Tile::copperBulb->blockID, 0, 3);
        level->updateLight(LightLayer::Block, x - 15, y - 15, z - 15, x + 15, y + 15, z + 15);
        level->playSound((float)x + 0.5f, (float)y + 0.5f, (float)z + 0.5f, "block.copper_bulb.turn_off", 1.0f, 1.0f);
    } else if (!this->isOn && hasSignal) {
        level->setTileAndData(x, y, z, Tile::copperBulbLit->blockID, 0, 3);
        level->updateLight(LightLayer::Block, x - 15, y - 15, z - 15, x + 15, y + 15, z + 15);
        level->playSound((float)x + 0.5f, (float)y + 0.5f, (float)z + 0.5f, "block.copper_bulb.turn_on", 1.0f, 1.0f);
    }
}

int32_t CopperBulbTile::getResource(int32_t meta, Random* random) {
    return Tile::copperBulb ? Tile::copperBulb->blockID : this->blockID;
}

int32_t CopperBulbTile::getResourceCount(Random* random) {
    return 1;
}
