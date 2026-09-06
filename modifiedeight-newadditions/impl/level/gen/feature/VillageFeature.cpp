#include <level/gen/feature/VillageFeature.hpp>
#include <level/Level.hpp>
#include <entity/Villager.hpp>
#include <level/MobSpawner.hpp>
#include <level/LightLayer.hpp>
#include <level/chunk/LevelChunk.hpp>
#include <level/biome/Biome.hpp>
#include <tile/Tile.hpp>
#include <tile/entity/ChestTileEntity.hpp>
#include <item/Item.hpp>
#include <item/ItemInstance.hpp>
#include <util/Random.hpp>
#include <math.h>

VillageFeature::VillageFeature(bool_t update)
	: Feature(update) {
}

VillageFeature::~VillageFeature() {
}

VillageType VillageFeature::getVillageType(Biome* biome) {
	if (biome == Biome::desert) return VillageType::DESERT;
	if (biome == Biome::taiga) return VillageType::TAIGA;
	if (biome == Biome::icePeaks || biome == Biome::tundra) return VillageType::SNOWY;
	return VillageType::PLAINS;
}

VillageMaterials VillageFeature::getMaterials(VillageType type) {
	VillageMaterials m;
	if (type == VillageType::DESERT) {
		m.woodId = Tile::sandStone->blockID;
		m.woodMeta = 2;
		m.plankId = Tile::sandStone->blockID;
		m.plankMeta = 0;
		m.stoneId = Tile::sandStone->blockID;
		m.stoneMeta = 0;
		m.stairId = Tile::stairs_sandStone ? Tile::stairs_sandStone->blockID : Tile::stairs_wood->blockID;
		m.slabId = Tile::stoneSlabHalf->blockID;
		m.slabMeta = 1;
		m.fenceId = Tile::fence->blockID;
		m.pathId = Tile::sandStone->blockID;
		m.pathMeta = 2;
		m.hasSnow = 0;
	} else if (type == VillageType::TAIGA) {
		m.woodId = Tile::treeTrunk->blockID;
		m.woodMeta = 1;
		m.plankId = Tile::wood->blockID;
		m.plankMeta = 1;
		m.stoneId = Tile::stoneBrick->blockID;
		m.stoneMeta = 0;
		m.stairId = Tile::woodStairsDark ? Tile::woodStairsDark->blockID : Tile::stairs_wood->blockID;
		m.slabId = Tile::woodSlabHalf ? Tile::woodSlabHalf->blockID : Tile::stoneSlabHalf->blockID;
		m.slabMeta = 1;
		m.fenceId = Tile::fence->blockID;
		m.pathId = Tile::gravel->blockID;
		m.pathMeta = 0;
		m.hasSnow = 0;
	} else if (type == VillageType::SNOWY) {
		m.woodId = Tile::treeTrunk->blockID;
		m.woodMeta = 1;
		m.plankId = Tile::wood->blockID;
		m.plankMeta = 1;
		m.stoneId = Tile::stoneBrick->blockID;
		m.stoneMeta = 0;
		m.stairId = Tile::woodStairsDark ? Tile::woodStairsDark->blockID : Tile::stairs_wood->blockID;
		m.slabId = Tile::woodSlabHalf ? Tile::woodSlabHalf->blockID : Tile::stoneSlabHalf->blockID;
		m.slabMeta = 1;
		m.fenceId = Tile::fence->blockID;
		m.pathId = Tile::gravel->blockID;
		m.pathMeta = 0;
		m.hasSnow = 1;
	} else {
		m.woodId = Tile::treeTrunk->blockID;
		m.woodMeta = 0;
		m.plankId = Tile::wood->blockID;
		m.plankMeta = 0;
		m.stoneId = Tile::stoneBrick->blockID;
		m.stoneMeta = 0;
		m.stairId = Tile::stairs_wood->blockID;
		m.slabId = Tile::woodSlabHalf ? Tile::woodSlabHalf->blockID : Tile::stoneSlabHalf->blockID;
		m.slabMeta = 0;
		m.fenceId = Tile::fence->blockID;
		m.pathId = Tile::gravel->blockID;
		m.pathMeta = 0;
		m.hasSnow = 0;
	}
	return m;
}

static int32_t rotateStairData(int32_t data, int32_t rotation) {
	int32_t dir = data & 3;
	int32_t upsideDown = data & 4;
	int32_t newDir = dir;
	if (rotation == 1) {
		if (dir == 0) newDir = 2;
		else if (dir == 1) newDir = 3;
		else if (dir == 2) newDir = 1;
		else if (dir == 3) newDir = 0;
	} else if (rotation == 2) {
		if (dir == 0) newDir = 1;
		else if (dir == 1) newDir = 0;
		else if (dir == 2) newDir = 3;
		else if (dir == 3) newDir = 2;
	} else if (rotation == 3) {
		if (dir == 0) newDir = 3;
		else if (dir == 1) newDir = 2;
		else if (dir == 2) newDir = 0;
		else if (dir == 3) newDir = 1;
	}
	return newDir | upsideDown;
}

static int32_t rotateTorchData(int32_t data, int32_t rotation) {
	if (data == 5) return 5;
	if (rotation == 1) {
		if (data == 1) return 3;
		if (data == 2) return 4;
		if (data == 3) return 2;
		if (data == 4) return 1;
	} else if (rotation == 2) {
		if (data == 1) return 2;
		if (data == 2) return 1;
		if (data == 3) return 4;
		if (data == 4) return 3;
	} else if (rotation == 3) {
		if (data == 1) return 4;
		if (data == 2) return 3;
		if (data == 3) return 1;
		if (data == 4) return 2;
	}
	return data;
}

static int32_t rotateFurnaceData(int32_t data, int32_t rotation) {
	if (rotation == 1) {
		if (data == 2) return 5;
		if (data == 3) return 4;
		if (data == 4) return 2;
		if (data == 5) return 3;
	} else if (rotation == 2) {
		if (data == 2) return 3;
		if (data == 3) return 2;
		if (data == 4) return 5;
		if (data == 5) return 4;
	} else if (rotation == 3) {
		if (data == 2) return 4;
		if (data == 3) return 5;
		if (data == 4) return 3;
		if (data == 5) return 2;
	}
	return data;
}

static int32_t rotateDoorData(int32_t data, int32_t rotation) {
	if (data & 8) return data;
	int32_t dir = data & 3;
	int32_t open = data & 4;
	int32_t newDir = dir;
	if (rotation == 1) {
		if (dir == 0) newDir = 1;
		else if (dir == 1) newDir = 2;
		else if (dir == 2) newDir = 3;
		else if (dir == 3) newDir = 0;
	} else if (rotation == 2) {
		if (dir == 0) newDir = 2;
		else if (dir == 1) newDir = 3;
		else if (dir == 2) newDir = 0;
		else if (dir == 3) newDir = 1;
	} else if (rotation == 3) {
		if (dir == 0) newDir = 3;
		else if (dir == 1) newDir = 0;
		else if (dir == 2) newDir = 1;
		else if (dir == 3) newDir = 2;
	}
	return (newDir & 3) | (open & 4);
}

static int32_t rotateBedData(int32_t data, int32_t rotation) {
	int32_t head = data & 8;
	int32_t dir = (data + rotation) & 3;
	return head | dir;
}

static void fillHouseLoot(Level* level, Random* random, int32_t x, int32_t y, int32_t z) {
	ChestTileEntity* te = (ChestTileEntity*)level->getTileEntity(x, y, z);
	if (!te) {
		te = new ChestTileEntity();
		level->setTileEntity(x, y, z, te);
	}

	int32_t numItems = 3 + (random->genrand_int32() % 4);
	bool gearAdded = false;

	for (int32_t i = 0; i < numItems; ++i) {
		int32_t slot = random->genrand_int32() % 27;
		int32_t roll = random->genrand_int32() % 100;
		ItemInstance inst(Item::bread, 2 + (random->genrand_int32() % 3), 0);

		if (!gearAdded && roll >= 80) {
			gearAdded = true;
			int gearRoll = random->genrand_int32() % 100;
			if (gearRoll < 60) {
				int toolOrArmor = random->genrand_int32() % 6;
				if (toolOrArmor == 0) inst = ItemInstance(Item::sword_iron, 1, 0);
				else if (toolOrArmor == 1) inst = ItemInstance(Item::pickAxe_iron, 1, 0);
				else if (toolOrArmor == 2) inst = ItemInstance(Item::helmet_iron, 1, 0);
				else if (toolOrArmor == 3) inst = ItemInstance(Item::chestplate_iron, 1, 0);
				else if (toolOrArmor == 4) inst = ItemInstance(Item::leggings_iron, 1, 0);
				else inst = ItemInstance(Item::boots_iron, 1, 0);
			} else if (gearRoll < 90) {
				int toolOrArmor = random->genrand_int32() % 6;
				if (toolOrArmor == 0) inst = ItemInstance(Item::sword_gold, 1, 0);
				else if (toolOrArmor == 1) inst = ItemInstance(Item::pickAxe_gold, 1, 0);
				else if (toolOrArmor == 2) inst = ItemInstance(Item::helmet_gold, 1, 0);
				else if (toolOrArmor == 3) inst = ItemInstance(Item::chestplate_gold, 1, 0);
				else if (toolOrArmor == 4) inst = ItemInstance(Item::leggings_gold, 1, 0);
				else inst = ItemInstance(Item::boots_gold, 1, 0);
			} else {
				int toolOrArmor = random->genrand_int32() % 6;
				if (toolOrArmor == 0) inst = ItemInstance(Item::sword_emerald, 1, 0);
				else if (toolOrArmor == 1) inst = ItemInstance(Item::pickAxe_emerald, 1, 0);
				else if (toolOrArmor == 2) inst = ItemInstance(Item::helmet_diamond, 1, 0);
				else if (toolOrArmor == 3) inst = ItemInstance(Item::chestplate_diamond, 1, 0);
				else if (toolOrArmor == 4) inst = ItemInstance(Item::leggings_diamond, 1, 0);
				else inst = ItemInstance(Item::boots_diamond, 1, 0);
			}
		} else {
			int resourceRoll = random->genrand_int32() % 13;
			if (resourceRoll == 0) {
				inst = ItemInstance(Item::bread, 1 + (random->genrand_int32() % 4), 0);
			} else if (resourceRoll == 1) {
				inst = ItemInstance(Item::apple, 1 + (random->genrand_int32() % 3), 0);
			} else if (resourceRoll == 2) {
				inst = ItemInstance(Item::ironIngot, 1 + (random->genrand_int32() % 3), 0);
			} else if (resourceRoll == 3) {
				inst = ItemInstance(Item::stick, 2 + (random->genrand_int32() % 5), 0);
			} else if (resourceRoll == 4) {
				inst = ItemInstance(Item::coal, 2 + (random->genrand_int32() % 4), 0);
			} else if (resourceRoll == 5) {
				inst = ItemInstance(Item::potato, 2 + (random->genrand_int32() % 4), 0);
			} else if (resourceRoll == 6) {
				inst = ItemInstance(Item::carrot, 2 + (random->genrand_int32() % 4), 0);
			} else if (resourceRoll == 7) {
				inst = ItemInstance(Item::beetroot, 2 + (random->genrand_int32() % 4), 0);
			} else if (resourceRoll == 8) {
				inst = ItemInstance(Item::seeds_wheat, 2 + (random->genrand_int32() % 5), 0);
			} else if (resourceRoll == 9) {
				inst = ItemInstance(Item::seeds_pumpkin, 2 + (random->genrand_int32() % 4), 0);
			} else if (resourceRoll == 10) {
				inst = ItemInstance(Item::seeds_melon, 2 + (random->genrand_int32() % 4), 0);
			} else if (resourceRoll == 11) {
				inst = ItemInstance(Item::seeds_beetroot, 2 + (random->genrand_int32() % 4), 0);
			} else {
				if ((random->genrand_int32() % 3) == 0) {
					inst = ItemInstance(Item::goldIngot, 1 + (random->genrand_int32() % 2), 0);
				} else {
					inst = ItemInstance(Item::emerald, 1, 0);
				}
			}
		}
		te->setItem(slot, &inst);
	}

	if ((random->genrand_int32() % 2) == 0) {
		int32_t dyeSlot = random->genrand_int32() % 27;
		ItemInstance dyeInst(Item::dye_powder, 2 + (random->genrand_int32() % 4), 0);
		te->setItem(dyeSlot, &dyeInst);
	}

	if (Tile::sponge && (random->genrand_int32() % 3) == 0) {
		int32_t spongeSlot = random->genrand_int32() % 27;
		ItemInstance spongeInst(Tile::sponge, 1 + (random->genrand_int32() % 3), 0);
		te->setItem(spongeSlot, &spongeInst);
	}
}

void VillageFeature::placeRotated(Level* level, int32_t originX, int32_t originY, int32_t originZ, int32_t lx, int32_t ly, int32_t lz, int32_t sizeX, int32_t sizeZ, int32_t blockId, int32_t data, int32_t rotation) {
	int32_t rx = lx;
	int32_t rz = lz;
	if (rotation == 1) {
		rx = (sizeZ - 1) - lz;
		rz = lx;
	} else if (rotation == 2) {
		rx = (sizeX - 1) - lx;
		rz = (sizeZ - 1) - lz;
	} else if (rotation == 3) {
		rx = lz;
		rz = (sizeX - 1) - lx;
	}

	int32_t wx = originX + rx;
	int32_t wy = originY + ly;
	int32_t wz = originZ + rz;

	if (wy > 0 && wy < 128) {
		if (blockId == Tile::stairs_wood->blockID || blockId == Tile::stairs_sandStone->blockID || (Tile::woodStairsDark && blockId == Tile::woodStairsDark->blockID)) {
			data = rotateStairData(data, rotation);
		} else if (blockId == Tile::torch->blockID) {
			data = rotateTorchData(data, rotation);
		} else if (blockId == Tile::furnace->blockID || blockId == Tile::chest->blockID) {
			data = rotateFurnaceData(data, rotation);
		} else if (blockId == (Tile::door_wood ? Tile::door_wood->blockID : 64) || (Tile::door_spruce && blockId == Tile::door_spruce->blockID) || (Tile::door_birch && blockId == Tile::door_birch->blockID) || (Tile::door_iron && blockId == Tile::door_iron->blockID)) {
			data = rotateDoorData(data, rotation);
		} else if (Tile::bed && (blockId == Tile::bed->blockID || (blockId >= 200 && blockId <= 215))) {
			data = rotateBedData(data, rotation);
		}
		this->placeBlock(level, wx, wy, wz, blockId, data);
		if (blockId == Tile::torch->blockID) {
			Tile::torch->onPlace(level, wx, wy, wz);
			level->updateLight(LightLayer::Block, wx - 14, wy - 14, wz - 14, wx + 14, wy + 14, wz + 14);
		}
	}
}

void VillageFeature::fillFoundation(Level* level, int32_t minX, int32_t maxX, int32_t minZ, int32_t maxZ, int32_t baseY, const VillageMaterials& mat) {
	for (int32_t x = minX; x <= maxX; ++x) {
		for (int32_t z = minZ; z <= maxZ; ++z) {
			bool isCorner = (x == minX || x == maxX) && (z == minZ || z == maxZ);
			int32_t fBlock = isCorner ? mat.woodId : mat.stoneId;
			int32_t fData = isCorner ? mat.woodMeta : mat.stoneMeta;

			for (int32_t y = baseY - 1; y >= baseY - 14; --y) {
				if (y <= 0) break;
				int32_t cur = level->getTile(x, y, z);
				if (cur != 0 && cur != Tile::water->blockID && cur != Tile::calmWater->blockID && cur != Tile::tallgrass->blockID && cur != Tile::flower->blockID && cur != Tile::rose->blockID && cur != Tile::topSnow->blockID && cur != Tile::treeTrunk->blockID && cur != Tile::leaves->blockID && (!Tile::doublePlant || cur != Tile::doublePlant->blockID)) {
					break;
				}
				this->placeBlock(level, x, y, z, fBlock, fData);
			}
		}
	}
}

static void clearTreeArea(Level* level, int32_t minX, int32_t maxX, int32_t minZ, int32_t maxZ, int32_t baseY, int32_t height) {
	for (int32_t x = minX - 2; x <= maxX + 2; ++x) {
		for (int32_t z = minZ - 2; z <= maxZ + 2; ++z) {
			for (int32_t y = baseY + 1; y <= baseY + height; ++y) {
				if (y < 128) {
					int32_t t = level->getTile(x, y, z);
					if (t == Tile::treeTrunk->blockID || t == Tile::leaves->blockID || (Tile::vine && t == Tile::vine->blockID) || t == Tile::tallgrass->blockID || t == Tile::flower->blockID || t == Tile::rose->blockID || t == Tile::topSnow->blockID || (Tile::doublePlant && t == Tile::doublePlant->blockID)) {
						level->setTileAndDataNoUpdate(x, y, z, 0, 0);
					}
				}
			}
		}
	}
}

void VillageFeature::placeWell(Level* level, Random* random, int32_t x, int32_t y, int32_t z, const VillageMaterials& mat) {
	clearTreeArea(level, x - 1, x + 4, z - 1, z + 4, y, 10);

	for (int32_t fx = -1; fx <= 4; ++fx) {
		for (int32_t fz = -1; fz <= 4; ++fz) {
			for (int32_t fy = y - 1; fy >= y - 14; --fy) {
				if (fy <= 0) break;
				int32_t cur = level->getTile(x + fx, fy, z + fz);
				if (cur != 0 && cur != Tile::water->blockID && cur != Tile::calmWater->blockID && cur != Tile::tallgrass->blockID && cur != Tile::flower->blockID && cur != Tile::rose->blockID && cur != Tile::topSnow->blockID && cur != Tile::treeTrunk->blockID && cur != Tile::leaves->blockID && (!Tile::doublePlant || cur != Tile::doublePlant->blockID)) {
					break;
				}
				this->placeBlock(level, x + fx, fy, z + fz, mat.stoneId, mat.stoneMeta);
			}
			this->placeBlock(level, x + fx, y, z + fz, mat.pathId, mat.pathMeta);
		}
	}

	for (int32_t wx = 0; wx <= 3; ++wx) {
		for (int32_t wz = 0; wz <= 3; ++wz) {
			bool isWall = (wx == 0 || wx == 3 || wz == 0 || wz == 3);
			for (int32_t wy = -6; wy <= 0; ++wy) {
				if (isWall) {
					this->placeBlock(level, x + wx, y + 1 + wy, z + wz, mat.stoneId, mat.stoneMeta);
				} else {
					int32_t bId = (wy <= -1) ? Tile::calmWater->blockID : 0;
					this->placeBlock(level, x + wx, y + 1 + wy, z + wz, bId, 0);
				}
			}
			this->placeBlock(level, x + wx, y - 6, z + wz, mat.stoneId, mat.stoneMeta);
		}
	}

	this->placeBlock(level, x, y + 2, z, mat.fenceId, 0);
	this->placeBlock(level, x + 3, y + 2, z, mat.fenceId, 0);
	this->placeBlock(level, x, y + 2, z + 3, mat.fenceId, 0);
	this->placeBlock(level, x + 3, y + 2, z + 3, mat.fenceId, 0);

	this->placeBlock(level, x, y + 3, z, mat.fenceId, 0);
	this->placeBlock(level, x + 3, y + 3, z, mat.fenceId, 0);
	this->placeBlock(level, x, y + 3, z + 3, mat.fenceId, 0);
	this->placeBlock(level, x + 3, y + 3, z + 3, mat.fenceId, 0);

	for (int32_t rx = 0; rx <= 3; ++rx) {
		for (int32_t rz = 0; rz <= 3; ++rz) {
			this->placeBlock(level, x + rx, y + 4, z + rz, mat.stoneId, mat.stoneMeta);
		}
	}
}

void VillageFeature::placeSmallHouse(Level* level, Random* random, int32_t x, int32_t y, int32_t z, int32_t rotation, int32_t variant, const VillageMaterials& mat) {
	int32_t sizeX = 5;
	int32_t sizeZ = 5;

	int32_t minX = x;
	int32_t maxX = x + sizeX - 1;
	int32_t minZ = z;
	int32_t maxZ = z + sizeZ - 1;
	if (rotation == 1 || rotation == 3) {
		maxX = x + sizeZ - 1;
		maxZ = z + sizeX - 1;
	}

	clearTreeArea(level, minX, maxX, minZ, maxZ, y, 10);
	this->fillFoundation(level, minX, maxX, minZ, maxZ, y, mat);

	for (int32_t lx = 0; lx < sizeX; ++lx) {
		for (int32_t lz = 0; lz < sizeZ; ++lz) {
			bool isCorner = (lx == 0 || lx == sizeX - 1) && (lz == 0 || lz == sizeZ - 1);
			int32_t floorBlock = isCorner ? mat.woodId : mat.stoneId;
			int32_t floorMeta = isCorner ? mat.woodMeta : mat.stoneMeta;
			this->placeRotated(level, x, y, z, lx, 0, lz, sizeX, sizeZ, floorBlock, floorMeta, rotation);
		}
	}

	for (int32_t ly = 1; ly <= 4; ++ly) {
		for (int32_t lx = 0; lx < sizeX; ++lx) {
			for (int32_t lz = 0; lz < sizeZ; ++lz) {
				bool isCorner = (lx == 0 || lx == sizeX - 1) && (lz == 0 || lz == sizeZ - 1);
				bool isWall = (lx == 0 || lx == sizeX - 1 || lz == 0 || lz == sizeZ - 1);

				if (isCorner) {
					this->placeRotated(level, x, y, z, lx, ly, lz, sizeX, sizeZ, mat.woodId, mat.woodMeta, rotation);
				} else if (ly == 4) {
					this->placeRotated(level, x, y, z, lx, ly, lz, sizeX, sizeZ, mat.plankId, mat.plankMeta, rotation);
				} else if (isWall) {
					this->placeRotated(level, x, y, z, lx, ly, lz, sizeX, sizeZ, mat.plankId, mat.plankMeta, rotation);
				} else {
					this->placeRotated(level, x, y, z, lx, ly, lz, sizeX, sizeZ, 0, 0, rotation);
				}
			}
		}
	}

	this->placeRotated(level, x, y, z, 2, 1, 0, sizeX, sizeZ, Tile::door_wood ? Tile::door_wood->blockID : 0, 1, rotation);
	this->placeRotated(level, x, y, z, 2, 2, 0, sizeX, sizeZ, Tile::door_wood ? Tile::door_wood->blockID : 0, 8, rotation);

	this->placeRotated(level, x, y, z, 0, 2, 2, sizeX, sizeZ, Tile::thinGlass->blockID, 0, rotation);
	this->placeRotated(level, x, y, z, 4, 2, 2, sizeX, sizeZ, Tile::thinGlass->blockID, 0, rotation);
	this->placeRotated(level, x, y, z, 2, 2, 4, sizeX, sizeZ, Tile::thinGlass->blockID, 0, rotation);
	this->placeRotated(level, x, y, z, 2, 3, 1, sizeX, sizeZ, Tile::torch->blockID, 3, rotation);
	this->placeRotated(level, x, y, z, 2, 3, 3, sizeX, sizeZ, Tile::torch->blockID, 4, rotation);

	if (variant == 1 || ((random->genrand_int32() % 2) == 0)) {
		this->placeRotated(level, x, y, z, 2, 1, 3, sizeX, sizeZ, Tile::stonecutterBench ? Tile::stonecutterBench->blockID : Tile::workBench->blockID, 2, rotation);
	}

	if ((random->genrand_int32() % 4) == 0) {
		this->placeRotated(level, x, y, z, 1, 1, 3, sizeX, sizeZ, Tile::chest->blockID, 2, rotation);
		int32_t chestLx = 1;
		int32_t chestLz = 3;
		int32_t crx = chestLx;
		int32_t crz = chestLz;
		if (rotation == 1) {
			crx = (sizeZ - 1) - chestLz;
			crz = chestLx;
		} else if (rotation == 2) {
			crx = (sizeX - 1) - chestLx;
			crz = (sizeZ - 1) - chestLz;
		} else if (rotation == 3) {
			crx = chestLz;
			crz = (sizeX - 1) - chestLx;
		}
		fillHouseLoot(level, random, x + crx, y + 1, z + crz);
	}

	if (variant == 0) {
		for (int32_t lx = 0; lx < sizeX; ++lx) {
			for (int32_t lz = 0; lz < sizeZ; ++lz) {
				if (lx == 0 || lx == sizeX - 1 || lz == 0 || lz == sizeZ - 1) {
					this->placeRotated(level, x, y, z, lx, 5, lz, sizeX, sizeZ, mat.fenceId, 0, rotation);
				}
			}
		}
	} else {
		for (int32_t gx = 0; gx < sizeX; ++gx) {
			this->placeRotated(level, x, y, z, gx, 5, 1, sizeX, sizeZ, mat.plankId, mat.plankMeta, rotation);
			this->placeRotated(level, x, y, z, gx, 5, 2, sizeX, sizeZ, mat.plankId, mat.plankMeta, rotation);
			this->placeRotated(level, x, y, z, gx, 5, 3, sizeX, sizeZ, mat.plankId, mat.plankMeta, rotation);
			this->placeRotated(level, x, y, z, gx, 6, 2, sizeX, sizeZ, mat.plankId, mat.plankMeta, rotation);
		}

		for (int32_t lx = -1; lx <= sizeX; ++lx) {
			this->placeRotated(level, x, y, z, lx, 3, -1, sizeX, sizeZ, mat.stairId, 2, rotation);
			this->placeRotated(level, x, y, z, lx, 3, 5, sizeX, sizeZ, mat.stairId, 3, rotation);
			this->placeRotated(level, x, y, z, lx, 4, 0, sizeX, sizeZ, mat.stairId, 2, rotation);
			this->placeRotated(level, x, y, z, lx, 4, 4, sizeX, sizeZ, mat.stairId, 3, rotation);
			this->placeRotated(level, x, y, z, lx, 5, 1, sizeX, sizeZ, mat.stairId, 2, rotation);
			this->placeRotated(level, x, y, z, lx, 5, 3, sizeX, sizeZ, mat.stairId, 3, rotation);
			this->placeRotated(level, x, y, z, lx, 6, 2, sizeX, sizeZ, mat.slabId, mat.slabMeta, rotation);
		}
	}

	if (Tile::bed) {
		this->placeRotated(level, x, y, z, 3, 1, 2, sizeX, sizeZ, Tile::bed->blockID, 0, rotation);
		this->placeRotated(level, x, y, z, 3, 1, 3, sizeX, sizeZ, Tile::bed->blockID, 8, rotation);
	}

	if (!level->isClientMaybe) {
		Villager* v = new Villager(level);
		int32_t vrx = 2, vrz = 2;
		if (rotation == 1) { vrx = sizeZ - 2; vrz = 2; }
		else if (rotation == 2) { vrx = sizeX - 2; vrz = sizeZ - 2; }
		else if (rotation == 3) { vrx = 2; vrz = sizeX - 2; }
		v->houseX = x + vrx; v->houseY = y + 1; v->houseZ = z + vrz;
		v->hasBed = 1;
		v->profession = (variant + rotation) % 5;
		v->moveTo((float)(x + vrx) + 0.5f, (float)(y + 1), (float)(z + vrz) + 0.5f, 0.0f, 0.0f);
		MobSpawner::finalizeMobSettings(v, level, 0.0, 0.0, 0.0);
		level->addEntity(v);
	}

	if (mat.hasSnow) {
		for (int32_t lx = minX - 1; lx <= maxX + 1; ++lx) {
			for (int32_t lz = minZ - 1; lz <= maxZ + 1; ++lz) {
				int32_t hm = level->getHeightmap(lx, lz);
				if (hm > 0 && level->isEmptyTile(lx, hm, lz)) {
					this->placeBlock(level, lx, hm, lz, Tile::topSnow->blockID, 0);
				}
			}
		}
	}

	for (int32_t lx = minX - 2; lx <= maxX + 2; lx += 16) {
		for (int32_t lz = minZ - 2; lz <= maxZ + 2; lz += 16) {
			LevelChunk* c = level->getChunkAt(lx, lz);
			if (c) c->recalcHeightmap();
		}
	}
	level->updateLight(LightLayer::Sky, minX - 2, y, minZ - 2, maxX + 2, y + 8, maxZ + 2);
	level->updateLight(LightLayer::Block, minX - 2, y, minZ - 2, maxX + 2, y + 8, maxZ + 2);
}

void VillageFeature::placeLargeHouse(Level* level, Random* random, int32_t x, int32_t y, int32_t z, int32_t rotation, const VillageMaterials& mat) {
	int32_t sizeX = 8;
	int32_t sizeZ = 6;

	int32_t minX = x;
	int32_t maxX = x + sizeX - 1;
	int32_t minZ = z;
	int32_t maxZ = z + sizeZ - 1;
	if (rotation == 1 || rotation == 3) {
		maxX = x + sizeZ - 1;
		maxZ = z + sizeX - 1;
	}

	clearTreeArea(level, minX, maxX, minZ, maxZ, y, 11);
	this->fillFoundation(level, minX, maxX, minZ, maxZ, y, mat);

	for (int32_t lx = 0; lx < sizeX; ++lx) {
		for (int32_t lz = 0; lz < sizeZ; ++lz) {
			bool isCorner = (lx == 0 || lx == sizeX - 1) && (lz == 0 || lz == sizeZ - 1);
			int32_t floorBlock = isCorner ? mat.woodId : mat.stoneId;
			int32_t floorMeta = isCorner ? mat.woodMeta : mat.stoneMeta;
			this->placeRotated(level, x, y, z, lx, 0, lz, sizeX, sizeZ, floorBlock, floorMeta, rotation);
		}
	}

	for (int32_t ly = 1; ly <= 5; ++ly) {
		for (int32_t lx = 0; lx < sizeX; ++lx) {
			for (int32_t lz = 0; lz < sizeZ; ++lz) {
				bool isCorner = (lx == 0 || lx == sizeX - 1) && (lz == 0 || lz == sizeZ - 1);
				bool isWall = (lx == 0 || lx == sizeX - 1 || lz == 0 || lz == sizeZ - 1);

				if (isCorner) {
					this->placeRotated(level, x, y, z, lx, ly, lz, sizeX, sizeZ, mat.woodId, mat.woodMeta, rotation);
				} else if (ly == 4 || ly == 5) {
					if (ly == 4 || isWall) {
						this->placeRotated(level, x, y, z, lx, ly, lz, sizeX, sizeZ, mat.plankId, mat.plankMeta, rotation);
					}
				} else if (isWall) {
					this->placeRotated(level, x, y, z, lx, ly, lz, sizeX, sizeZ, mat.plankId, mat.plankMeta, rotation);
				} else {
					this->placeRotated(level, x, y, z, lx, ly, lz, sizeX, sizeZ, 0, 0, rotation);
				}
			}
		}
	}

	this->placeRotated(level, x, y, z, 2, 1, 0, sizeX, sizeZ, Tile::door_wood ? Tile::door_wood->blockID : 0, 1, rotation);
	this->placeRotated(level, x, y, z, 2, 2, 0, sizeX, sizeZ, Tile::door_wood ? Tile::door_wood->blockID : 0, 8, rotation);

	for (int32_t wy = 2; wy <= 3; ++wy) {
		this->placeRotated(level, x, y, z, 4, wy, 0, sizeX, sizeZ, Tile::thinGlass->blockID, 0, rotation);
		this->placeRotated(level, x, y, z, 5, wy, 0, sizeX, sizeZ, Tile::thinGlass->blockID, 0, rotation);

		this->placeRotated(level, x, y, z, 0, wy, 2, sizeX, sizeZ, Tile::thinGlass->blockID, 0, rotation);
		this->placeRotated(level, x, y, z, 0, wy, 3, sizeX, sizeZ, Tile::thinGlass->blockID, 0, rotation);

		this->placeRotated(level, x, y, z, 7, wy, 2, sizeX, sizeZ, Tile::thinGlass->blockID, 0, rotation);
		this->placeRotated(level, x, y, z, 7, wy, 3, sizeX, sizeZ, Tile::thinGlass->blockID, 0, rotation);

		this->placeRotated(level, x, y, z, 3, wy, 5, sizeX, sizeZ, Tile::thinGlass->blockID, 0, rotation);
		this->placeRotated(level, x, y, z, 4, wy, 5, sizeX, sizeZ, Tile::thinGlass->blockID, 0, rotation);
	}

	this->placeRotated(level, x, y, z, 1, 1, 4, sizeX, sizeZ, Tile::workBench->blockID, 0, rotation);
	this->placeRotated(level, x, y, z, 2, 1, 4, sizeX, sizeZ, Tile::furnace->blockID, 2, rotation);
	this->placeRotated(level, x, y, z, 6, 1, 4, sizeX, sizeZ, Tile::chest->blockID, 2, rotation);

	int32_t rx = 6;
	int32_t rz = 4;
	if (rotation == 1) { rx = (sizeZ - 1) - 4; rz = 6; }
	else if (rotation == 2) { rx = (sizeX - 1) - 6; rz = (sizeZ - 1) - 4; }
	else if (rotation == 3) { rx = 4; rz = (sizeX - 1) - 6; }
	fillHouseLoot(level, random, x + rx, y + 1, z + rz);
	this->placeRotated(level, x, y, z, 2, 3, 1, sizeX, sizeZ, Tile::torch->blockID, 3, rotation);
	this->placeRotated(level, x, y, z, 5, 3, 1, sizeX, sizeZ, Tile::torch->blockID, 3, rotation);
	this->placeRotated(level, x, y, z, 4, 3, 4, sizeX, sizeZ, Tile::torch->blockID, 4, rotation);

	for (int32_t gx = 0; gx < sizeX; ++gx) {
		for (int32_t lz = 0; lz <= 5; ++lz) {
			this->placeRotated(level, x, y, z, gx, 5, lz, sizeX, sizeZ, mat.plankId, mat.plankMeta, rotation);
		}
		for (int32_t lz = 1; lz <= 4; ++lz) {
			this->placeRotated(level, x, y, z, gx, 6, lz, sizeX, sizeZ, mat.plankId, mat.plankMeta, rotation);
		}
		for (int32_t lz = 2; lz <= 3; ++lz) {
			this->placeRotated(level, x, y, z, gx, 7, lz, sizeX, sizeZ, mat.plankId, mat.plankMeta, rotation);
		}
	}

	for (int32_t lx = -1; lx <= sizeX; ++lx) {
		this->placeRotated(level, x, y, z, lx, 4, -1, sizeX, sizeZ, mat.stairId, 2, rotation);
		this->placeRotated(level, x, y, z, lx, 4, 6, sizeX, sizeZ, mat.stairId, 3, rotation);
		this->placeRotated(level, x, y, z, lx, 5, 0, sizeX, sizeZ, mat.stairId, 2, rotation);
		this->placeRotated(level, x, y, z, lx, 5, 5, sizeX, sizeZ, mat.stairId, 3, rotation);
		this->placeRotated(level, x, y, z, lx, 6, 1, sizeX, sizeZ, mat.stairId, 2, rotation);
		this->placeRotated(level, x, y, z, lx, 6, 4, sizeX, sizeZ, mat.stairId, 3, rotation);
		this->placeRotated(level, x, y, z, lx, 7, 2, sizeX, sizeZ, mat.stairId, 2, rotation);
		this->placeRotated(level, x, y, z, lx, 7, 3, sizeX, sizeZ, mat.stairId, 3, rotation);
	}

	if (!level->isClientMaybe) {
		Villager* v = new Villager(level);
		int32_t vrx = 2, vrz = 2;
		if (rotation == 1) { vrx = sizeZ - 2; vrz = 2; }
		else if (rotation == 2) { vrx = sizeX - 2; vrz = sizeZ - 2; }
		else if (rotation == 3) { vrx = 2; vrz = sizeX - 2; }
		v->houseX = x + vrx; v->houseY = y + 1; v->houseZ = z + vrz;
		v->hasBed = 1;
		v->profession = 3;
		v->moveTo((float)(x + vrx) + 0.5f, (float)(y + 1), (float)(z + vrz) + 0.5f, 0.0f, 0.0f);
		MobSpawner::finalizeMobSettings(v, level, 0.0, 0.0, 0.0);
		level->addEntity(v);
	}

	for (int32_t lx = minX - 2; lx <= maxX + 2; lx += 16) {
		for (int32_t lz = minZ - 2; lz <= maxZ + 2; lz += 16) {
			LevelChunk* c = level->getChunkAt(lx, lz);
			if (c) c->recalcHeightmap();
		}
	}
	level->updateLight(LightLayer::Sky, minX - 2, y, minZ - 2, maxX + 2, y + 9, maxZ + 2);
	level->updateLight(LightLayer::Block, minX - 2, y, minZ - 2, maxX + 2, y + 9, maxZ + 2);
}

void VillageFeature::placeFarm(Level* level, Random* random, int32_t x, int32_t y, int32_t z, int32_t rotation, const VillageMaterials& mat) {
	int32_t sizeX = 7;
	int32_t sizeZ = 9;

	int32_t minX = x;
	int32_t maxX = x + sizeX - 1;
	int32_t minZ = z;
	int32_t maxZ = z + sizeZ - 1;
	if (rotation == 1 || rotation == 3) {
		maxX = x + sizeZ - 1;
		maxZ = z + sizeX - 1;
	}

	clearTreeArea(level, minX, maxX, minZ, maxZ, y, 6);
	this->fillFoundation(level, minX, maxX, minZ, maxZ, y, mat);

	for (int32_t lx = 0; lx < sizeX; ++lx) {
		for (int32_t lz = 0; lz < sizeZ; ++lz) {
			bool isBorder = (lx == 0 || lx == sizeX - 1 || lz == 0 || lz == sizeZ - 1);
			if (isBorder) {
				this->placeRotated(level, x, y, z, lx, 0, lz, sizeX, sizeZ, mat.woodId, mat.woodMeta, rotation);
				this->placeRotated(level, x, y, z, lx, 1, lz, sizeX, sizeZ, 0, 0, rotation);
			} else if (lx == 3) {
				this->placeRotated(level, x, y, z, lx, 0, lz, sizeX, sizeZ, Tile::calmWater->blockID, 0, rotation);
				this->placeRotated(level, x, y, z, lx, 1, lz, sizeX, sizeZ, 0, 0, rotation);
			} else {
				this->placeRotated(level, x, y, z, lx, 0, lz, sizeX, sizeZ, Tile::farmland ? Tile::farmland->blockID : Tile::dirt->blockID, 7, rotation);
				int32_t cropType = random->genrand_int32() % 3;
				int32_t cropTile = Tile::crops ? Tile::crops->blockID : 0;
				if (cropType == 1 && Tile::carrots) cropTile = Tile::carrots->blockID;
				if (cropType == 2 && Tile::potatoes) cropTile = Tile::potatoes->blockID;

				int32_t cropAge = 3 + (random->genrand_int32() % 5);
				this->placeRotated(level, x, y, z, lx, 1, lz, sizeX, sizeZ, cropTile, cropAge, rotation);
			}
		}
	}

	if (!level->isClientMaybe) {
		Villager* v = new Villager(level);
		int32_t vrx = 3, vrz = 2;
		if (rotation == 1 || rotation == 3) { vrx = 2; vrz = 3; }
		v->houseX = x + vrx; v->houseY = y + 1; v->houseZ = z + vrz;
		v->profession = 0;
		v->moveTo((float)(x + vrx) + 0.5f, (float)(y + 1), (float)(z + vrz) + 0.5f, 0.0f, 0.0f);
		MobSpawner::finalizeMobSettings(v, level, 0.0, 0.0, 0.0);
		level->addEntity(v);
	}
}

void VillageFeature::placeRoad(Level* level, int32_t x1, int32_t z1, int32_t x2, int32_t z2, const VillageMaterials& mat) {
	int32_t dx = abs(x2 - x1);
	int32_t dz = abs(z2 - z1);
	int32_t sx = (x1 < x2) ? 1 : -1;
	int32_t sz = (z1 < z2) ? 1 : -1;
	int32_t err = dx - dz;

	int32_t cx = x1;
	int32_t cz = z1;

	while (true) {
		for (int32_t rx = -1; rx <= 1; ++rx) {
			for (int32_t rz = -1; rz <= 1; ++rz) {
				int32_t px = cx + rx;
				int32_t pz = cz + rz;

				int32_t groundY = 0;
				for (int32_t checkY = 110; checkY >= 45; --checkY) {
					int32_t t = level->getTile(px, checkY, pz);
					if (t == Tile::grass->blockID || t == Tile::dirt->blockID || t == Tile::sand->blockID || t == Tile::sandStone->blockID || t == Tile::stoneBrick->blockID || t == Tile::gravel->blockID || t == Tile::rock->blockID || t == Tile::topSnow->blockID) {
						groundY = checkY + 1;
						break;
					}
				}

				if (groundY >= 64) {
					int32_t top = level->getTile(px, groundY - 1, pz);
					if (top != Tile::water->blockID && top != Tile::calmWater->blockID && top != Tile::treeTrunk->blockID && top != Tile::wood->blockID && top != (Tile::farmland ? Tile::farmland->blockID : 0) && top != Tile::stoneBrick->blockID && top != Tile::door_wood->blockID && (!Tile::lightGem || top != Tile::lightGem->blockID) && top != mat.stairId && top != mat.slabId && top != mat.plankId && top != Tile::thinGlass->blockID) {
						this->placeBlock(level, px, groundY - 1, pz, mat.pathId, mat.pathMeta);
						for (int32_t cy = groundY; cy <= groundY + 3; ++cy) {
							if (cy < 128) {
								int32_t ct = level->getTile(px, cy, pz);
								if (ct != 0 && ct != Tile::fence->blockID && ct != Tile::door_wood->blockID && ct != mat.woodId && ct != mat.stoneId && ct != mat.plankId && ct != mat.stairId && ct != mat.slabId && ct != Tile::thinGlass->blockID && (!Tile::lightGem || ct != Tile::lightGem->blockID)) {
									level->setTileAndDataNoUpdate(px, cy, pz, 0, 0);
								}
							}
						}
					}
				}
			}
		}

		if (cx == x2 && cz == z2) break;
		int32_t e2 = 2 * err;
		if (e2 > -dz) {
			err -= dz;
			cx += sx;
		}
		if (e2 < dx) {
			err += dx;
			cz += sz;
		}
	}
}

void VillageFeature::placeLamp(Level* level, int32_t x, int32_t y, int32_t z, const VillageMaterials& mat) {
	this->placeBlock(level, x, y, z, mat.fenceId, 0);
	this->placeBlock(level, x, y + 1, z, mat.fenceId, 0);
	this->placeBlock(level, x, y + 2, z, mat.fenceId, 0);
	this->placeBlock(level, x, y + 3, z, Tile::lightGem ? Tile::lightGem->blockID : mat.woodId, 0);
}

bool_t VillageFeature::place(Level* level, Random* random, int32_t x, int32_t y, int32_t z) {
	if (y < 64 || y > 90) return 0;

	int32_t ground = level->getTile(x, y - 1, z);
	if (ground == Tile::water->blockID || ground == Tile::calmWater->blockID) return 0;

	Biome* biome = level->getBiome(x, z);
	VillageType type = getVillageType(biome);
	VillageMaterials mat = getMaterials(type);

	this->placeWell(level, random, x, y, z, mat);

	int32_t numBuildings = 4 + (random->genrand_int32() % 4);
	int32_t buildingOffsets[8][2] = {
		{14, 0},
		{-16, 0},
		{0, 14},
		{0, -16},
		{14, 14},
		{-16, 14},
		{14, -16},
		{-16, -16}
	};

	int32_t placedBuildings = 0;
	for (int32_t b = 0; b < 8 && placedBuildings < numBuildings; ++b) {
		int32_t baseOffX = buildingOffsets[b][0];
		int32_t baseOffZ = buildingOffsets[b][1];

		int32_t bType = random->genrand_int32() % 4;

		int32_t doorLx = 2;
		int32_t doorLz = -1;
		int32_t sX = 5, sZ = 5;

		if (bType == 0) {
			sX = 7; sZ = 9;
			doorLx = 3; doorLz = -1;
		} else if (bType == 1) {
			sX = 8; sZ = 6;
			doorLx = 2; doorLz = -1;
		} else {
			sX = 5; sZ = 5;
			doorLx = 2; doorLz = -1;
		}

		int32_t bRot = 0;
		if (abs(baseOffX) >= abs(baseOffZ)) {
			bRot = (baseOffX > 0) ? 3 : 1;
		} else {
			bRot = (baseOffZ > 0) ? 0 : 2;
		}

		int32_t effSX = (bRot == 1 || bRot == 3) ? sZ : sX;
		int32_t effSZ = (bRot == 1 || bRot == 3) ? sX : sZ;

		int32_t bestBx = x + baseOffX;
		int32_t bestBz = z + baseOffZ;
		int32_t bestMinDiff = 999;
		int32_t bestBy = 0;
		bool foundSpot = false;

		for (int32_t scanDx = -3; scanDx <= 3; scanDx += 3) {
			for (int32_t scanDz = -3; scanDz <= 3; scanDz += 3) {
				int32_t cbx = x + baseOffX + scanDx;
				int32_t cbz = z + baseOffZ + scanDz;

				int32_t h1 = level->getHeightmap(cbx, cbz);
				int32_t h2 = level->getHeightmap(cbx + effSX - 1, cbz);
				int32_t h3 = level->getHeightmap(cbx, cbz + effSZ - 1);
				int32_t h4 = level->getHeightmap(cbx + effSX - 1, cbz + effSZ - 1);

				if (h1 < 64 || h2 < 64 || h3 < 64 || h4 < 64) continue;

				int32_t minCorner = (h1 < h2 ? h1 : h2);
				if (h3 < minCorner) minCorner = h3;
				if (h4 < minCorner) minCorner = h4;

				int32_t maxCorner = (h1 > h2 ? h1 : h2);
				if (h3 > maxCorner) maxCorner = h3;
				if (h4 > maxCorner) maxCorner = h4;

				int32_t diff = maxCorner - minCorner;
				if (diff <= 5 && abs(maxCorner - y) <= 8 && diff < bestMinDiff) {
					int32_t gTile = level->getTile(cbx + 2, maxCorner - 1, cbz + 2);
					if (gTile != Tile::water->blockID && gTile != Tile::calmWater->blockID) {
						bestMinDiff = diff;
						bestBx = cbx;
						bestBz = cbz;
						bestBy = maxCorner;
						foundSpot = true;
					}
				}
			}
		}

		if (!foundSpot) continue;

		int32_t bx = bestBx;
		int32_t bz = bestBz;
		int32_t by = bestBy;

		int32_t rx = doorLx;
		int32_t rz = doorLz;
		if (bRot == 1) { rx = (sZ - 1) - doorLz; rz = doorLx; }
		else if (bRot == 2) { rx = (sX - 1) - doorLx; rz = (sZ - 1) - doorLz; }
		else if (bRot == 3) { rx = doorLz; rz = (sX - 1) - doorLx; }

		int32_t doorWorldX = bx + rx;
		int32_t doorWorldZ = bz + rz;

		int32_t roadStartX = x + ((baseOffX > 0) ? 4 : ((baseOffX < 0) ? -1 : 1));
		int32_t roadStartZ = z + ((baseOffZ > 0) ? 4 : ((baseOffZ < 0) ? -1 : 1));
		this->placeRoad(level, roadStartX, roadStartZ, doorWorldX, doorWorldZ, mat);

		if (bType == 0) {
			this->placeFarm(level, random, bx, by, bz, bRot, mat);
		} else if (bType == 1) {
			this->placeLargeHouse(level, random, bx, by, bz, bRot, mat);
		} else {
			int32_t variant = random->genrand_int32() % 2;
			this->placeSmallHouse(level, random, bx, by, bz, bRot, variant, mat);
		}
		placedBuildings++;
	}

	int32_t lampOffsets[4][2] = {
		{-2, -2},
		{5, -2},
		{-2, 5},
		{5, 5}
	};
	for (int32_t i = 0; i < 4; ++i) {
		int32_t lx = x + lampOffsets[i][0];
		int32_t lz = z + lampOffsets[i][1];
		int32_t ly = level->getHeightmap(lx, lz);
		if (ly >= 64 && level->isEmptyTile(lx, ly, lz)) {
			this->placeLamp(level, lx, ly, lz, mat);
		}
	}

	return 1;
}
