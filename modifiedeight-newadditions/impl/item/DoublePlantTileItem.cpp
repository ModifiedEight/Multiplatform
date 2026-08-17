#include <item/DoublePlantTileItem.hpp>
#include <tile/Tile.hpp>
#include <level/Level.hpp>
#include <entity/Player.hpp>
#include <I18n.hpp>

DoublePlantTileItem::DoublePlantTileItem(int32_t id)
	: TileItem(id) {
	this->setMaxDamage(0);
	this->setStackedByData(1);
}

DoublePlantTileItem::~DoublePlantTileItem() {
}

TextureUVCoordinateSet* DoublePlantTileItem::getIcon(int32_t aux, int32_t unk, bool_t inHand) {
	if (Tile::doublePlant) {
		return Tile::doublePlant->getCarriedTexture(0, aux & 7);
	}
	return TileItem::getIcon(aux, unk, inHand);
}

int32_t DoublePlantTileItem::getLevelDataForAuxValue(int32_t aux) {
	return aux & 7;
}

bool_t DoublePlantTileItem::useOn(ItemInstance* item, Player* player, Level* level, int32_t x, int32_t y, int32_t z, int32_t side, float fx, float fy, float fz) {
	if (!item || item->count <= 0) {
		return 0;
	}

	Tile* clicked = Tile::tiles[level->getTile(x, y, z)];
	int32_t targetX = x;
	int32_t targetY = y;
	int32_t targetZ = z;

	if (clicked && clicked->replaceable) {
		targetX = x;
		targetY = y;
		targetZ = z;
	} else {
		if (side != 1) {
			return 0;
		}
		targetY = y + 1;
	}

	if (!Tile::doublePlant || !Tile::doublePlant->canSurvive(level, targetX, targetY, targetZ)) {
		return 0;
	}
	if (!level->isEmptyTile(targetX, targetY + 1, targetZ)) {
		return 0;
	}

	int32_t type = item->getAuxValue() & 7;
	if (level->setTileAndData(targetX, targetY, targetZ, Tile::doublePlant->blockID, type, 3)) {
		level->setTileAndData(targetX, targetY + 1, targetZ, Tile::doublePlant->blockID, type | 8, 3);
		const Tile::SoundType* st = Tile::doublePlant->soundType;
		if (st) {
			level->playSound((float)targetX + 0.5f, (float)targetY + 0.5f, (float)targetZ + 0.5f, st->field_C, (st->field_0 + 1.0f) * 0.5f, st->field_4 * 0.8f);
		}
		--item->count;
		return 1;
	}
	return 0;
}

std::string DoublePlantTileItem::getDescriptionId(const ItemInstance* item) {
	int32_t type = item->getAuxValue() & 7;
	switch (type) {
		case 1:
			return "tile.doublePlant.fern";
		case 2:
			return "tile.doublePlant.paeonia";
		case 3:
			return "tile.doublePlant.rose";
		case 0:
		default:
			return "tile.doublePlant.grass";
	}
}

std::string DoublePlantTileItem::getName(const ItemInstance* item) {
	return I18n::get(this->getDescriptionId(item) + ".name");
}
