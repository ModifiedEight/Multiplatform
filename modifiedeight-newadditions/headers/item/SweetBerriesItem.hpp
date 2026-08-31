#pragma once
#include <item/FoodItem.hpp>

struct SweetBerriesItem : FoodItem {
	SweetBerriesItem(int32_t id, int32_t healAmount, float sat, int8_t meat);
	virtual ~SweetBerriesItem();
	virtual bool_t useOn(ItemInstance* item, Player* player, struct Level* level, int32_t x, int32_t y, int32_t z, int32_t face, float fx, float fy, float fz);
};
