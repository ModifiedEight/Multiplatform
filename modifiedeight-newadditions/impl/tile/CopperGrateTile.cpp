#include <tile/CopperGrateTile.hpp>
#include <tile/material/Material.hpp>

CopperGrateTile::CopperGrateTile(int32_t id)
    : Tile(id, "copper_grate", Material::metal) {
    this->setLightBlock(0);
}

CopperGrateTile::~CopperGrateTile() {
}

bool_t CopperGrateTile::isSolidRender() {
    return 0;
}

bool_t CopperGrateTile::isCubeShaped() {
    return 1;
}

int32_t CopperGrateTile::getRenderLayer() {
    return 1;
}

bool_t CopperGrateTile::blocksLight() {
    return 0;
}
