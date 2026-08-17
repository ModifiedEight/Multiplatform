#pragma once
#include <item/TileItem.hpp>

class WaterLilyTileItem : public TileItem {
public:
	WaterLilyTileItem(int32_t id);
	virtual ~WaterLilyTileItem();
	virtual bool_t isLiquidClipItem(int32_t);
	virtual bool_t useOn(ItemInstance*, Player*, Level*, int32_t, int32_t, int32_t, int32_t, float, float, float);
};
