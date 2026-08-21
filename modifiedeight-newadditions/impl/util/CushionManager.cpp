#include <cstdio>
#include <entity/ItemEntity.hpp>
#include <entity/Player.hpp>
#include <entity/SeatEntity.hpp>
#include <inventory/Inventory.hpp>
#include <item/Item.hpp>
#include <item/ItemInstance.hpp>
#include <level/Level.hpp>
#include <level/LevelSource.hpp>
#include <mutex>
#include <rendering/TileRenderer.hpp>
#include <tile/Tile.hpp>
#include <unordered_map>
#include <util/CushionManager.hpp>

static std::unordered_map<uint64_t, uint8_t> s_cushions;
static std::mutex s_cushionMutex;

static inline uint64_t getPosKey(int32_t x, int32_t y, int32_t z) {
  return ((uint64_t)(x & 0x3FFFFFF) << 38) | ((uint64_t)(y & 0xFFF) << 26) |
         (uint64_t)(z & 0x3FFFFFF);
}

static inline void unpackPosKey(uint64_t key, int32_t &x, int32_t &y,
                                int32_t &z) {
  int64_t rawX = (int64_t)(key >> 38) & 0x3FFFFFF;
  if (rawX & 0x2000000)
    rawX |= ~0x3FFFFFFLL;
  x = (int32_t)rawX;

  int64_t rawY = (int64_t)(key >> 26) & 0xFFF;
  if (rawY & 0x800)
    rawY |= ~0xFFFLL;
  y = (int32_t)rawY;

  int64_t rawZ = (int64_t)key & 0x3FFFFFF;
  if (rawZ & 0x2000000)
    rawZ |= ~0x3FFFFFFLL;
  z = (int32_t)rawZ;
}

bool CushionManager::isStairTile(Tile *tile) {
  if (!tile)
    return false;
  return tile->getRenderShape() == 10;
}

bool CushionManager::isSlabTile(Tile *tile) {
  if (!tile)
    return false;
  int id = tile->blockID;
  return id == 44 || id == 126 || id == 182 || id == 184 || id == 186 ||
         id == 188 || (Tile::stoneSlabHalf && tile == Tile::stoneSlabHalf) ||
         (Tile::woodSlabHalf && tile == Tile::woodSlabHalf) ||
         (Tile::coloredSlabHalf1 && tile == Tile::coloredSlabHalf1) ||
         (Tile::coloredSlabHalf2 && tile == Tile::coloredSlabHalf2) ||
         (Tile::coloredBrickSlabHalf1 && tile == Tile::coloredBrickSlabHalf1) ||
         (Tile::coloredBrickSlabHalf2 && tile == Tile::coloredBrickSlabHalf2);
}

bool CushionManager::isCushionableTile(Tile *tile, int meta) {
  if (isStairTile(tile)) {
    return (meta & 4) == 0;
  }
  if (isSlabTile(tile)) {
    return (meta & 8) == 0;
  }
  return false;
}

bool CushionManager::hasCushion(LevelSource *level, int x, int y, int z) {
  if (!level)
    return false;
  std::lock_guard<std::mutex> lock(s_cushionMutex);
  uint64_t key = getPosKey(x, y, z);
  return s_cushions.find(key) != s_cushions.end();
}

int CushionManager::getCushionColor(LevelSource *level, int x, int y, int z) {
  if (!level)
    return 0;
  std::lock_guard<std::mutex> lock(s_cushionMutex);
  uint64_t key = getPosKey(x, y, z);
  auto it = s_cushions.find(key);
  if (it != s_cushions.end()) {
    return it->second;
  }
  return 0;
}

bool CushionManager::setCushion(Level *level, int x, int y, int z, int color) {
  if (!level)
    return false;
  {
    std::lock_guard<std::mutex> lock(s_cushionMutex);
    uint64_t key = getPosKey(x, y, z);
    s_cushions[key] = (uint8_t)(color & 15);
  }
  level->sendTileUpdated(x, y, z);
  return true;
}

bool CushionManager::removeCushion(Level *level, int x, int y, int z) {
  if (!level)
    return false;
  bool removed = false;
  {
    std::lock_guard<std::mutex> lock(s_cushionMutex);
    uint64_t key = getPosKey(x, y, z);
    auto it = s_cushions.find(key);
    if (it != s_cushions.end()) {
      s_cushions.erase(it);
      removed = true;
    }
  }
  if (removed) {
    level->sendTileUpdated(x, y, z);
  }
  return removed;
}

void CushionManager::clear() {
  std::lock_guard<std::mutex> lock(s_cushionMutex);
  s_cushions.clear();
}

bool CushionManager::handleUse(Player *player, Level *level, int x, int y,
                               int z, int face, float faceX, float faceY,
                               float faceZ, ItemInstance *sel) {
  if (!player || !level)
    return false;
  int blockId = level->getTile(x, y, z);
  if (blockId <= 0 || blockId >= 256)
    return false;
  Tile *tile = Tile::tiles[blockId];
  if (!tile)
    return false;
  int meta = level->getData(x, y, z);

  bool isShears =
      (sel && ((sel->itemClass && sel->itemClass == (Item *)Item::shears) ||
               (Item::shears && sel->getId() == Item::shears->itemID) ||
               sel->getId() == 359));
  bool isWool =
      (sel && ((sel->tileClass && sel->tileClass == Tile::cloth) ||
               (Tile::cloth && sel->getId() == Tile::cloth->blockID) ||
               sel->getId() == 35));

  if (isShears) {
    if (hasCushion(level, x, y, z)) {
      int color = getCushionColor(level, x, y, z);
      removeCushion(level, x, y, z);
      if (!level->isClientMaybe) {
        ItemEntity *dropped = new ItemEntity(
            level, (float)x + 0.5f, (float)y + 0.6f, (float)z + 0.5f,
            ItemInstance(Tile::cloth, 1, color));
        level->addEntity(dropped);
      }
      level->playSound((float)x + 0.5f, (float)y + 0.5f, (float)z + 0.5f,
                       "step.cloth", 1.0f, 1.0f);
      return true;
    }
  }

  if (isWool) {
    if (isCushionableTile(tile, meta)) {
      bool isCenter = (faceX >= 0.20f && faceX <= 0.80f && faceZ >= 0.20f &&
                       faceZ <= 0.80f);
      if (isCenter && !hasCushion(level, x, y, z)) {
        int color = sel->getAuxValue();
        setCushion(level, x, y, z, color);
        level->playSound((float)x + 0.5f, (float)y + 0.5f, (float)z + 0.5f,
                         "step.cloth", 1.0f, 0.8f);
        if (player->inventory && !player->abilities.instabuild) {
          if (sel->count > 0) {
            sel->count--;
            if (sel->count <= 0) {
              sel->setNull();
            }
          }
        }
        return true;
      }
    }
  }

  if (hasCushion(level, x, y, z)) {
    if (player->isSneaking())
      return false;
    if (player->ridingAt) {
      player->ride(0);
      return true;
    }
    float seatX = (float)x + 0.5f;
    float seatY = (float)y + 0.5f;
    float seatZ = (float)z + 0.5f;
    float seatYaw = player->yaw;

    if (isStairTile(tile)) {
      int dir = meta & 3;
      if (dir == 0) {
        seatX = (float)x + 0.28f;
        seatYaw = -90.0f;
      } else if (dir == 1) {
        seatX = (float)x + 0.72f;
        seatYaw = 90.0f;
      } else if (dir == 2) {
        seatZ = (float)z + 0.28f;
        seatYaw = 180.0f;
      } else if (dir == 3) {
        seatZ = (float)z + 0.72f;
        seatYaw = 0.0f;
      }
    }

    if (!level->isClientMaybe) {
      SeatEntity *seat = new SeatEntity(level, seatX, seatY, seatZ, x, y, z);
      seat->yaw = seatYaw;
      seat->prevYaw = seatYaw;
      level->addEntity(seat);
      player->ride(seat);
      player->yaw = seatYaw;
      player->prevYaw = seatYaw;
    }
    return true;
  }

  return false;
}

void CushionManager::renderCushion(TileRenderer *renderer, Tile *tile, int x,
                                   int y, int z) {
  if (!renderer || !renderer->levelSource || !Tile::cloth)
    return;
  if (!hasCushion(renderer->levelSource, x, y, z))
    return;

  int color = getCushionColor(renderer->levelSource, x, y, z);
  int meta = renderer->levelSource->getData(x, y, z);

  float minX = 0.0625f, maxX = 0.9375f;
  float minY = 0.5f, maxY = 0.5625f;
  float minZ = 0.0625f, maxZ = 0.9375f;

  if (isStairTile(tile)) {
    int dir = meta & 3;
    if (dir == 0) {
      minX = 0.0625f;
      maxX = 0.5f;
    } else if (dir == 1) {
      minX = 0.5f;
      maxX = 0.9375f;
    } else if (dir == 2) {
      minZ = 0.0625f;
      maxZ = 0.5f;
    } else if (dir == 3) {
      minZ = 0.5f;
      maxZ = 0.9375f;
    }
  }

  Tile::cloth->setShape(minX, minY, minZ, maxX, maxY, maxZ);
  TextureUVCoordinateSet *tex = Tile::cloth->getTexture(0, color);
  if (tex) {
    renderer->hasUVCoords = 1;
    renderer->field_8 = *tex;
    renderer->tesselateBlockInWorld(Tile::cloth, x, y, z);
    renderer->hasUVCoords = 0;
  }
  Tile::cloth->setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
}

void CushionManager::onTileDestroyed(Level *level, int x, int y, int z) {
  if (!level)
    return;
  if (hasCushion(level, x, y, z)) {
    int color = getCushionColor(level, x, y, z);
    removeCushion(level, x, y, z);
    if (!level->isClientMaybe) {
      ItemEntity *dropped =
          new ItemEntity(level, (float)x + 0.5f, (float)y + 0.5f,
                         (float)z + 0.5f, ItemInstance(Tile::cloth, 1, color));
      level->addEntity(dropped);
    }
  }
}

void CushionManager::save(const std::string &dir) {
  if (dir.empty())
    return;
  std::string filePath = dir + "/cushions.dat";
  FILE *f = fopen(filePath.c_str(), "wb");
  if (!f)
    return;

  std::lock_guard<std::mutex> lock(s_cushionMutex);
  uint32_t count = (uint32_t)s_cushions.size();
  fwrite(&count, sizeof(uint32_t), 1, f);

  for (auto &pair : s_cushions) {
    int32_t x, y, z;
    unpackPosKey(pair.first, x, y, z);
    uint8_t color = pair.second;
    fwrite(&x, sizeof(int32_t), 1, f);
    fwrite(&y, sizeof(int32_t), 1, f);
    fwrite(&z, sizeof(int32_t), 1, f);
    fwrite(&color, sizeof(uint8_t), 1, f);
  }
  fclose(f);
}

void CushionManager::load(const std::string &dir) {
  clear();
  if (dir.empty())
    return;
  std::string filePath = dir + "/cushions.dat";
  FILE *f = fopen(filePath.c_str(), "rb");
  if (!f)
    return;

  uint32_t count = 0;
  if (fread(&count, sizeof(uint32_t), 1, f) == 1) {
    std::lock_guard<std::mutex> lock(s_cushionMutex);
    for (uint32_t i = 0; i < count; ++i) {
      int32_t x = 0, y = 0, z = 0;
      uint8_t color = 0;
      if (fread(&x, sizeof(int32_t), 1, f) == 1 &&
          fread(&y, sizeof(int32_t), 1, f) == 1 &&
          fread(&z, sizeof(int32_t), 1, f) == 1 &&
          fread(&color, sizeof(uint8_t), 1, f) == 1) {
        uint64_t key = getPosKey(x, y, z);
        s_cushions[key] = color;
      }
    }
  }
  fclose(f);
}
