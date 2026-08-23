#include <tile/ButtonTile.hpp>
#include <level/Level.hpp>
#include <entity/Player.hpp>
#include <tile/material/Material.hpp>
#include <tile/entity/MixedSlabTileEntity.hpp>

static bool isButtonSupportValid(Level* level, int32_t x, int32_t y, int32_t z, int32_t side) {
    if (!level) return false;
    if (side == 1) {
        if (level->isSolidBlockingTile(x - 1, y, z)) return true;
        int32_t t = level->getTile(x - 1, y, z);
        if (Tile::tiles[t] && Tile::tiles[t]->isSolidRender()) return true;
        if (Tile::mixedSlab && t == Tile::mixedSlab->blockID) {
            MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(x - 1, y, z);
            if (te) return (te->bottomTileId != 0 || te->topTileId != 0);
        }
    } else if (side == 2) {
        if (level->isSolidBlockingTile(x + 1, y, z)) return true;
        int32_t t = level->getTile(x + 1, y, z);
        if (Tile::tiles[t] && Tile::tiles[t]->isSolidRender()) return true;
        if (Tile::mixedSlab && t == Tile::mixedSlab->blockID) {
            MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(x + 1, y, z);
            if (te) return (te->bottomTileId != 0 || te->topTileId != 0);
        }
    } else if (side == 3) {
        if (level->isSolidBlockingTile(x, y, z - 1)) return true;
        int32_t t = level->getTile(x, y, z - 1);
        if (Tile::tiles[t] && Tile::tiles[t]->isSolidRender()) return true;
        if (Tile::mixedSlab && t == Tile::mixedSlab->blockID) {
            MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(x, y, z - 1);
            if (te) return (te->bottomTileId != 0 || te->topTileId != 0);
        }
    } else if (side == 4) {
        if (level->isSolidBlockingTile(x, y, z + 1)) return true;
        int32_t t = level->getTile(x, y, z + 1);
        if (Tile::tiles[t] && Tile::tiles[t]->isSolidRender()) return true;
        if (Tile::mixedSlab && t == Tile::mixedSlab->blockID) {
            MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(x, y, z + 1);
            if (te) return (te->bottomTileId != 0 || te->topTileId != 0);
        }
    } else if (side == 5) {
        if (level->isSolidBlockingTile(x, y - 1, z) || level->isTopSolidBlocking(x, y - 1, z)) return true;
        int32_t t = level->getTile(x, y - 1, z);
        if (Tile::tiles[t] && Tile::tiles[t]->isSolidRender()) return true;
        if (Tile::mixedSlab && t == Tile::mixedSlab->blockID) {
            MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(x, y - 1, z);
            if (te) return (te->bottomTileId != 0 || te->topTileId != 0);
        }
    } else if (side == 0) {
        if (level->isSolidBlockingTile(x, y + 1, z)) return true;
        int32_t t = level->getTile(x, y + 1, z);
        if (Tile::tiles[t] && Tile::tiles[t]->isSolidRender()) return true;
        if (Tile::mixedSlab && t == Tile::mixedSlab->blockID) {
            MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(x, y + 1, z);
            if (te) return (te->bottomTileId != 0 || te->topTileId != 0);
        }
    }
    return false;
}

ButtonTile::ButtonTile(int32_t id, const std::string& texName, int32_t texAux, const Material* mat, int32_t delay)
    : Tile(id, mat), textureName(texName), textureAux(texAux), delayTicks(delay) {
    this->textureUV = Tile::getTextureUVCoordinateSet(texName, texAux);
    this->setShape(0.3125f, 0.375f, 0.375f, 0.6875f, 0.625f, 0.625f);
}

ButtonTile::~ButtonTile() {}

bool_t ButtonTile::isCubeShaped() { return 0; }

bool_t ButtonTile::isSolidRender() { return 0; }

AABB* ButtonTile::getAABB(Level* level, int32_t x, int32_t y, int32_t z) {
    return 0;
}

void ButtonTile::updateDefaultShape() {
    this->setShape(0.3125f, 0.375f, 0.375f, 0.6875f, 0.625f, 0.625f);
}

void ButtonTile::updateShape(LevelSource* level, int32_t x, int32_t y, int32_t z) {
    int32_t meta = level->getData(x, y, z);
    int32_t dir = meta & 7;
    bool_t active = (meta & 8) != 0;
    float t = active ? 0.0625f : 0.125f;

    float offset = 0.0f;
    if (Tile::mixedSlab) {
        if (dir == 1) {
            int32_t tid = level->getTile(x - 1, y, z);
            if (tid == Tile::mixedSlab->blockID) {
                MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(x - 1, y, z);
                if (te && te->mode == 2 && te->bottomTileId != 0 && te->topTileId == 0) offset = -0.5f;
            }
        } else if (dir == 2) {
            int32_t tid = level->getTile(x + 1, y, z);
            if (tid == Tile::mixedSlab->blockID) {
                MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(x + 1, y, z);
                if (te && te->mode == 2 && te->bottomTileId == 0 && te->topTileId != 0) offset = 0.5f;
            }
        } else if (dir == 3) {
            int32_t tid = level->getTile(x, y, z - 1);
            if (tid == Tile::mixedSlab->blockID) {
                MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(x, y, z - 1);
                if (te && te->mode == 1 && te->bottomTileId != 0 && te->topTileId == 0) offset = -0.5f;
            }
        } else if (dir == 4) {
            int32_t tid = level->getTile(x, y, z + 1);
            if (tid == Tile::mixedSlab->blockID) {
                MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(x, y, z + 1);
                if (te && te->mode == 1 && te->bottomTileId == 0 && te->topTileId != 0) offset = 0.5f;
            }
        } else if (dir == 5 || dir == 6) {
            int32_t tid = level->getTile(x, y - 1, z);
            if (tid == Tile::mixedSlab->blockID) {
                MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(x, y - 1, z);
                if (te && te->mode == 0 && te->bottomTileId != 0 && te->topTileId == 0) offset = -0.5f;
            }
        } else if (dir == 0 || dir == 7) {
            int32_t tid = level->getTile(x, y + 1, z);
            if (tid == Tile::mixedSlab->blockID) {
                MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(x, y + 1, z);
                if (te && te->mode == 0 && te->bottomTileId == 0 && te->topTileId != 0) offset = 0.5f;
            }
        }
    }

    if (dir == 1) this->setShape(0.0f + offset, 0.375f, 0.3125f, t + offset, 0.625f, 0.6875f);
    else if (dir == 2) this->setShape(1.0f - t + offset, 0.375f, 0.3125f, 1.0f + offset, 0.625f, 0.6875f);
    else if (dir == 3) this->setShape(0.3125f, 0.375f, 0.0f + offset, 0.6875f, 0.625f, t + offset);
    else if (dir == 4) this->setShape(0.3125f, 0.375f, 1.0f - t + offset, 0.6875f, 0.625f, 1.0f + offset);
    else if (dir == 5 || dir == 6) this->setShape(0.3125f, 0.0f + offset, 0.375f, 0.6875f, t + offset, 0.625f);
    else if (dir == 0 || dir == 7) this->setShape(0.3125f, 1.0f - t + offset, 0.375f, 0.6875f, 1.0f + offset, 0.625f);
    else this->setShape(0.3125f, 0.375f, 0.375f, 0.6875f, 0.625f, 0.625f);
}

int32_t ButtonTile::getRenderShape() { return 12; }

bool_t ButtonTile::mayPlace(Level* level, int32_t x, int32_t y, int32_t z) {
    return isButtonSupportValid(level, x, y, z, 1) ||
           isButtonSupportValid(level, x, y, z, 2) ||
           isButtonSupportValid(level, x, y, z, 3) ||
           isButtonSupportValid(level, x, y, z, 4) ||
           isButtonSupportValid(level, x, y, z, 5) ||
           isButtonSupportValid(level, x, y, z, 0);
}

bool_t ButtonTile::mayPlace(Level* level, int32_t x, int32_t y, int32_t z, uint8_t side) {
    if (side == 0 && isButtonSupportValid(level, x, y, z, 0)) return 1;
    if (side == 1 && isButtonSupportValid(level, x, y, z, 5)) return 1;
    if (side == 2 && isButtonSupportValid(level, x, y, z, 4)) return 1;
    if (side == 3 && isButtonSupportValid(level, x, y, z, 3)) return 1;
    if (side == 4 && isButtonSupportValid(level, x, y, z, 2)) return 1;
    if (side == 5 && isButtonSupportValid(level, x, y, z, 1)) return 1;
    return 0;
}

int32_t ButtonTile::getPlacementDataValue(Level* level, int32_t x, int32_t y, int32_t z, int32_t side, float fx, float fy, float fz, Mob* mob, int32_t meta) {
    int32_t face = side & 7;
    if (face == 0 && isButtonSupportValid(level, x, y, z, 0)) return 0;
    if (face == 1 && isButtonSupportValid(level, x, y, z, 5)) return 5;
    if (face == 2 && isButtonSupportValid(level, x, y, z, 4)) return 4;
    if (face == 3 && isButtonSupportValid(level, x, y, z, 3)) return 3;
    if (face == 4 && isButtonSupportValid(level, x, y, z, 2)) return 2;
    if (face == 5 && isButtonSupportValid(level, x, y, z, 1)) return 1;
    return 1;
}

void ButtonTile::neighborChanged(Level* level, int32_t x, int32_t y, int32_t z, int32_t ox, int32_t oy, int32_t oz, int32_t tileId) {
    int32_t meta = level->getData(x, y, z);
    int32_t dir = meta & 7;
    bool_t drop = 0;
    if (dir == 1 && !isButtonSupportValid(level, x, y, z, 1)) drop = 1;
    if (dir == 2 && !isButtonSupportValid(level, x, y, z, 2)) drop = 1;
    if (dir == 3 && !isButtonSupportValid(level, x, y, z, 3)) drop = 1;
    if (dir == 4 && !isButtonSupportValid(level, x, y, z, 4)) drop = 1;
    if ((dir == 5 || dir == 6) && !isButtonSupportValid(level, x, y, z, 5)) drop = 1;
    if ((dir == 0 || dir == 7) && !isButtonSupportValid(level, x, y, z, 0)) drop = 1;

    if (drop) {
        this->spawnResources(level, x, y, z, meta, 1.0f);
        level->setTile(x, y, z, 0, 3);
    }
}

bool_t ButtonTile::use(Level* level, int32_t x, int32_t y, int32_t z, Player* player) {
    int32_t meta = level->getData(x, y, z);
    int32_t dir = meta & 7;
    if (meta & 8) return 1;

    level->setData(x, y, z, dir | 8, 3);
    level->playSound((float)x + 0.5f, (float)y + 0.5f, (float)z + 0.5f, "random.click", 0.3f, 0.6f);
    level->sendTileUpdated(x, y, z);

    level->updateNeighborsAt(x, y, z, this->blockID);
    if (dir == 1) level->updateNeighborsAt(x - 1, y, z, this->blockID);
    else if (dir == 2) level->updateNeighborsAt(x + 1, y, z, this->blockID);
    else if (dir == 3) level->updateNeighborsAt(x, y, z - 1, this->blockID);
    else if (dir == 4) level->updateNeighborsAt(x, y, z + 1, this->blockID);
    else if (dir == 5 || dir == 6) level->updateNeighborsAt(x, y - 1, z, this->blockID);
    else if (dir == 0 || dir == 7) level->updateNeighborsAt(x, y + 1, z, this->blockID);

    level->addToTickNextTick(x, y, z, this->blockID, this->delayTicks);
    return 1;
}

void ButtonTile::tick(Level* level, int32_t x, int32_t y, int32_t z, Random* rand) {
    int32_t meta = level->getData(x, y, z);
    int32_t dir = meta & 7;
    if (meta & 8) {
        level->setData(x, y, z, dir, 3);
        level->playSound((float)x + 0.5f, (float)y + 0.5f, (float)z + 0.5f, "random.click", 0.3f, 0.5f);
        level->sendTileUpdated(x, y, z);

        level->updateNeighborsAt(x, y, z, this->blockID);
        if (dir == 1) level->updateNeighborsAt(x - 1, y, z, this->blockID);
        else if (dir == 2) level->updateNeighborsAt(x + 1, y, z, this->blockID);
        else if (dir == 3) level->updateNeighborsAt(x, y, z - 1, this->blockID);
        else if (dir == 4) level->updateNeighborsAt(x, y, z + 1, this->blockID);
        else if (dir == 5 || dir == 6) level->updateNeighborsAt(x, y - 1, z, this->blockID);
        else if (dir == 0 || dir == 7) level->updateNeighborsAt(x, y + 1, z, this->blockID);
    }
}

void ButtonTile::onRemove(Level* level, int32_t x, int32_t y, int32_t z) {
    int32_t meta = level->getData(x, y, z);
    int32_t dir = meta & 7;
    if (meta & 8) {
        level->updateNeighborsAt(x, y, z, this->blockID);
        if (dir == 1) level->updateNeighborsAt(x - 1, y, z, this->blockID);
        else if (dir == 2) level->updateNeighborsAt(x + 1, y, z, this->blockID);
        else if (dir == 3) level->updateNeighborsAt(x, y, z - 1, this->blockID);
        else if (dir == 4) level->updateNeighborsAt(x, y, z + 1, this->blockID);
        else if (dir == 5 || dir == 6) level->updateNeighborsAt(x, y - 1, z, this->blockID);
        else if (dir == 0 || dir == 7) level->updateNeighborsAt(x, y + 1, z, this->blockID);
    }
    Tile::onRemove(level, x, y, z);
}

bool_t ButtonTile::isSignalSource() { return 1; }

int32_t ButtonTile::getSignal(LevelSource* level, int32_t x, int32_t y, int32_t z, int32_t side) {
    return (level->getData(x, y, z) & 8) ? 15 : 0;
}

int32_t ButtonTile::getDirectSignal(Level* level, int32_t x, int32_t y, int32_t z, int32_t side) {
    int32_t meta = level->getData(x, y, z);
    if (!(meta & 8)) return 0;
    int32_t dir = meta & 7;
    if (dir == 1 && side == 5) return 15;
    if (dir == 2 && side == 4) return 15;
    if (dir == 3 && side == 3) return 15;
    if (dir == 4 && side == 2) return 15;
    if ((dir == 5 || dir == 6) && side == 1) return 15;
    if ((dir == 0 || dir == 7) && side == 0) return 15;
    return 0;
}

TextureUVCoordinateSet* ButtonTile::getTexture(int32_t side, int32_t data) {
    this->textureUV = Tile::getTextureUVCoordinateSet(this->textureName, this->textureAux);
    return &this->textureUV;
}

TextureUVCoordinateSet* ButtonTile::getTexture(int32_t side) {
    return this->getTexture(side, 0);
}

TextureUVCoordinateSet* ButtonTile::getTexture(LevelSource* level, int32_t x, int32_t y, int32_t z, int32_t side) {
    return this->getTexture(side, level ? level->getData(x, y, z) : 0);
}

TextureUVCoordinateSet* ButtonTile::getCarriedTexture(int32_t side, int32_t data) {
    return this->getTexture(side, data);
}
