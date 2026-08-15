
#include <cmath>
#include <entity/Minecart.hpp>
#include <entity/Player.hpp>
#include <level/Level.hpp>
#include <tile/LeverTile.hpp>
#include <tile/material/Material.hpp>
#include <vector>

LeverTile::LeverTile(int32_t id) : Tile(id, "lever", Material::decoration) {
  this->setShape(0.25f, 0.0f, 0.25f, 0.75f, 0.6f, 0.75f);
}

LeverTile::~LeverTile() {}

bool_t LeverTile::isCubeShaped() { return 0; }

bool_t LeverTile::isSolidRender() { return 0; }

AABB *LeverTile::getAABB(Level *level, int32_t x, int32_t y, int32_t z) {
  return 0;
}

void LeverTile::updateDefaultShape() {
  this->setShape(0.20f, 0.0f, 0.20f, 0.80f, 0.65f, 0.80f);
}

void LeverTile::updateShape(LevelSource *level, int32_t x, int32_t y,
                            int32_t z) {
  int32_t dir = level->getData(x, y, z) & 7;
  if (dir == 1) {
    this->setShape(0.0f, 0.15f, 0.20f, 0.65f, 0.85f, 0.80f);
  } else if (dir == 2) {
    this->setShape(0.35f, 0.15f, 0.20f, 1.0f, 0.85f, 0.80f);
  } else if (dir == 3) {
    this->setShape(0.20f, 0.15f, 0.0f, 0.80f, 0.85f, 0.65f);
  } else if (dir == 4) {
    this->setShape(0.20f, 0.15f, 0.35f, 0.80f, 0.85f, 1.0f);
  } else if (dir == 0 || dir == 7) {
    this->setShape(0.20f, 0.35f, 0.20f, 0.80f, 1.0f, 0.80f);
  } else {
    this->setShape(0.20f, 0.0f, 0.20f, 0.80f, 0.65f, 0.80f);
  }
}

int32_t LeverTile::getRenderShape() { return 25; }

bool_t LeverTile::mayPlace(Level *level, int32_t x, int32_t y, int32_t z) {
  if (level->isSolidBlockingTile(x - 1, y, z))
    return 1;
  if (level->isSolidBlockingTile(x + 1, y, z))
    return 1;
  if (level->isSolidBlockingTile(x, y, z - 1))
    return 1;
  if (level->isSolidBlockingTile(x, y, z + 1))
    return 1;
  if (level->isSolidBlockingTile(x, y - 1, z))
    return 1;
  if (level->isSolidBlockingTile(x, y + 1, z))
    return 1;
  return 0;
}

bool_t LeverTile::mayPlace(Level *level, int32_t x, int32_t y, int32_t z,
                           int32_t side) {
  if (side == 0 && level->isSolidBlockingTile(x, y + 1, z))
    return 1;
  if (side == 1 && level->isSolidBlockingTile(x, y - 1, z))
    return 1;
  if (side == 2 && level->isSolidBlockingTile(x, y, z + 1))
    return 1;
  if (side == 3 && level->isSolidBlockingTile(x, y, z - 1))
    return 1;
  if (side == 4 && level->isSolidBlockingTile(x + 1, y, z))
    return 1;
  if (side == 5 && level->isSolidBlockingTile(x - 1, y, z))
    return 1;
  return 0;
}

int32_t LeverTile::getPlacementDataValue(Level *level, int32_t x, int32_t y,
                                         int32_t z, int32_t side, float fx,
                                         float fy, float fz, Mob *mob,
                                         int32_t meta) {
  int32_t face = side & 7;
  int32_t playerFacing =
      mob ? ((int32_t)floorf((mob->yaw * 4.0f / 360.0f) + 0.5f) & 3) : 0;
  if (face == 0 && level->isSolidBlockingTile(x, y + 1, z)) {
    return (playerFacing == 0 || playerFacing == 2) ? 7 : 0;
  }
  if (face == 1 && level->isSolidBlockingTile(x, y - 1, z)) {
    return (playerFacing == 0 || playerFacing == 2) ? 5 : 6;
  }
  if (face == 2 && level->isSolidBlockingTile(x, y, z + 1))
    return 4;
  if (face == 3 && level->isSolidBlockingTile(x, y, z - 1))
    return 3;
  if (face == 4 && level->isSolidBlockingTile(x + 1, y, z))
    return 2;
  if (face == 5 && level->isSolidBlockingTile(x - 1, y, z))
    return 1;
  return (playerFacing == 0 || playerFacing == 2) ? 5 : 6;
}

void LeverTile::neighborChanged(Level *level, int32_t x, int32_t y, int32_t z,
                                int32_t ox, int32_t oy, int32_t oz,
                                int32_t tileId) {
  int32_t meta = level->getData(x, y, z);
  int32_t dir = meta & 7;
  bool_t drop = 0;
  if (dir == 1 && !level->isSolidBlockingTile(x - 1, y, z))
    drop = 1;
  if (dir == 2 && !level->isSolidBlockingTile(x + 1, y, z))
    drop = 1;
  if (dir == 3 && !level->isSolidBlockingTile(x, y, z - 1))
    drop = 1;
  if (dir == 4 && !level->isSolidBlockingTile(x, y, z + 1))
    drop = 1;
  if ((dir == 5 || dir == 6) && !level->isSolidBlockingTile(x, y - 1, z))
    drop = 1;
  if ((dir == 0 || dir == 7) && !level->isSolidBlockingTile(x, y + 1, z))
    drop = 1;

  if (drop) {
    this->spawnResources(level, x, y, z, meta, 1.0f);
    level->setTile(x, y, z, 0, 3);
  }
}

bool_t LeverTile::use(Level *level, int32_t x, int32_t y, int32_t z,
                      Player *player) {
  int32_t meta = level->getData(x, y, z);
  int32_t dir = meta & 7;
  int32_t active = meta & 8;
  int32_t newMeta = dir | (active ^ 8);

  level->setData(x, y, z, newMeta, 3);
  level->playSound((float)x + 0.5f, (float)y + 0.5f, (float)z + 0.5f,
                   "random.click", 0.3f, (newMeta & 8) ? 0.6f : 0.5f);
  level->sendTileUpdated(x, y, z);

  level->updateNeighborsAt(x, y, z, this->blockID);
  if (dir == 1)
    level->updateNeighborsAt(x - 1, y, z, this->blockID);
  else if (dir == 2)
    level->updateNeighborsAt(x + 1, y, z, this->blockID);
  else if (dir == 3)
    level->updateNeighborsAt(x, y, z - 1, this->blockID);
  else if (dir == 4)
    level->updateNeighborsAt(x, y, z + 1, this->blockID);
  else if (dir == 5 || dir == 6)
    level->updateNeighborsAt(x, y - 1, z, this->blockID);
  else if (dir == 0 || dir == 7)
    level->updateNeighborsAt(x, y + 1, z, this->blockID);

  if (newMeta & 8) {
    AABB box = {(float)x - 2.5f, (float)y - 2.0f, (float)z - 2.5f,
                (float)x + 3.5f, (float)y + 3.0f, (float)z + 3.5f};
    std::vector<Entity *> *ents = level->getEntities(nullptr, box);
    if (ents) {
      for (Entity *e : *ents) {
        if (dynamic_cast<Minecart *>(e) || (e->getEntityTypeId() == 84)) {
          float dx = e->posX - ((float)x + 0.5f);
          float dz = e->posZ - ((float)z + 0.5f);
          float dist = sqrtf(dx * dx + dz * dz);
          if (dist > 0.001f) {
            dx /= dist;
            dz /= dist;
          } else {
            dx = 1.0f;
            dz = 0.0f;
          }
          float speed = 0.45f;
          e->motionX += dx * speed;
          e->motionZ += dz * speed;
        }
      }
    }
  }

  return 1;
}

void LeverTile::onRemove(Level *level, int32_t x, int32_t y, int32_t z) {
  int32_t meta = level->getData(x, y, z);
  if ((meta & 8) != 0) {
    level->updateNeighborsAt(x, y, z, this->blockID);
    int32_t dir = meta & 7;
    if (dir == 1)
      level->updateNeighborsAt(x - 1, y, z, this->blockID);
    else if (dir == 2)
      level->updateNeighborsAt(x + 1, y, z, this->blockID);
    else if (dir == 3)
      level->updateNeighborsAt(x, y, z - 1, this->blockID);
    else if (dir == 4)
      level->updateNeighborsAt(x, y, z + 1, this->blockID);
    else if (dir == 5)
      level->updateNeighborsAt(x, y - 1, z, this->blockID);
    else if (dir == 0)
      level->updateNeighborsAt(x, y + 1, z, this->blockID);
  }
  Tile::onRemove(level, x, y, z);
}

bool_t LeverTile::isSignalSource() { return 1; }

int32_t LeverTile::getSignal(LevelSource *level, int32_t x, int32_t y,
                             int32_t z, int32_t side) {
  return (level->getData(x, y, z) & 8) ? 15 : 0;
}

int32_t LeverTile::getDirectSignal(Level *level, int32_t x, int32_t y,
                                   int32_t z, int32_t side) {
  int32_t meta = level->getData(x, y, z);
  if ((meta & 8) == 0)
    return 0;
  int32_t dir = meta & 7;
  if (dir == 0 && side == 0)
    return 15;
  if (dir == 5 && side == 1)
    return 15;
  if (dir == 4 && side == 2)
    return 15;
  if (dir == 3 && side == 3)
    return 15;
  if (dir == 2 && side == 4)
    return 15;
  if (dir == 1 && side == 5)
    return 15;
  return 0;
}
