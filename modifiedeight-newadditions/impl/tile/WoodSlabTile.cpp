#include <tile/WoodSlabTile.hpp>
#include <tile/MixedSlabTile.hpp>
#include <tile/entity/MixedSlabTileEntity.hpp>
#include <tile/material/Material.hpp>
#include <level/Level.hpp>

static bool isAnyHalfSlabTile(int32_t id) {
	return (id == Tile::stoneSlabHalf->blockID ||
	        id == Tile::woodSlabHalf->blockID ||
	        (Tile::coloredSlabHalf1 && id == Tile::coloredSlabHalf1->blockID) ||
	        (Tile::coloredSlabHalf2 && id == Tile::coloredSlabHalf2->blockID) ||
	        (Tile::coloredBrickSlabHalf1 && id == Tile::coloredBrickSlabHalf1->blockID) ||
	        (Tile::coloredBrickSlabHalf2 && id == Tile::coloredBrickSlabHalf2->blockID));
}

WoodSlabTile::Item::~Item(){

}
bool_t WoodSlabTile::Item::useOn(ItemInstance* a2, Player* a3, Level* a4, int32_t x, int32_t y, int32_t z, int32_t face, float a9, float a10, float a11) {
	if (a2->count == 0) return 0;

	int32_t id = a4->getTile(x, y, z);
	uint32_t meta = a4->getData(x, y, z);

	if (id == Tile::mixedSlab->blockID) {
		MixedSlabTileEntity* te = (MixedSlabTileEntity*)a4->getTileEntity(x, y, z);
		if (te) {
			bool combined = false;
			if (te->mode == 0) {
				if (te->bottomTileId == 0 && te->topTileId != 0) {
					if (face == 0 || (face >= 2 && a10 <= 0.5f)) {
						te->bottomTileId = Tile::woodSlabHalf->blockID;
						te->bottomAux = a2->getAuxValue() & 7;
						combined = true;
					}
				} else if (te->topTileId == 0 && te->bottomTileId != 0) {
					if (face == 1 || (face >= 2 && a10 > 0.5f)) {
						te->topTileId = Tile::woodSlabHalf->blockID;
						te->topAux = a2->getAuxValue() & 7;
						combined = true;
					}
				}
			} else if (te->mode == 1) {
				if (te->bottomTileId == 0 && te->topTileId != 0) {
					if (face == 2 || (face != 3 && a11 <= 0.5f)) {
						te->bottomTileId = Tile::woodSlabHalf->blockID;
						te->bottomAux = a2->getAuxValue() & 7;
						combined = true;
					}
				} else if (te->topTileId == 0 && te->bottomTileId != 0) {
					if (face == 3 || (face != 2 && a11 > 0.5f)) {
						te->topTileId = Tile::woodSlabHalf->blockID;
						te->topAux = a2->getAuxValue() & 7;
						combined = true;
					}
				}
			} else if (te->mode == 2) {
				if (te->bottomTileId == 0 && te->topTileId != 0) {
					if (face == 4 || (face != 5 && a9 <= 0.5f)) {
						te->bottomTileId = Tile::woodSlabHalf->blockID;
						te->bottomAux = a2->getAuxValue() & 7;
						combined = true;
					}
				} else if (te->topTileId == 0 && te->bottomTileId != 0) {
					if (face == 5 || (face != 4 && a9 > 0.5f)) {
						te->topTileId = Tile::woodSlabHalf->blockID;
						te->topAux = a2->getAuxValue() & 7;
						combined = true;
					}
				}
			}
			if (combined) {
				a4->sendTileUpdated(x, y, z);
				a4->playSound((float)x + 0.5f, (float)y + 0.5f, (float)z + 0.5f, Tile::woodSlab->soundType->field_C, (float)(Tile::woodSlab->soundType->field_0 + 1.0f) * 0.5f, Tile::woodSlab->soundType->field_4 * 0.8f);
				--a2->count;
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
			tTileId = Tile::woodSlabHalf->blockID; tAux = a2->getAuxValue() & 7;
		} else if (face == 0 && isUpper) {
			canCombine = 1;
			bTileId = Tile::woodSlabHalf->blockID; bAux = a2->getAuxValue() & 7;
			tTileId = id; tAux = meta & 7;
		} else if (face >= 2) {
			if (a10 > 0.5f && !isUpper) {
				canCombine = 1;
				bTileId = id; bAux = meta & 7;
				tTileId = Tile::woodSlabHalf->blockID; tAux = a2->getAuxValue() & 7;
			} else if (a10 <= 0.5f && isUpper) {
				canCombine = 1;
				bTileId = Tile::woodSlabHalf->blockID; bAux = a2->getAuxValue() & 7;
				tTileId = id; tAux = meta & 7;
			}
		}

		if (canCombine) {
			AABB fullBox = {(float)x, (float)y, (float)z, (float)x + 1.0f, (float)y + 1.0f, (float)z + 1.0f};
			if (a4->isUnobstructed(fullBox)) {
				if (a4->setTileAndData(x, y, z, Tile::mixedSlab->blockID, 0, 3)) {
					MixedSlabTileEntity* te = (MixedSlabTileEntity*)a4->getTileEntity(x, y, z);
					if (te) {
						te->mode = 0;
						te->bottomTileId = bTileId;
						te->bottomAux = bAux;
						te->topTileId = tTileId;
						te->topAux = tAux;
					}
					a4->sendTileUpdated(x, y, z);
					a4->playSound((float)x + 0.5f, (float)y + 0.5f, (float)z + 0.5f, Tile::woodSlab->soundType->field_C, (float)(Tile::woodSlab->soundType->field_0 + 1.0f) * 0.5f, Tile::woodSlab->soundType->field_4 * 0.8f);
					--a2->count;
				}
			}
			return 1;
		}
	}

	if (face >= 2) {
		bool isMiddle = false;
		if (face == 2 || face == 3) {
			if (a9 >= 0.25f && a9 <= 0.75f) isMiddle = true;
		} else {
			if (a11 >= 0.25f && a11 <= 0.75f) isMiddle = true;
		}

		if (!isMiddle) {
			int32_t targetX = x;
			int32_t targetY = y;
			int32_t targetZ = z;
			if (face == 2) --targetZ;
			else if (face == 3) ++targetZ;
			else if (face == 4) --targetX;
			else if (face == 5) ++targetX;

			if (a4->mayPlace(Tile::mixedSlab->blockID, targetX, targetY, targetZ, 0, face)) {
				int32_t mode = 0;
				int32_t bTileId = 0, bAux = 0;
				int32_t tTileId = 0, tAux = 0;

				if (face == 2 || face == 3) {
					mode = 2;
					if (a9 < 0.5f) {
						bTileId = Tile::woodSlabHalf->blockID;
						bAux = a2->getAuxValue() & 7;
					} else {
						tTileId = Tile::woodSlabHalf->blockID;
						tAux = a2->getAuxValue() & 7;
					}
				} else {
					mode = 1;
					if (a11 < 0.5f) {
						bTileId = Tile::woodSlabHalf->blockID;
						bAux = a2->getAuxValue() & 7;
					} else {
						tTileId = Tile::woodSlabHalf->blockID;
						tAux = a2->getAuxValue() & 7;
					}
				}

				if (a4->setTileAndData(targetX, targetY, targetZ, Tile::mixedSlab->blockID, 0, 3)) {
					MixedSlabTileEntity* te = (MixedSlabTileEntity*)a4->getTileEntity(targetX, targetY, targetZ);
					if (te) {
						te->mode = mode;
						te->bottomTileId = bTileId;
						te->bottomAux = bAux;
						te->topTileId = tTileId;
						te->topAux = tAux;
					}
					a4->sendTileUpdated(targetX, targetY, targetZ);
					a4->playSound((float)targetX + 0.5f, (float)targetY + 0.5f, (float)targetZ + 0.5f, Tile::woodSlab->soundType->field_C, (float)(Tile::woodSlab->soundType->field_0 + 1.0f) * 0.5f, Tile::woodSlab->soundType->field_4 * 0.8f);
					--a2->count;
					return 1;
				}
			}
		}
	}

	return TileItem::useOn(a2, a3, a4, x, y, z, face, a9, a10, a11);
}

WoodSlabTile::WoodSlabTile(int32_t a2, bool_t a3)
	: SlabTile(a2, "planks", a3, Material::wood) {
	this->field_84 = *this->getTextureItem("planks");
}

WoodSlabTile::~WoodSlabTile() {
}
TextureUVCoordinateSet* WoodSlabTile::getTexture(int32_t a2, int32_t a3) {
	return this->field_84.getUV(a3 & 7);
}
int32_t WoodSlabTile::getResource(int32_t, Random*) {
	return Tile::woodSlabHalf->blockID;
}
std::string WoodSlabTile::getTypeDescriptionId(int32_t a2) {
	return Tile::WOOD_NAMES[a2 & 7];
}
int32_t WoodSlabTile::getSpawnResourcesAuxValue(int32_t a2) {
	return a2 & 7;
}
