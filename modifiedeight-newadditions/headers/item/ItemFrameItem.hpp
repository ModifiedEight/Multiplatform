#pragma once
#include <item/Item.hpp>

struct ItemFrameItem : Item {
	ItemFrameItem(int32_t id);
	virtual ~ItemFrameItem();
	virtual bool_t useOn(ItemInstance*, Player*, Level*, int32_t x, int32_t y, int32_t z, int32_t side, float clickX, float clickY, float clickZ);
};
