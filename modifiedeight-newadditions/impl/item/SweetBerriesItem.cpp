#include <item/SweetBerriesItem.hpp>
#include <tile/Tile.hpp>
#include <level/Level.hpp>
#include <entity/Player.hpp>
#include <inventory/Inventory.hpp>

SweetBerriesItem::SweetBerriesItem(int32_t id, int32_t healAmount, float sat, int8_t meat)
	: FoodItem(id, healAmount, sat, meat) {
}

SweetBerriesItem::~SweetBerriesItem() {
}

bool_t SweetBerriesItem::useOn(ItemInstance* item, Player* player, struct Level* level, int32_t x, int32_t y, int32_t z, int32_t face, float fx, float fy, float fz) {
	if (face != 1 || !level || !Tile::sweetBerryBush) return 0;
	int32_t targetTile = level->getTile(x, y, z);
	if (targetTile != Tile::grass->blockID && targetTile != Tile::dirt->blockID && targetTile != Tile::farmland->blockID && (!Tile::grassPath || targetTile != Tile::grassPath->blockID)) {
		return 0;
	}
	if (!level->isEmptyTile(x, y + 1, z)) {
		return 0;
	}
	level->setTileAndData(x, y + 1, z, Tile::sweetBerryBush->blockID, 0, 3);
	level->playSound((float)x + 0.5f, (float)(y + 1) + 0.5f, (float)z + 0.5f, "block.sweet_berry_bush.place", 1.0f, 0.8f + (float)(rand() % 40) / 100.0f);
	if (player && player->inventory && player->inventory->field_20 == 0) {
		--item->count;
	}
	return 1;
}
