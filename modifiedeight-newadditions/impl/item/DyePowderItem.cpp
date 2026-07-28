#include <item/DyePowderItem.hpp>
#include <entity/Player.hpp>
#include <level/Level.hpp>
#include <tile/Tile.hpp>
#include <entity/Sheep.hpp>
#include <I18n.hpp>
#include <Options.hpp>
#include <tile/ColoredSlabTile.hpp>
#include <tile/ColoredFenceTile.hpp>
#include <tile/ColoredLogTile.hpp>

std::string DyePowderItem::COLOR_DESCS[] = {"black", "red", "green", "brown", "blue", "purple", "cyan", "silver", "gray", "pink", "lime", "yellow", "lightBlue", "magenta", "orange", "white"};

DyePowderItem::DyePowderItem(int32_t a2)
	: Item(a2)
	, field_48() {
	this->setStackedByData(1);
	this->setMaxDamage(0);
	this->field_48 = *this->getTextureItem("dye_powder");
}

DyePowderItem::~DyePowderItem() {
}

TextureUVCoordinateSet* DyePowderItem::getIcon(int32_t a2, int32_t, bool_t) {
	if(a2 > 0b1111) a2 = 0b1111;
	return this->field_48.getUV(a2);
}

bool_t DyePowderItem::useOn(ItemInstance* item, Player* player, Level* level, int32_t x, int32_t y, int32_t z, int32_t face, float fx, float fy, float fz) {
	int32_t tileId = level->getTile(x, y, z);
	int32_t data = level->getData(x, y, z);

	if (Options::instance && Options::instance->newAdditions) {
		int color = ~item->getAuxValue() & 0xF;
		bool dyed = false;
		bool isGlassBlock = false;
		bool isGlassPane = false;
		bool isBrick = false;

		// PRIORITY 0: Brick Stairs & Slabs FIRST
		bool isBrickStairs = (tileId == Tile::stairs_brick->blockID);
		bool isColoredBrickStairs = false;
		for (int i = 0; i < 16; i++) {
			if (Tile::coloredBrickStairs[i] && tileId == Tile::coloredBrickStairs[i]->blockID) {
				isColoredBrickStairs = true;
				break;
			}
		}

		if ((isBrickStairs || isColoredBrickStairs) && Tile::coloredBrickStairs[color]) {
			level->setTileAndData(x, y, z, Tile::coloredBrickStairs[color]->blockID, data, 3);
			dyed = true;
		}

		bool isBrickHalfSlab = (tileId == Tile::stoneSlabHalf->blockID && (data & 7) == 4);
		bool isBrickFullSlab = (tileId == Tile::stoneSlab->blockID && (data & 7) == 4);
		bool isColoredBrickHalfSlab = (tileId == Tile::coloredBrickSlabHalf1->blockID || tileId == Tile::coloredBrickSlabHalf2->blockID);
		bool isColoredBrickFullSlab = (tileId == Tile::coloredBrickSlab1->blockID || tileId == Tile::coloredBrickSlab2->blockID);

		if (isBrickHalfSlab && !dyed) {
			int targetId = (color < 8) ? Tile::coloredBrickSlabHalf1->blockID : Tile::coloredBrickSlabHalf2->blockID;
			int targetData = (color < 8) ? (color | (data & 8)) : ((color - 8) | (data & 8));
			level->setTileAndData(x, y, z, targetId, targetData, 3);
			dyed = true;
		} else if (isBrickFullSlab && !dyed) {
			int targetId = (color < 8) ? Tile::coloredBrickSlab1->blockID : Tile::coloredBrickSlab2->blockID;
			int targetData = (color < 8) ? color : (color - 8);
			level->setTileAndData(x, y, z, targetId, targetData, 3);
			dyed = true;
		} else if (isColoredBrickHalfSlab && !dyed) {
			int targetId = (color < 8) ? Tile::coloredBrickSlabHalf1->blockID : Tile::coloredBrickSlabHalf2->blockID;
			int targetData = (color < 8) ? (color | (data & 8)) : ((color - 8) | (data & 8));
			level->setTileAndData(x, y, z, targetId, targetData, 3);
			dyed = true;
		} else if (isColoredBrickFullSlab && !dyed) {
			int targetId = (color < 8) ? Tile::coloredBrickSlab1->blockID : Tile::coloredBrickSlab2->blockID;
			int targetData = (color < 8) ? color : (color - 8);
			level->setTileAndData(x, y, z, targetId, targetData, 3);
			dyed = true;
		}

		if (!dyed) {
			// 1. Logs
			bool isLog = (tileId == Tile::treeTrunk->blockID && (data & 3) == 0);
			if (!isLog) {
				for (int i = 0; i < 16; i++) {
					if (Tile::coloredLogs[i] && tileId == Tile::coloredLogs[i]->blockID) {
						isLog = true;
						break;
					}
				}
			}
			if (isLog) {
				level->setTileAndData(x, y, z, Tile::coloredLogs[color]->blockID, data & 0xF, 3);
				dyed = true;
			}

			// 2. Stairs (wood only)
			bool isStairs = (tileId == Tile::stairs_wood->blockID);
			if (!isStairs) {
				for (int i = 0; i < 16; i++) {
					if (Tile::coloredStairs[i] && tileId == Tile::coloredStairs[i]->blockID) {
						isStairs = true;
						break;
					}
				}
			}
			if (isStairs && !dyed) {
				level->setTileAndData(x, y, z, Tile::coloredStairs[color]->blockID, data, 3);
				dyed = true;
			}

			// 3. Planks
			bool isPlank = (tileId == Tile::wood->blockID && data == 0) || (Tile::coloredPlanks && tileId == Tile::coloredPlanks->blockID);
			if (isPlank && !dyed) {
				level->setTileAndData(x, y, z, Tile::coloredPlanks->blockID, color, 3);
				dyed = true;
			}

			// 4. Fences
			bool isFence = (tileId == Tile::fence->blockID);
			if (!isFence) {
				for (int i = 0; i < 16; i++) {
					if (Tile::coloredFences[i] && tileId == Tile::coloredFences[i]->blockID) {
						isFence = true;
						break;
					}
				}
			}
			if (isFence && !dyed) {
				level->setTileAndData(x, y, z, Tile::coloredFences[color]->blockID, data, 3);
				dyed = true;
			}

			// 5. Slabs (wood only)
			bool isHalfSlab = ((tileId == Tile::woodSlabHalf->blockID && (data & 7) == 0) ||
			                   (Tile::coloredSlabHalf1 && tileId == Tile::coloredSlabHalf1->blockID) ||
			                   (Tile::coloredSlabHalf2 && tileId == Tile::coloredSlabHalf2->blockID));
			bool isFullSlab = ((tileId == Tile::woodSlab->blockID && (data & 7) == 0) ||
			                   (Tile::coloredSlab1 && tileId == Tile::coloredSlab1->blockID) ||
			                   (Tile::coloredSlab2 && tileId == Tile::coloredSlab2->blockID));
			if (isHalfSlab && !dyed) {
				int targetId = (color < 8) ? Tile::coloredSlabHalf1->blockID : Tile::coloredSlabHalf2->blockID;
				int targetData = (color < 8) ? (color | (data & 8)) : ((color - 8) | (data & 8));
				level->setTileAndData(x, y, z, targetId, targetData, 3);
				dyed = true;
			} else if (isFullSlab && !dyed) {
				int targetId = (color < 8) ? Tile::coloredSlab1->blockID : Tile::coloredSlab2->blockID;
				int targetData = (color < 8) ? color : (color - 8);
				level->setTileAndData(x, y, z, targetId, targetData, 3);
				dyed = true;
			}

			// 6. Glass & Glass Panes
			isGlassBlock = (tileId == Tile::glass->blockID || (Tile::stainedGlass && tileId == Tile::stainedGlass->blockID));
			isGlassPane = (tileId == Tile::thinGlass->blockID || (Tile::stainedGlassPane && tileId == Tile::stainedGlassPane->blockID));
			if (isGlassBlock && Tile::stainedGlass && !dyed) {
				level->setTileAndData(x, y, z, Tile::stainedGlass->blockID, color, 3);
				level->playSound((float)x + 0.5f, (float)y + 0.5f, (float)z + 0.5f, Tile::glass->soundType->field_C, 1.0f, 1.0f);
				dyed = true;
			} else if (isGlassPane && Tile::stainedGlassPane && !dyed) {
				level->setTileAndData(x, y, z, Tile::stainedGlassPane->blockID, color, 3);
				level->playSound((float)x + 0.5f, (float)y + 0.5f, (float)z + 0.5f, Tile::glass->soundType->field_C, 1.0f, 1.0f);
				dyed = true;
			}

			// 7. Bricks
			isBrick = (tileId == Tile::redBrick->blockID || (Tile::coloredBricks && tileId == Tile::coloredBricks->blockID));
			if (isBrick && Tile::coloredBricks && !dyed) {
				level->setTileAndData(x, y, z, Tile::coloredBricks->blockID, color, 3);
				dyed = true;
			}
		}

		if (dyed) {
			if (isGlassBlock || isGlassPane) {
				// sound already played
			} else if (isBrick || isBrickStairs || isColoredBrickStairs || isBrickHalfSlab || isBrickFullSlab || isColoredBrickHalfSlab || isColoredBrickFullSlab) {
				level->playSound((float)x + 0.5f, (float)y + 0.5f, (float)z + 0.5f, Tile::redBrick->soundType->field_C, 1.0f, 1.0f);
			} else {
				level->playSound((float)x + 0.5f, (float)y + 0.5f, (float)z + 0.5f, Tile::wood->soundType->field_C, 1.0f, 1.0f);
			}
			if (!player->abilities.instabuild) {
				--item->count;
			}
			return 1;
		}
	}

	if(item->getAuxValue() != 15) {
		return 0;
	}
	if(!player->abilities.instabuild) {
		--item->count;
	}
	Tile* result = Tile::tiles[level->getTile(x, y, z)];
	if(result) {
		return result->onFertilized(level, x, y, z);
	}
	return 0;
}

void DyePowderItem::interactEnemy(ItemInstance* a2, Mob* a3, Player* a4) {
	int8_t meta; // r7
	int32_t v9; // r7

	if(a3->getEntityTypeId() == 13) {
		meta = a2->getAuxValue();
		if(!((Sheep*)a3)->isSheared()) {
			v9 = ~meta & 0xF;
			if(((Sheep*)a3)->getColor() != v9) {
				((Sheep*)a3)->setColor(v9);
				if(a4) {
					if(!a4->abilities.instabuild) {
						--a2->count;
					}
				}
			}
		}
	}
}

std::string DyePowderItem::getName(const ItemInstance* a2) {
	int32_t meta = a2->getAuxValue();
	int32_t v6 = meta & ~(meta >> 31);
	int32_t v7;
	if(v6 >= 15) {
		v7 = 15;
	} else {
		v7 = v6;
	}
	return I18n::get(Item::getDescriptionId() + "." + DyePowderItem::COLOR_DESCS[v7] + ".name");
}

std::string DyePowderItem::getDescriptionId(const ItemInstance* a2) {
	int32_t meta = a2->getAuxValue();
	int32_t v6 = meta & ~(meta >> 31);
	int32_t v7;
	if(v6 >= 15) {
		v7 = 15;
	} else {
		v7 = v6;
	}
	return Item::getDescriptionId() + "." + DyePowderItem::COLOR_DESCS[v7];
}