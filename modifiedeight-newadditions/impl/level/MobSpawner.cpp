#include <cmath>
#include <entity/AgableMob.hpp>
#include <entity/Mob.hpp>
#include <entity/MobCategory.hpp>
#include <entity/MobFactory.hpp>
#include <entity/AbstractFish.hpp>
#include <entity/Player.hpp>
#include <level/Level.hpp>
#include <level/MobSpawner.hpp>
#include <perf/Stopwatch.hpp>
#include <tile/Tile.hpp>
#include <tile/material/Material.hpp>
#include <vector>

std::map<ChunkPos, bool> MobSpawner::chunksToPoll;

bool MobSpawner::addMob(Level *level, Mob *entity, float x, float y, float z,
                        float yaw, float pitch, bool_t force) {
  entity->moveTo(x, y, z, yaw, pitch);
  if (force || entity->canSpawn()) {
    level->addEntity(entity);
    MobSpawner::finalizeMobSettings(entity, level, x, y, z);
    return 1;
  }
  return 0;
}
void MobSpawner::finalizeMobSettings(Mob *a1, Level *a2, float a3, float a4,
                                     float a5) {
  a1->finalizeMobSpawn();
  MobSpawner::makeBabyMob(a1, 0.5);
}
Pos MobSpawner::getRandomPosWithin(Level *a2, int32_t startChunkX,
                                   int32_t startChunkZ) {
  return {startChunkX + (int32_t)(a2->random.genrand_int32() & 0xf),
          (int32_t)(a2->random.genrand_int32() & 0x7f),
          startChunkZ + (int32_t)(a2->random.genrand_int32() & 0xf)};
}
bool_t MobSpawner::isSpawnPositionOk(const MobCategory &a1, Level *level,
                                     int32_t x, int32_t y, int32_t z) {
  if (a1.material == Material::water) {
    if (!level->getMaterial(x, y, z)->isLiquid()) {
      return 0;
    }
  } else {
    if (!level->isSolidBlockingTile(x, y - 1, z)) {
      return 0;
    }
    if (level->isSolidBlockingTile(x, y, z)) {
      return 0;
    }
    if (level->getMaterial(x, y, z)->isLiquid()) {
      return 0;
    }
  }
  return 1;
}
void MobSpawner::makeBabyMob(Mob *a1, float chance) {
  static Random randomInstance2(0x5DEEA8F);
  if (a1->getCreatureBaseType() == 2) {
    if (randomInstance2.nextFloat() < chance) {
      ((AgableMob *)a1)->setAge(-24000);
    }
  }
}
void MobSpawner::postProcessSpawnMobs(Level *a2, Biome *a3, int32_t a4,
                                      int32_t a5, int32_t a6, int32_t a7,
                                      Random *a8) {
  std::vector<Biome::MobSpawnerData> v61 =
      *a3->getMobs(MobCategory::creature); // TODO check
  if (!v61.empty()) {
    while (1) {
      float v26 = a8->nextFloat();
      if (v26 >= a3->getCreatureProbability()) {
        break;
      }

      int totalrarity = 0;
      for (auto &&msd : v61) {
        totalrarity += msd.rarity;
      }
      if (totalrarity <= 0)
        break;

      int v30 = a2->random.genrand_int32() % totalrarity;
      Biome::MobSpawnerData *data = 0;
      for (auto &&i : v61) {
        v30 -= i.rarity;
        if (v30 < 0) {
          data = &i;
          goto LABEL_15;
        }
      }
    LABEL_15:
      if (!data)
        break;
      int min = data->min;
      int v33 = data->max + 1 - min;
      if (v33 <= 0) v33 = 1;
      int v60 = min + a8->genrand_int32() % v33;
      int v58 = a4 + a8->genrand_int32() % a6;
      int v34 = 0;
      int v35 = v58;
      int v59 = a8->genrand_int32() % a7 + a5;
      int v50 = v59;
    LABEL_34:
      int v55 = v34;
      if (v34 < v60) {
        int v56 = 4;
        while (1) {
          int top = a2->getTopSolidBlock(v35, v50);
          bool v43;
          if (MobSpawner::isSpawnPositionOk(MobCategory::creature, a2, v35, top,
                                            v50)) {
            Mob *mob = MobFactory::CreateMob(data->mobtype, a2);
            if (!mob) {
              if (!--v56) {
                v34 = v55 + 1;
                goto LABEL_34;
              }
              continue;
              // goto LABEL_24;
            }
            mob->moveTo((float)v35 + 0.5, (float)top, (float)v50 + 0.5,
                        a8->nextFloat() * 360.0, 0.0);
            a2->addEntity(mob);
            v43 = 1;
            MobSpawner::finalizeMobSettings(mob, a2, (float)v35 + 0.5,
                                            (float)top, (float)v50 + 0.5);
          } else {
            v43 = 0;
          }

          v35 += a8->genrand_int32() % 5u - a8->genrand_int32() % 5;
          v50 += a8->genrand_int32() % 5u - a8->genrand_int32() % 5;
          int v44 = a4 + a6;
          int v45 = a5 + a6;
          while (v35 < a4 || v35 >= v44 || v50 < a5 || v50 >= v45) {
            int v54 = v45;
            int v53 = v44;
            int v46 = v58 + a8->genrand_int32() % 5;
            v35 = v46 - a8->genrand_int32() % 5;
            int v47 = v59 + a8->genrand_int32() % 5;
            int v48 = a8->genrand_int32() % 5;
            v45 = v54;
            v44 = v53;
            v50 = v47 - v48;
          }
          if (v43) {
          LABEL_33:
            v34 = v55 + 1;
            goto LABEL_34;
          }
          if (!--v56) {
            goto LABEL_33;
          }
        }
      }
    }
  }
}

bool_t MobSpawner::tick(Level *level, bool_t hostiles, bool_t animals) {
  if (!hostiles && !animals)
    return 0;
  if (!level || level->isClientMaybe)
    return 0;
  if (level->playersMaybe.empty())
    return 0;

  MobSpawner::chunksToPoll.clear();

  for (auto *player : level->playersMaybe) {
    if (!player)
      continue;
    int px = (int)floorf(player->posX * 0.0625f);
    int pz = (int)floorf(player->posZ * 0.0625f);
    for (int dx = -4; dx <= 4; ++dx) {
      for (int dz = -4; dz <= 4; ++dz) {
        int cx = px + dx;
        int cz = pz + dz;
        if (level->getLevelData()->getGeneratorVersion() == 0) {
          if (cx < 0 || cx > 15 || cz < 0 || cz > 15)
            continue;
        }
        MobSpawner::chunksToPoll.insert({{cx, cz}, 0});
      }
    }
  }

  if (MobSpawner::chunksToPoll.empty())
    return 0;

  int totalSpawned = 0;
  std::vector<ChunkPos> chunkList;
  chunkList.reserve(MobSpawner::chunksToPoll.size());
  for (const auto &kv : MobSpawner::chunksToPoll) {
    chunkList.push_back(kv.first);
  }
  for (size_t i = chunkList.size() - 1; i > 0; --i) {
    size_t j = (size_t)(level->random.genrand_int32() % (i + 1));
    std::swap(chunkList[i], chunkList[j]);
  }

  for (int catIdx = 0; catIdx < MobCategory::numValues; ++catIdx) {
    const MobCategory *cat = MobCategory::values[catIdx];
    if (!cat)
      continue;

    bool isMonster = (cat->id == 1);
    bool isCreature = (cat->id == 2);
    bool isWater = (cat->id == 3);

    if (isMonster && !hostiles)
      continue;
    if ((isCreature || isWater) && !animals)
      continue;

    int maxCap = isMonster ? 70 : (isCreature ? 15 : 20);
    int currentCount = level->countInstanceOfBaseType(cat->id);
    if (currentCount >= maxCap)
      continue;

    int chunksToProcess = (int)chunkList.size();
    for (int i = 0; i < chunksToProcess && currentCount < maxCap; ++i) {
      const ChunkPos &cpos = chunkList[i];
      Pos startPos =
          MobSpawner::getRandomPosWithin(level, cpos.x * 16, cpos.z * 16);
      int x = startPos.x;
      int y = startPos.y;
      int z = startPos.z;

      if (y <= 1 || y >= 127)
        continue;

      if (isWater) {
        bool foundWater = false;
        for (int scanY = 56; scanY >= 35; --scanY) {
          if (level->getMaterial(x, scanY, z)->isLiquid() &&
              level->getMaterial(x, scanY + 1, z)->isLiquid() &&
              level->getMaterial(x, scanY + 2, z)->isLiquid()) {
            y = scanY;
            foundWater = true;
            break;
          }
        }
        if (!foundWater) {
          for (int scanY = 60; scanY >= 30; --scanY) {
            if (level->getMaterial(x, scanY, z)->isLiquid()) {
              y = scanY;
              foundWater = true;
              break;
            }
          }
        }
        if (!foundWater)
          continue;
      } else if (isMonster) {
        bool foundCaveFloor = false;
        Player *refPlayer = level->getNearestPlayer((float)x, (float)y, (float)z, -1.0f);
        int targetY = refPlayer ? ((int)refPlayer->posY + (int)(level->random.genrand_int32() % 24) - 12) : y;
        if (targetY < 2) targetY = 2;
        if (targetY > 126) targetY = 126;

        for (int scanY = targetY; scanY > 1; --scanY) {
          if (!level->isSolidBlockingTile(x, scanY, z) &&
              level->isSolidBlockingTile(x, scanY - 1, z) &&
              level->getRawBrightness(x, scanY, z) <= 7) {
            y = scanY;
            foundCaveFloor = true;
            break;
          }
        }
        if (!foundCaveFloor) {
          for (int scanY = 126; scanY > targetY; --scanY) {
            if (!level->isSolidBlockingTile(x, scanY, z) &&
                level->isSolidBlockingTile(x, scanY - 1, z) &&
                level->getRawBrightness(x, scanY, z) <= 7) {
              y = scanY;
              foundCaveFloor = true;
              break;
            }
          }
        }
        if (!foundCaveFloor)
          continue;
      } else {
        if (level->isSolidBlockingTile(x, y, z))
          continue;
      }

      int packSpawned = 0;
      for (int packAttempt = 0; packAttempt < 3 && currentCount < maxCap;
           ++packAttempt) {
        int sx = x;
        int sy = y;
        int sz = z;

        Biome::MobSpawnerData spawnData(-128, 0, 0, 0);
        AbstractFish* schoolLeader = 0;

        for (int clusterAttempt = 0;
             clusterAttempt < 4 && currentCount < maxCap; ++clusterAttempt) {
          if (clusterAttempt > 0) {
            sx += (int)(level->random.genrand_int32() % 6) -
                  (int)(level->random.genrand_int32() % 6);
            sy += (int)(level->random.genrand_int32() % 2) -
                  (int)(level->random.genrand_int32() % 2);
            sz += (int)(level->random.genrand_int32() % 6) -
                  (int)(level->random.genrand_int32() % 6);
          }

          if (sy <= 1 || sy >= 127)
            continue;

          if (isWater) {
            if (!level->getMaterial(sx, sy, sz)->isLiquid()) {
              bool snapped = false;
              for (int dy = 1; dy <= 6; ++dy) {
                if (sy - dy > 1 && level->getMaterial(sx, sy - dy, sz)->isLiquid()) {
                  sy -= dy;
                  snapped = true;
                  break;
                }
                if (sy + dy < 127 && level->getMaterial(sx, sy + dy, sz)->isLiquid()) {
                  sy += dy;
                  snapped = true;
                  break;
                }
              }
              if (!snapped)
                continue;
            }
          } else if (level->isSolidBlockingTile(sx, sy, sz) || !level->isSolidBlockingTile(sx, sy - 1, sz)) {
            bool snapped = false;
            for (int dy = 1; dy <= 6; ++dy) {
              if (sy - dy > 1 && !level->isSolidBlockingTile(sx, sy - dy, sz) &&
                  level->isSolidBlockingTile(sx, sy - dy - 1, sz)) {
                sy -= dy;
                snapped = true;
                break;
              }
              if (sy + dy < 127 && !level->isSolidBlockingTile(sx, sy + dy, sz) &&
                  level->isSolidBlockingTile(sx, sy + dy - 1, sz)) {
                sy += dy;
                snapped = true;
                break;
              }
            }
            if (!snapped)
              continue;
          }

          float fx = (float)sx + 0.5f;
          float fy = (float)sy;
          float fz = (float)sz + 0.5f;

          Player *nearest = level->getNearestPlayer(fx, fy, fz, -1.0f);
          if (!nearest)
            continue;

          float dx = nearest->posX - fx;
          float dy = nearest->posY - fy;
          float dz = nearest->posZ - fz;
          float distSq = dx * dx + dy * dy + dz * dz;

          if (distSq < 144.0f || distSq > 16384.0f)
            continue;

          if (!MobSpawner::isSpawnPositionOk(*cat, level, sx, sy, sz))
            continue;

          if (spawnData.rarity <= 0 || spawnData.mobtype == 0) {
            spawnData = level->getRandomMobSpawnAt(*cat, sx, sy, sz);
            if (spawnData.rarity <= 0 || spawnData.mobtype == 0) {
              if (isMonster) {
                int r = (int)(level->random.genrand_int32() % 5);
                int mtype = 32;
                if (r == 1) mtype = 34;
                else if (r == 2) mtype = 35;
                else if (r == 3) mtype = 33;
                else if (r == 4) mtype = 37;
                spawnData = Biome::MobSpawnerData(10, mtype, 1, 4);
              } else if (isCreature) {
                int r = (int)(level->random.genrand_int32() % 6);
                int mtype = 10;
                if (r == 1) mtype = 11;
                else if (r == 2) mtype = 12;
                else if (r == 3) mtype = 13;
                else if (r == 4) mtype = 14;
                else if (r == 5) mtype = 26;
                spawnData = Biome::MobSpawnerData(10, mtype, 1, 4);
              } else if (isWater) {
                int r = (int)(level->random.genrand_int32() % 5);
                int mtype = (r == 0) ? 17 : ((r == 1) ? 27 : ((r == 2) ? 28 : ((r == 3) ? 29 : 30)));
                spawnData = Biome::MobSpawnerData(10, mtype, 1, 4);
              } else {
                break;
              }
            }
          }

          if (isMonster) {
            if (spawnData.mobtype != 37 && level->getRawBrightness(sx, sy, sz) > 7)
              continue;
          } else if (isCreature) {
            int tileBelow = level->getTile(sx, sy - 1, sz);
            if (tileBelow != Tile::grass->blockID && tileBelow != Tile::snow->blockID && tileBelow != Tile::ice->blockID && tileBelow != Tile::topSnow->blockID)
              continue;
            if (spawnData.mobtype != 37 && level->getRawBrightness(sx, sy, sz) < 9)
              continue;
          }

          Mob *mob = MobFactory::CreateMob(spawnData.mobtype, level);
          if (!mob)
            continue;

          mob->moveTo(fx, fy, fz, level->random.nextFloat() * 360.0f, 0.0f);
          if (mob->canSpawn() || !level->isSolidBlockingTile(sx, sy, sz)) {
            level->addEntity(mob);
            MobSpawner::finalizeMobSettings(mob, level, fx, fy, fz);
            if (spawnData.mobtype >= 27 && spawnData.mobtype <= 30) {
              AbstractFish* fish = (AbstractFish*)mob;
              if (!schoolLeader) {
                schoolLeader = fish;
              } else {
                fish->startFollowing(schoolLeader);
              }
            }
            ++currentCount;
            ++totalSpawned;
            ++packSpawned;
            if (packSpawned >= mob->getMaxSpawnClusterSize()) {
              break;
            }
          } else {
            delete mob;
          }
        }
        if (packSpawned > 0)
          break;
      }
    }
  }

  return totalSpawned;
}
