#include <item/WaterLilyTileItem.hpp>
#include <level/Level.hpp>
#include <tile/Tile.hpp>
#include <tile/material/Material.hpp>
#include <entity/Player.hpp>

WaterLilyTileItem::WaterLilyTileItem(int32_t id)
	: TileItem(id) {
}

WaterLilyTileItem::~WaterLilyTileItem() {
}

bool_t WaterLilyTileItem::isLiquidClipItem(int32_t a2) {
	return 1;
}

bool_t WaterLilyTileItem::useOn(ItemInstance* item, Player* player, Level* level, int32_t x, int32_t y, int32_t z, int32_t side, float fx, float fy, float fz) {
	if (!item || item->count <= 0) return 0;
	if (!Tile::waterLily) return 0;

	int32_t targetX = x;
	int32_t targetY = y;
	int32_t targetZ = z;

	int32_t curTile = level->getTile(targetX, targetY, targetZ);
	Material* curMat = level->getMaterial(targetX, targetY, targetZ);
	if (curTile == Tile::water->blockID || curTile == Tile::calmWater->blockID || (curMat && curMat == Material::water)) {
		while (targetY < 127) {
			int32_t t = level->getTile(targetX, targetY + 1, targetZ);
			Material* m = level->getMaterial(targetX, targetY + 1, targetZ);
			if (t == Tile::water->blockID || t == Tile::calmWater->blockID || (m && m == Material::water)) {
				targetY++;
			} else {
				break;
			}
		}
		targetY++;
	} else {
		if (side == 0) targetY--;
		else if (side == 1) targetY++;
		else if (side == 2) targetZ--;
		else if (side == 3) targetZ++;
		else if (side == 4) targetX--;
		else if (side == 5) targetX++;

		int32_t tAtTarget = level->getTile(targetX, targetY, targetZ);
		Material* mAtTarget = level->getMaterial(targetX, targetY, targetZ);
		if (tAtTarget == Tile::water->blockID || tAtTarget == Tile::calmWater->blockID || (mAtTarget && mAtTarget == Material::water)) {
			targetY++;
		}
	}

	int32_t targetBlock = level->getTile(targetX, targetY, targetZ);
	if (targetBlock != 0 && (!Tile::tiles[targetBlock] || !Tile::tiles[targetBlock]->replaceable)) {
		return 0;
	}

	if (!Tile::waterLily->canSurvive(level, targetX, targetY, targetZ)) {
		return 0;
	}

	if (level->setTileAndData(targetX, targetY, targetZ, Tile::waterLily->blockID, 0, 3)) {
		Tile::waterLily->onPlace(level, targetX, targetY, targetZ);
		const Tile::SoundType* st = Tile::waterLily->soundType;
		if (st) {
			level->playSound((float)targetX + 0.5f, (float)targetY + 0.5f, (float)targetZ + 0.5f, st->field_C, (st->field_0 + 1.0f) * 0.5f, st->field_4 * 0.8f);
		}
		if (player && !player->abilities.instabuild) {
			--item->count;
		}
		return 1;
	}
	return 0;
}
