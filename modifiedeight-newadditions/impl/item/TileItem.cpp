#include <item/TileItem.hpp>
#include <tile/Tile.hpp>
#include <level/Level.hpp>
#include <tile/material/Material.hpp>
#include <entity/Player.hpp>
#include <tile/entity/MixedSlabTileEntity.hpp>
#include <tile/MobHeadTile.hpp>
#include <tile/entity/MobHeadTileEntity.hpp>
#include <math/Mth.hpp>

TileItem::TileItem(int32_t id)
	: Item(id) {
	this->blockID = id + 256;
	this->setIcon(*Tile::tiles[this->blockID]->getTexture(2));
}

TileItem::~TileItem() {
}
bool_t TileItem::useOn(ItemInstance* item, Player* player, Level* level, int32_t x, int32_t y, int32_t z, int32_t side, float faceX, float faceY, float faceZ) {
	if (Tile::slimeBlock && this->blockID == Tile::slimeBlock->blockID && item && item->count > 0) {
		int32_t targetTile = level->getTile(x, y, z);
		if (targetTile == Tile::mixedSlab->blockID) {
			MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(x, y, z);
			if (te) {
				bool combined = false;
				if (te->mode == 0) {
					if (te->bottomTileId == 0 && te->topTileId != 0) {
						if (side == 0 || (side >= 2 && faceY <= 0.5f)) {
							te->bottomTileId = this->blockID;
							te->bottomAux = 0;
							combined = true;
						}
					} else if (te->topTileId == 0 && te->bottomTileId != 0) {
						if (side == 1 || (side >= 2 && faceY > 0.5f)) {
							te->topTileId = this->blockID;
							te->topAux = 0;
							combined = true;
						}
					}
				} else if (te->mode == 1) {
					if (te->bottomTileId == 0 && te->topTileId != 0) {
						if (side == 2 || (side != 3 && faceZ <= 0.5f)) {
							te->bottomTileId = this->blockID;
							te->bottomAux = 0;
							combined = true;
						}
					} else if (te->topTileId == 0 && te->bottomTileId != 0) {
						if (side == 3 || (side != 2 && faceZ > 0.5f)) {
							te->topTileId = this->blockID;
							te->topAux = 0;
							combined = true;
						}
					}
				} else if (te->mode == 2) {
					if (te->bottomTileId == 0 && te->topTileId != 0) {
						if (side == 4 || (side != 5 && faceX <= 0.5f)) {
							te->bottomTileId = this->blockID;
							te->bottomAux = 0;
							combined = true;
						}
					} else if (te->topTileId == 0 && te->bottomTileId != 0) {
						if (side == 5 || (side != 4 && faceX > 0.5f)) {
							te->topTileId = this->blockID;
							te->topAux = 0;
							combined = true;
						}
					}
				}
				if (combined) {
					level->sendTileUpdated(x, y, z);
					Tile* fullTile = Tile::tiles[this->blockID];
					if (fullTile && fullTile->soundType) {
						level->playSound((float)x + 0.5f, (float)y + 0.5f, (float)z + 0.5f, fullTile->soundType->field_C, (float)(fullTile->soundType->field_0 + 1.0f) * 0.5f, fullTile->soundType->field_4 * 0.8f);
					}
					if (!player || !player->abilities.instabuild) --item->count;
					return 1;
				}
			}
		}

		if (side >= 2) {
			bool isMiddle = false;
			if (side == 2 || side == 3) {
				if (faceX >= 0.25f && faceX <= 0.75f) isMiddle = true;
			} else {
				if (faceZ >= 0.25f && faceZ <= 0.75f) isMiddle = true;
			}

			if (!isMiddle) {
				int32_t targetX = x;
				int32_t targetY = y;
				int32_t targetZ = z;
				if (side == 2) --targetZ;
				else if (side == 3) ++targetZ;
				else if (side == 4) --targetX;
				else if (side == 5) ++targetX;

				if (level->mayPlace(Tile::mixedSlab->blockID, targetX, targetY, targetZ, 0, side)) {
					int32_t mode = 0;
					int32_t bTileId = 0, tTileId = 0;

					if (side == 2 || side == 3) {
						mode = 2;
						if (faceX < 0.5f) {
							bTileId = this->blockID;
						} else {
							tTileId = this->blockID;
						}
					} else {
						mode = 1;
						if (faceZ < 0.5f) {
							bTileId = this->blockID;
						} else {
							tTileId = this->blockID;
						}
					}

					if (level->setTileAndData(targetX, targetY, targetZ, Tile::mixedSlab->blockID, 0, 3)) {
						MixedSlabTileEntity* te = (MixedSlabTileEntity*)level->getTileEntity(targetX, targetY, targetZ);
						if (te) {
							te->mode = mode;
							te->bottomTileId = bTileId;
							te->bottomAux = 0;
							te->topTileId = tTileId;
							te->topAux = 0;
						}
						level->sendTileUpdated(targetX, targetY, targetZ);
						Tile* fullTile = Tile::tiles[this->blockID];
						if (fullTile && fullTile->soundType) {
							level->playSound((float)targetX + 0.5f, (float)targetY + 0.5f, (float)targetZ + 0.5f, fullTile->soundType->field_C, (float)(fullTile->soundType->field_0 + 1.0f) * 0.5f, fullTile->soundType->field_4 * 0.8f);
						}
						if (!player || !player->abilities.instabuild) --item->count;
						return 1;
					}
				}
			}
		}
	}
	int32_t yNew;	  // r5
	int32_t zNew;	  // r6
	int32_t sideNew;  // r9
	int32_t blockID;  // r2
	Tile* v19;		  // r3
	Tile* v20;		  // r11
	int32_t auxValue; // r0
	int32_t meta;	  // r0
	int32_t v23;	  // r0

	yNew = y;
	zNew = z;
	sideNew = side;
	if(level->adventureSettings.allowInteract) {
		blockID = this->blockID;
		if(blockID != Tile::leaves->blockID && Tile::tiles[blockID]->material != Material::plant) {
			return 0;
		}
	}
	int32_t origX = x, origY = y, origZ = z;
	v19 = Tile::tiles[level->getTile(x, y, z)];
	if(v19 && v19->replaceable) {
		sideNew = 1;
	} else {
		switch(side) {
			case 0:
				yNew = y - 1;
				break;
			case 1:
				yNew = y + 1;
				break;
			case 2:
				zNew = z - 1;
				break;
			case 3:
				zNew = z + 1;
				break;
			case 4:
				--x;
				break;
			case 5:
				++x;
				break;
			default:
				break;
		}
	}

	bool_t canPlace = level->mayPlace(this->blockID, x, yNew, zNew, 0, sideNew);
	if (Tile::seagrass && this->blockID == Tile::seagrass->blockID) {
		canPlace = Tile::seagrass->mayPlace(level, x, yNew, zNew);
	}
	if (!canPlace && (this->blockID == Tile::torch->blockID || this->blockID == Tile::lever->blockID) && side == 1) {
		if (level->isTopSolidBlocking(origX, origY, origZ)) {
			canPlace = true;
		}
	}

	if(!item->count || !canPlace) {
		return 0;
	}

	v20 = Tile::tiles[this->blockID];
	auxValue = item->getAuxValue();
	meta = this->getLevelDataForAuxValue(auxValue);
	v23 = v20->getPlacementDataValue(level, x, yNew, zNew, sideNew, faceX, faceY, faceZ, (Mob*)player, meta);
	if(level->setTileAndData(x, yNew, zNew, this->blockID, v23, 3)) {
		if (MobHeadTile::isHeadBlock(this->blockID)) {
			MobHeadTileEntity* te = (MobHeadTileEntity*)level->getTileEntity(x, yNew, zNew);
			if (!te) {
				te = new MobHeadTileEntity(MobHeadTile::getHeadType(this->blockID), 0);
				level->setTileEntity(x, yNew, zNew, te);
			}
			if (te) {
				te->headType = MobHeadTile::getHeadType(this->blockID);
				if (player) {
					te->rotation = (Mth::floor(((player->yaw + 180.0f) * 16.0f / 360.0f) + 0.5f)) & 15;
				}
			}
		}
		level->playSound((float)x + 0.5, (float)yNew + 0.5, (float)zNew + 0.5, v20->soundType->field_C, (float)(v20->soundType->field_0 + 1.0) * 0.5, v20->soundType->field_4 * 0.8);
		--item->count;
	}
	return 1;
}
TextureUVCoordinateSet* TileItem::getIcon(int32_t data, int32_t a3, bool_t inHand) {
	if (this->blockID >= 0 && this->blockID < 256 && Tile::tiles[this->blockID]) {
		TextureUVCoordinateSet* tex = Tile::tiles[this->blockID]->getCarriedTexture(2, data);
		if (!tex || (tex->minX == 0.0f && tex->maxX == 0.0f && tex->minY == 0.0f && tex->maxY == 0.0f)) {
			tex = Tile::tiles[this->blockID]->getTexture(2, data);
		}
		if (tex && (tex->minX != 0.0f || tex->maxX != 0.0f || tex->minY != 0.0f || tex->maxY != 0.0f)) {
			return tex;
		}
	}
	return Item::getIcon(data, a3, inHand);
}
std::string TileItem::getDescriptionId() {
	return Tile::tiles[this->blockID]->getDescriptionId();
}
std::string TileItem::getDescriptionId(const ItemInstance* a3){
	return Tile::tiles[this->blockID]->getDescriptionId(a3);
}
bool_t TileItem::isEmissive(int32_t a2) {
	int32_t blockID; // r2

	blockID = this->blockID;
	return blockID == Tile::torch->blockID || blockID == Tile::glowingObsidian->blockID || blockID == Tile::lightGem->blockID;
}
