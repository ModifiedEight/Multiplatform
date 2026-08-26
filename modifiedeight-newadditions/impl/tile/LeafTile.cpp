#include <entity/Player.hpp>
#include <inventory/Inventory.hpp>
#include <item/Item.hpp>
#include <level/Level.hpp>
#include <level/biome/Biome.hpp>
#include <rendering/TextureAtlasTextureItem.hpp>
#include <tile/BlockColorRegistry.hpp>
#include <tile/LeafTile.hpp>
#include <tile/material/Material.hpp>
#include <util/Random.hpp>

LeafTile::LeafTile(int32_t id, const std::string &name)
    : TransparentTile(id, name, Material::leaves) {
  this->treeBlocksNearby = 0;
  TextureAtlasTextureItem *texIt = this->getTextureItem("leaves_opaque");
  TextureAtlasTextureItem *texIt2 = this->getTextureItem("leaves");
  for (int32_t i = 0; i < 4; ++i) {
    this->field_8C[i] = *texIt->getUV(i);
    this->field_14C[i] = *texIt2->getUV(i);
    this->field_EC[i] = *texIt2->getUV(i + 4);
  }

  this->field_5C = 3;
  this->setTicking(true);
}
LeafTile::~LeafTile() {
  if (this->treeBlocksNearby)
    delete[] this->treeBlocksNearby;
}
TextureUVCoordinateSet *LeafTile::getTexture(int32_t a2, int32_t meta) {
  if (this->field_88)
    return &this->field_14C[meta & 3];
  return &this->field_8C[meta & 3];
}
TextureUVCoordinateSet *LeafTile::getCarriedTexture(int32_t a2, int32_t a3) {
  return &this->field_EC[a3 & 3];
}
bool_t LeafTile::isSolidRender() { return 0; }
void LeafTile::tick(Level *level, int32_t x, int32_t y, int32_t z,
                    Random *random) {
  if (level->isClientMaybe)
    return;
  int32_t meta = level->getData(x, y, z);
  if ((meta & 8) != 0)
    return;
  if ((meta & 4) == 0)
    return;

  if (!level->hasChunksAt(x - 5, y - 5, z - 5, x + 5, y + 5, z + 5))
    return;

  const int R = 4;
  const int D = 2 * R + 1;
  const int SZ = D * D * D;
  int dist[SZ];
  for (int i = 0; i < SZ; ++i)
    dist[i] = -1;

  for (int dx = -R; dx <= R; ++dx) {
    for (int dy = -R; dy <= R; ++dy) {
      for (int dz = -R; dz <= R; ++dz) {
        int t = level->getTile(x + dx, y + dy, z + dz);
        int idx = (dx + R) * (D * D) + (dy + R) * D + (dz + R);
        if (t == Tile::treeTrunk->blockID) {
          dist[idx] = 0;
        } else if (t == Tile::leaves->blockID) {
          dist[idx] = -2;
        }
      }
    }
  }

  for (int pass = 1; pass <= R; ++pass) {
    for (int dx = -R; dx <= R; ++dx) {
      for (int dy = -R; dy <= R; ++dy) {
        for (int dz = -R; dz <= R; ++dz) {
          int idx = (dx + R) * (D * D) + (dy + R) * D + (dz + R);
          if (dist[idx] == pass - 1) {
            if (dx > -R && dist[idx - (D * D)] == -2)
              dist[idx - (D * D)] = pass;
            if (dx < R && dist[idx + (D * D)] == -2)
              dist[idx + (D * D)] = pass;
            if (dy > -R && dist[idx - D] == -2)
              dist[idx - D] = pass;
            if (dy < R && dist[idx + D] == -2)
              dist[idx + D] = pass;
            if (dz > -R && dist[idx - 1] == -2)
              dist[idx - 1] = pass;
            if (dz < R && dist[idx + 1] == -2)
              dist[idx + 1] = pass;
          }
        }
      }
    }
  }

  int centerIdx = R * (D * D) + R * D + R;
  if (dist[centerIdx] >= 0) {
    level->setDataNoUpdate(x, y, z, meta & ~4);
  } else {
    this->spawnResources(level, x, y, z, meta & 3, 0.0f);
    level->setTile(x, y, z, 0, 3);
  }
}
void LeafTile::neighborChanged(Level *level, int32_t x, int32_t y, int32_t z,
                               int32_t fromTileId) {
  if (!level->isClientMaybe) {
    int32_t meta = level->getData(x, y, z);
    if ((meta & 8) == 0 && (meta & 4) == 0) {
      level->setDataNoUpdate(x, y, z, meta | 4);
    }
  }
}
void LeafTile::onRemove(Level *level, int32_t x, int32_t y, int32_t z) {
  int32_t v8;   // r11
  int32_t meta; // r0
  int32_t zz;   // r7
  int32_t yy;   // r6
  int32_t xx;   // r5
  int32_t v13;  // [sp+10h] [bp-30h]

  if (!level->isClientMaybe &&
      level->hasChunksAt(x - 2, y - 2, z - 2, x + 2, y + 2, z + 2)) {
    for (xx = -1; xx != 2; ++xx) {
      yy = -1;
      v13 = xx + x;
      do {
        for (zz = -1; zz != 2; ++zz) {
          v8 = zz + z;
          if (level->getTile(v13, yy + y, zz + z) == Tile::leaves->blockID) {
            meta = level->getData(v13, yy + y, v8);
            level->setDataNoUpdate(v13, yy + y, v8, meta | 4);
          }
        }
        ++yy;
      } while (yy != 2);
    }
  }
}
void LeafTile::onGraphicsModeChanged(bool_t a2) {
  this->field_7C = a2;
  this->field_88 = a2;
  this->goodGraphics = a2;
}
int32_t LeafTile::getResource(int32_t, Random *) {
  return Tile::sapling->blockID;
}
int32_t LeafTile::getResourceCount(Random *a2) {
  return a2->genrand_int32() % 20 == 0;
}
void LeafTile::spawnResources(Level *level, int32_t x, int32_t y, int32_t z,
                              int32_t meta, float a7) {
  Random *p_random;
  int32_t v12;
  int32_t v13;

  if (!level->isClientMaybe) {
    p_random = &level->random;
    int32_t chance = 20;
    if ((meta & 3) == 3) {
      chance = 40;
    }
    if (!(level->random.genrand_int32() % chance)) {
      v13 = this->getResource(meta, p_random);
      v12 = this->getSpawnResourcesAuxValue(meta);
      this->popResource(level, x, y, z, ItemInstance(v13, 1, v12));
    }
    if ((meta & 3) == 0 && !(p_random->genrand_int32() % 200)) {
      this->popResource(level, x, y, z, ItemInstance(Item::apple, 1, 0));
    }
  }
}
int32_t LeafTile::getRenderLayer() { return 3; }
int32_t LeafTile::getColor(int32_t aux) {
  int32_t v5 = aux & 3;
  if (v5 == 1) {
    return 0x619961;
  }
  if (v5 == 2) {
    return 0x80A755;
  }
  return 0x48B518;
}
int32_t LeafTile::getColor(LevelSource *level, int32_t x, int32_t y,
                           int32_t z) {
  int32_t v5 = (level ? level->getData(x, y, z) : 0) & 3;
  if (v5 == 1) {
    return 0x619961;
  }
  if (v5 == 2) {
    return 0x80A755;
  }
  if (level) {
    int totalR = 0, totalG = 0, totalB = 0;
    for (int sx = -1; sx <= 1; ++sx) {
      for (int sz = -1; sz <= 1; ++sz) {
        Biome *b = level->getBiome(x + sx, z + sz);
        int c = 0x48B518;
        if (b == Biome::swampland)
          c = 0x6A7039;
        else if (b == Biome::jungle || b == Biome::rainForest)
          c = 0x30BB0B;
        else if (b == Biome::plains)
          c = 0x77AB2F;
        else if (b == Biome::forest)
          c = 0x59AE30;
        else if (b == Biome::birchForest)
          c = 0x6BAE41;
        else if (b == Biome::seasonalForest)
          c = 0x509C2C;
        else if (b == Biome::savanna || b == Biome::desert ||
                 b == Biome::iceDesert)
          c = 0xAEA42A;
        else if (b == Biome::taiga)
          c = 0x68B55F;
        else if (b == Biome::tundra || b == Biome::icePeaks)
          c = 0x60A17B;
        else if (b == Biome::mountain)
          c = 0x55A834;
        totalR += (c >> 16) & 0xFF;
        totalG += (c >> 8) & 0xFF;
        totalB += c & 0xFF;
      }
    }
    return ((totalR / 9) << 16) | ((totalG / 9) << 8) | (totalB / 9);
  }
  return (v5 == 3) ? 0x30BB0B : 0x48B518;
}
void LeafTile::playerDestroy(Level *level, Player *player, int32_t x, int32_t y,
                             int32_t z, int32_t a7) {
  ItemInstance *sel; // r0

  if (!level->isClientMaybe && (sel = player->inventory->getSelected()) != 0 &&
      Item::shears == sel->itemClass && Item::shears) {
    this->popResource(level, x, y, z,
                      ItemInstance(Tile::leaves->blockID, 1, a7 & 3));
  } else {
    Tile::playerDestroy(level, player, x, y, z, a7);
  }
}
int32_t LeafTile::getSpawnResourcesAuxValue(int32_t a2) { return a2 & 3; }
