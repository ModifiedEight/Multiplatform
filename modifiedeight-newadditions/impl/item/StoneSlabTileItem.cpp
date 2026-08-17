#include <item/StoneSlabTileItem.hpp>
#include <tile/StoneSlabTile.hpp>
#include <tile/MixedSlabTile.hpp>
#include <tile/entity/MixedSlabTileEntity.hpp>
#include <item/DyePowderItem.hpp>
#include <I18n.hpp>
#include <level/Level.hpp>

static bool isAnyHalfSlabTile(int32_t id) {
	return (id == Tile::stoneSlabHalf->blockID ||
	        id == Tile::woodSlabHalf->blockID ||
	        (Tile::coloredSlabHalf1 && id == Tile::coloredSlabHalf1->blockID) ||
	        (Tile::coloredSlabHalf2 && id == Tile::coloredSlabHalf2->blockID) ||
	        (Tile::coloredBrickSlabHalf1 && id == Tile::coloredBrickSlabHalf1->blockID) ||
	        (Tile::coloredBrickSlabHalf2 && id == Tile::coloredBrickSlabHalf2->blockID));
}

StoneSlabTileItem::StoneSlabTileItem(int32_t id) : TileItem(id){
	this->setMaxDamage(0);
	this->setStackedByData(1);
}
StoneSlabTileItem::~StoneSlabTileItem() {
}
TextureUVCoordinateSet* StoneSlabTileItem::getIcon(int32_t a2, int32_t, bool_t) {
	return Tile::stoneSlabHalf->getTexture(2, a2);
}
bool_t StoneSlabTileItem::useOn(ItemInstance* item, Player* player, Level* level, int32_t x, int32_t y, int32_t z, int32_t face, float faceX, float faceY, float faceZ) {
	if (item->count == 0) return 0;

	int32_t id = level->getTile(x, y, z);
	uint32_t meta = level->getData(x, y, z);

	if (id == Tile::mixedSlab->blockID) {
		MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(x, y, z);
		if (te) {
			bool combined = false;
			if (te->mode == 0) {
				if (te->bottomTileId == 0 && te->topTileId != 0) {
					if (face == 0 || (face >= 2 && faceY <= 0.5f)) {
						te->bottomTileId = Tile::stoneSlabHalf->blockID;
						te->bottomAux = item->getAuxValue() & 7;
						combined = true;
					}
				} else if (te->topTileId == 0 && te->bottomTileId != 0) {
					if (face == 1 || (face >= 2 && faceY > 0.5f)) {
						te->topTileId = Tile::stoneSlabHalf->blockID;
						te->topAux = item->getAuxValue() & 7;
						combined = true;
					}
				}
			} else if (te->mode == 1) {
				if (te->bottomTileId == 0 && te->topTileId != 0) {
					if (face == 2 || (face != 3 && faceZ <= 0.5f)) {
						te->bottomTileId = Tile::stoneSlabHalf->blockID;
						te->bottomAux = item->getAuxValue() & 7;
						combined = true;
					}
				} else if (te->topTileId == 0 && te->bottomTileId != 0) {
					if (face == 3 || (face != 2 && faceZ > 0.5f)) {
						te->topTileId = Tile::stoneSlabHalf->blockID;
						te->topAux = item->getAuxValue() & 7;
						combined = true;
					}
				}
			} else if (te->mode == 2) {
				if (te->bottomTileId == 0 && te->topTileId != 0) {
					if (face == 4 || (face != 5 && faceX <= 0.5f)) {
						te->bottomTileId = Tile::stoneSlabHalf->blockID;
						te->bottomAux = item->getAuxValue() & 7;
						combined = true;
					}
				} else if (te->topTileId == 0 && te->bottomTileId != 0) {
					if (face == 5 || (face != 4 && faceX > 0.5f)) {
						te->topTileId = Tile::stoneSlabHalf->blockID;
						te->topAux = item->getAuxValue() & 7;
						combined = true;
					}
				}
			}
			if (combined) {
				level->sendTileUpdated(x, y, z);
				level->playSound((float)x + 0.5f, (float)y + 0.5f, (float)z + 0.5f, Tile::stoneSlab->soundType->field_C, (float)(Tile::stoneSlab->soundType->field_0 + 1.0f) * 0.5f, Tile::stoneSlab->soundType->field_4 * 0.8f);
				--item->count;
				return 1;
			}
		}
	}

	int32_t isUpper = (meta >> 3) & 1;
	if (isAnyHalfSlabTile(id)) {
		bool_t canCombine = 0;
		int32_t bTileId = 0, bAux = 0;
		int32_t tTileId = 0, tAux = 0;

		if (face == 1 && !isUpper) {
			canCombine = 1;
			bTileId = id; bAux = meta & 7;
			tTileId = Tile::stoneSlabHalf->blockID; tAux = item->getAuxValue() & 7;
		} else if (face == 0 && isUpper) {
			canCombine = 1;
			bTileId = Tile::stoneSlabHalf->blockID; bAux = item->getAuxValue() & 7;
			tTileId = id; tAux = meta & 7;
		} else if (face >= 2) {
			if (faceY > 0.5f && !isUpper) {
				canCombine = 1;
				bTileId = id; bAux = meta & 7;
				tTileId = Tile::stoneSlabHalf->blockID; tAux = item->getAuxValue() & 7;
			} else if (faceY <= 0.5f && isUpper) {
				canCombine = 1;
				bTileId = Tile::stoneSlabHalf->blockID; bAux = item->getAuxValue() & 7;
				tTileId = id; tAux = meta & 7;
			}
		}

		if (canCombine) {
			AABB fullBox = {(float)x, (float)y, (float)z, (float)x + 1.0f, (float)y + 1.0f, (float)z + 1.0f};
			if (level->isUnobstructed(fullBox)) {
				if (level->setTileAndData(x, y, z, Tile::mixedSlab->blockID, 0, 3)) {
					MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(x, y, z);
					if (te) {
						te->mode = 0;
						te->bottomTileId = bTileId;
						te->bottomAux = bAux;
						te->topTileId = tTileId;
						te->topAux = tAux;
					}
					level->sendTileUpdated(x, y, z);
					level->playSound((float)x + 0.5f, (float)y + 0.5f, (float)z + 0.5f, Tile::stoneSlab->soundType->field_C, (float)(Tile::stoneSlab->soundType->field_0 + 1.0f) * 0.5f, Tile::stoneSlab->soundType->field_4 * 0.8f);
					--item->count;
				}
			}
			return 1;
		}
	}

	if (face >= 2) {
		bool isMiddle = false;
		if (face == 2 || face == 3) {
			if (faceX >= 0.25f && faceX <= 0.75f) isMiddle = true;
		} else {
			if (faceZ >= 0.25f && faceZ <= 0.75f) isMiddle = true;
		}

		if (!isMiddle) {
			int32_t targetX = x;
			int32_t targetY = y;
			int32_t targetZ = z;
			if (face == 2) --targetZ;
			else if (face == 3) ++targetZ;
			else if (face == 4) --targetX;
			else if (face == 5) ++targetX;

			if (level->mayPlace(Tile::mixedSlab->blockID, targetX, targetY, targetZ, 0, face)) {
				int32_t mode = 0;
				int32_t bTileId = 0, bAux = 0;
				int32_t tTileId = 0, tAux = 0;

				if (face == 2 || face == 3) {
					mode = 2;
					if (faceX < 0.5f) {
						bTileId = Tile::stoneSlabHalf->blockID;
						bAux = item->getAuxValue() & 7;
					} else {
						tTileId = Tile::stoneSlabHalf->blockID;
						tAux = item->getAuxValue() & 7;
					}
				} else {
					mode = 1;
					if (faceZ < 0.5f) {
						bTileId = Tile::stoneSlabHalf->blockID;
						bAux = item->getAuxValue() & 7;
					} else {
						tTileId = Tile::stoneSlabHalf->blockID;
						tAux = item->getAuxValue() & 7;
					}
				}

				if (level->setTileAndData(targetX, targetY, targetZ, Tile::mixedSlab->blockID, 0, 3)) {
					MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(targetX, targetY, targetZ);
					if (te) {
						te->mode = mode;
						te->bottomTileId = bTileId;
						te->bottomAux = bAux;
						te->topTileId = tTileId;
						te->topAux = tAux;
					}
					level->sendTileUpdated(targetX, targetY, targetZ);
					level->playSound((float)targetX + 0.5f, (float)targetY + 0.5f, (float)targetZ + 0.5f, Tile::stoneSlab->soundType->field_C, (float)(Tile::stoneSlab->soundType->field_0 + 1.0f) * 0.5f, Tile::stoneSlab->soundType->field_4 * 0.8f);
					--item->count;
					return 1;
				}
			}
		}
	}

	return TileItem::useOn(item, player, level, x, y, z, face, faceX, faceY, faceZ);
}
int32_t StoneSlabTileItem::getLevelDataForAuxValue(int32_t a2) {
	return a2;
}
std::string StoneSlabTileItem::getName(const ItemInstance* a3){
	return I18n::get(this->getDescriptionId(a3) + ".name");
}
std::string StoneSlabTileItem::getDescriptionId(const ItemInstance* a3) {
	int32_t id = this->blockID;
	if (Tile::coloredBrickSlabHalf1 && id == Tile::coloredBrickSlabHalf1->blockID) {
		int meta = a3->getAuxValue() & 7;
		return "tile.coloredBrickSlab." + DyePowderItem::COLOR_DESCS[(~meta) & 0xF];
	}
	if (Tile::coloredBrickSlabHalf2 && id == Tile::coloredBrickSlabHalf2->blockID) {
		int meta = (a3->getAuxValue() & 7) + 8;
		return "tile.coloredBrickSlab." + DyePowderItem::COLOR_DESCS[(~meta) & 0xF];
	}
	if (Tile::coloredSlabHalf1 && id == Tile::coloredSlabHalf1->blockID) {
		int meta = a3->getAuxValue() & 7;
		return "tile.coloredSlab." + DyePowderItem::COLOR_DESCS[(~meta) & 0xF];
	}
	if (Tile::coloredSlabHalf2 && id == Tile::coloredSlabHalf2->blockID) {
		int meta = (a3->getAuxValue() & 7) + 8;
		return "tile.coloredSlab." + DyePowderItem::COLOR_DESCS[(~meta) & 0xF];
	}
	int32_t meta = a3->getAuxValue();
	int32_t v6 = meta;
	if(meta < 0) {
		v6 = 0;
	} else if(meta >= 7) {
		v6 = 0;
	}
	return TileItem::getDescriptionId() + "." + StoneSlabTile::SLAB_NAMES[v6];
}
