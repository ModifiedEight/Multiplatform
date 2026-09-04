#pragma once
#include <item/Item.hpp>

struct BoatItem : Item {
	BoatItem(int32_t id);
	virtual ~BoatItem();

	virtual bool_t useOn(ItemInstance*, Player*, Level*, int32_t, int32_t, int32_t, int32_t, float, float, float);
	virtual TextureUVCoordinateSet* getIcon(int32_t, int32_t, bool_t);
	virtual std::string getName(const ItemInstance*);
};
