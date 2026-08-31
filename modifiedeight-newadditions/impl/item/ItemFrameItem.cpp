#include <item/ItemFrameItem.hpp>
#include <entity/ItemFrame.hpp>
#include <level/Level.hpp>
#include <entity/Player.hpp>
#include <inventory/Inventory.hpp>

ItemFrameItem::ItemFrameItem(int32_t id)
	: Item(id) {
}

ItemFrameItem::~ItemFrameItem() {
}

bool_t ItemFrameItem::useOn(ItemInstance* item, Player* player, Level* level, int32_t x, int32_t y, int32_t z, int32_t side, float clickX, float clickY, float clickZ) {
	if (side == 0 || side == 1 || !level || !player) {
		return 0;
	}

	int32_t dir = 0;
	if (side == 2) dir = 2;
	else if (side == 3) dir = 0;
	else if (side == 4) dir = 1;
	else if (side == 5) dir = 3;

	ItemFrame* frame = new ItemFrame(level, x, y, z, dir);
	if (frame->survives()) {
		level->addEntity(frame);
		if (player->inventory && player->inventory->field_20 == 0) {
			item->count--;
		}
		level->playSound(frame, "dig.wood", 1.0f, 1.0f);
		return 1;
	} else {
		delete frame;
		return 0;
	}
}
