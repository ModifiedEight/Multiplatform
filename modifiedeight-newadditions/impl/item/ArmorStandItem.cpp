#include <item/ArmorStandItem.hpp>
#include <entity/ArmorStand.hpp>
#include <level/Level.hpp>
#include <entity/Player.hpp>
#include <inventory/Inventory.hpp>

ArmorStandItem::ArmorStandItem(int32_t id)
	: Item(id) {
}

ArmorStandItem::~ArmorStandItem() {
}

bool_t ArmorStandItem::useOn(ItemInstance* item, Player* player, Level* level, int32_t x, int32_t y, int32_t z, int32_t side, float clickX, float clickY, float clickZ) {
	if (side != 1 || !level || !player) {
		return 0;
	}

	float placeX = (float)x + 0.5f;
	float placeY = (float)(y + 1);
	float placeZ = (float)z + 0.5f;

	ArmorStand* stand = new ArmorStand(level, placeX, placeY, placeZ);
	stand->yaw = player->yaw + 180.0f;
	stand->prevYaw = player->yaw + 180.0f;
	level->addEntity(stand);

	if (player->inventory && player->inventory->field_20 == 0) {
		item->count--;
	}
	level->playSound(stand, "dig.wood", 1.0f, 1.0f);
	return 1;
}
