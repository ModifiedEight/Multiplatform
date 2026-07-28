#pragma once
#include <item/TileItem.hpp>

struct ColoredBricksTileItem : TileItem {
	ColoredBricksTileItem(int32_t);
	virtual ~ColoredBricksTileItem();

	virtual TextureUVCoordinateSet* getIcon(int32_t, int32_t, bool_t);
	virtual int32_t getLevelDataForAuxValue(int32_t);
	virtual std::string getName(const ItemInstance*);
	virtual std::string getDescription(const ItemInstance*);
};
