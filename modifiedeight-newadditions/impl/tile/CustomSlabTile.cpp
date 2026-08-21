#include <tile/CustomSlabTile.hpp>
#include <tile/MixedSlabTile.hpp>
#include <tile/entity/MixedSlabTileEntity.hpp>
#include <tile/material/Material.hpp>
#include <level/Level.hpp>
#include <entity/Player.hpp>
#include <tile/BlockColorRegistry.hpp>

static bool isAnyHalfSlabTile(int32_t id) {
	return (id == Tile::stoneSlabHalf->blockID ||
	        id == Tile::woodSlabHalf->blockID ||
	        (Tile::coloredSlabHalf1 && id == Tile::coloredSlabHalf1->blockID) ||
	        (Tile::coloredSlabHalf2 && id == Tile::coloredSlabHalf2->blockID) ||
	        (Tile::coloredBrickSlabHalf1 && id == Tile::coloredBrickSlabHalf1->blockID) ||
	        (Tile::coloredBrickSlabHalf2 && id == Tile::coloredBrickSlabHalf2->blockID) ||
	        (Tile::dirtSlabHalf && id == Tile::dirtSlabHalf->blockID) ||
	        (Tile::grassSlabHalf && id == Tile::grassSlabHalf->blockID) ||
	        (Tile::rockSlabHalf && id == Tile::rockSlabHalf->blockID));
}

CustomSlabTile::Item::Item(int32_t id, Tile* tile, int32_t slabHalfId, int32_t slabFullId)
	: AuxDataTileItem(id, tile), slabHalfId(slabHalfId), slabFullId(slabFullId) {}

CustomSlabTile::Item::~Item() {}

bool_t CustomSlabTile::Item::useOn(ItemInstance* a2, Player* a3, Level* a4, int32_t x, int32_t y, int32_t z, int32_t face, float a9, float a10, float a11) {
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
						te->bottomTileId = this->slabHalfId;
						te->bottomAux = a2->getAuxValue() & 7;
						combined = true;
					}
				} else if (te->topTileId == 0 && te->bottomTileId != 0) {
					if (face == 1 || (face >= 2 && a10 > 0.5f)) {
						te->topTileId = this->slabHalfId;
						te->topAux = a2->getAuxValue() & 7;
						combined = true;
					}
				}
			} else if (te->mode == 1) {
				if (te->bottomTileId == 0 && te->topTileId != 0) {
					if (face == 2 || (face != 3 && a11 <= 0.5f)) {
						te->bottomTileId = this->slabHalfId;
						te->bottomAux = a2->getAuxValue() & 7;
						combined = true;
					}
				} else if (te->topTileId == 0 && te->bottomTileId != 0) {
					if (face == 3 || (face != 2 && a11 > 0.5f)) {
						te->topTileId = this->slabHalfId;
						te->topAux = a2->getAuxValue() & 7;
						combined = true;
					}
				}
			} else if (te->mode == 2) {
				if (te->bottomTileId == 0 && te->topTileId != 0) {
					if (face == 4 || (face != 5 && a9 <= 0.5f)) {
						te->bottomTileId = this->slabHalfId;
						te->bottomAux = a2->getAuxValue() & 7;
						combined = true;
					}
				} else if (te->topTileId == 0 && te->bottomTileId != 0) {
					if (face == 5 || (face != 4 && a9 > 0.5f)) {
						te->topTileId = this->slabHalfId;
						te->topAux = a2->getAuxValue() & 7;
						combined = true;
					}
				}
			}
			if (combined) {
				a4->sendTileUpdated(x, y, z);
				Tile* fullTile = Tile::tiles[this->slabFullId];
				if (fullTile && fullTile->soundType) {
					a4->playSound((float)x + 0.5f, (float)y + 0.5f, (float)z + 0.5f, fullTile->soundType->field_C, (float)(fullTile->soundType->field_0 + 1.0f) * 0.5f, fullTile->soundType->field_4 * 0.8f);
				}
				if (!a3->abilities.instabuild) --a2->count;
				return 1;
			}
		}
	}

	int32_t isUpper = (meta >> 3) & 1;
	if (id == this->slabHalfId) {
		bool canCombineSame = false;
		if (face == 1 && !isUpper) canCombineSame = true;
		else if (face == 0 && isUpper) canCombineSame = true;
		else if (face >= 2 && ((a10 > 0.5f && !isUpper) || (a10 <= 0.5f && isUpper))) canCombineSame = true;

		if (canCombineSame) {
			AABB fullBox = {(float)x, (float)y, (float)z, (float)x + 1.0f, (float)y + 1.0f, (float)z + 1.0f};
			if (a4->isUnobstructed(fullBox)) {
				if (a4->setTileAndData(x, y, z, this->slabFullId, 0, 3)) {
					Tile* fullTile = Tile::tiles[this->slabFullId];
					if (fullTile && fullTile->soundType) {
						a4->playSound((float)x + 0.5f, (float)y + 0.5f, (float)z + 0.5f, fullTile->soundType->field_C, (float)(fullTile->soundType->field_0 + 1.0f) * 0.5f, fullTile->soundType->field_4 * 0.8f);
					}
					if (!a3->abilities.instabuild) --a2->count;
					return 1;
				}
			}
		}
	} else if (isAnyHalfSlabTile(id)) {
		bool_t canCombine = 0;
		int32_t bTileId = 0, bAux = 0;
		int32_t tTileId = 0, tAux = 0;

		if (face == 1 && !isUpper) {
			canCombine = 1;
			bTileId = id; bAux = meta & 7;
			tTileId = this->slabHalfId; tAux = a2->getAuxValue() & 7;
		} else if (face == 0 && isUpper) {
			canCombine = 1;
			bTileId = this->slabHalfId; bAux = a2->getAuxValue() & 7;
			tTileId = id; tAux = meta & 7;
		} else if (face >= 2) {
			if (a10 > 0.5f && !isUpper) {
				canCombine = 1;
				bTileId = id; bAux = meta & 7;
				tTileId = this->slabHalfId; tAux = a2->getAuxValue() & 7;
			} else if (a10 <= 0.5f && isUpper) {
				canCombine = 1;
				bTileId = this->slabHalfId; bAux = a2->getAuxValue() & 7;
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
					Tile* fullTile = Tile::tiles[this->slabFullId];
					if (fullTile && fullTile->soundType) {
						a4->playSound((float)x + 0.5f, (float)y + 0.5f, (float)z + 0.5f, fullTile->soundType->field_C, (float)(fullTile->soundType->field_0 + 1.0f) * 0.5f, fullTile->soundType->field_4 * 0.8f);
					}
					if (!a3->abilities.instabuild) --a2->count;
					return 1;
				}
			}
		}
	}

	if (face >= 2) {
		int32_t targetX = x;
		int32_t targetY = y;
		int32_t targetZ = z;
		if (face == 2) targetZ--;
		if (face == 3) targetZ++;
		if (face == 4) targetX--;
		if (face == 5) targetX++;

		int32_t existingTile = a4->getTile(targetX, targetY, targetZ);
		if (existingTile == 0 || Tile::tiles[existingTile]->replaceable) {
			if (a4->mayPlace(this->slabHalfId, targetX, targetY, targetZ, 0, face)) {
				int32_t slabData = 0;
				if (a10 > 0.5f) slabData = 8;
				if (a4->setTileAndData(targetX, targetY, targetZ, this->slabHalfId, slabData, 3)) {
					Tile* halfTile = Tile::tiles[this->slabHalfId];
					if (halfTile && halfTile->soundType) {
						a4->playSound((float)targetX + 0.5f, (float)targetY + 0.5f, (float)targetZ + 0.5f, halfTile->soundType->field_C, (float)(halfTile->soundType->field_0 + 1.0f) * 0.5f, halfTile->soundType->field_4 * 0.8f);
					}
					if (!a3->abilities.instabuild) --a2->count;
					return 1;
				}
			}
		}
	}

	return AuxDataTileItem::useOn(a2, a3, a4, x, y, z, face, a9, a10, a11);
}

static const Material* getSlabMaterial(int32_t slabType) {
	if (slabType == 0 || slabType == 1) return Material::dirt;
	return Material::stone;
}

CustomSlabTile::CustomSlabTile(int32_t id, bool_t isFull, int32_t slabType)
	: SlabTile(id, (slabType == 0) ? "dirt" : ((slabType == 1) ? "grass" : "stone"), isFull, getSlabMaterial(slabType)),
	  slabType(slabType), partnerSlabId(0) {
	if (slabType == 0) {
		this->setSoundType(Tile::SOUND_GRAVEL);
		this->setDestroyTime(0.5f);
	} else if (slabType == 1) {
		this->setSoundType(Tile::SOUND_GRASS);
		this->setDestroyTime(0.6f);
	} else {
		this->setSoundType(Tile::SOUND_STONE);
		this->setDestroyTime(1.5f);
		this->setExplodeable(10.0f);
	}
}

CustomSlabTile::~CustomSlabTile() {}

TextureUVCoordinateSet* CustomSlabTile::getTexture(int32_t face, int32_t data) {
	if (this->slabType == 0) {
		return Tile::dirt ? Tile::dirt->getTexture(face, 0) : &this->textureUV;
	} else if (this->slabType == 1) {
		if (Tile::grass) {
			if (face == 1) return Tile::grass->getTexture(1, 0);
			if (face == 0) return Tile::dirt ? Tile::dirt->getTexture(0, 0) : &this->textureUV;
			return Tile::grass->getTexture(2, 0);
		}
		return &this->textureUV;
	} else {
		return Tile::rock ? Tile::rock->getTexture(face, 0) : &this->textureUV;
	}
}

TextureUVCoordinateSet* CustomSlabTile::getTexture(LevelSource* level, int32_t x, int32_t y, int32_t z, int32_t face) {
	return this->getTexture(face, level ? level->getData(x, y, z) : 0);
}

int32_t CustomSlabTile::getColor(LevelSource* level, int32_t x, int32_t y, int32_t z) {
	if (this->slabType == 1 && Tile::grass) {
		return Tile::grass->getColor(level, x, y, z);
	}
	return 0xFFFFFF;
}

int32_t CustomSlabTile::getColor(int32_t data) {
	if (this->slabType == 1 && Tile::grass) {
		return Tile::grass->getColor(0);
	}
	return 0xFFFFFF;
}

int32_t CustomSlabTile::getResource(int32_t data, Random* rand) {
	if (this->slabType == 0) {
		return Tile::dirtSlabHalf ? Tile::dirtSlabHalf->blockID : this->blockID;
	} else if (this->slabType == 1) {
		return Tile::dirtSlabHalf ? Tile::dirtSlabHalf->blockID : this->blockID;
	} else {
		return Tile::rockSlabHalf ? Tile::rockSlabHalf->blockID : this->blockID;
	}
}

int32_t CustomSlabTile::getSpawnResourcesAuxValue(int32_t data) {
	return 0;
}
