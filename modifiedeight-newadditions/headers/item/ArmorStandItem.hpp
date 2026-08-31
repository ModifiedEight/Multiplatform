#pragma once
#include <item/Item.hpp>

struct ArmorStandItem : Item {
	ArmorStandItem(int32_t id);
	virtual ~ArmorStandItem();
	virtual bool_t useOn(ItemInstance*, Player*, Level*, int32_t x, int32_t y, int32_t z, int32_t side, float clickX, float clickY, float clickZ);
};
