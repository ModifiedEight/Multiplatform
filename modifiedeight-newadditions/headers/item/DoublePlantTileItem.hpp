#pragma once
#include <item/TileItem.hpp>

struct DoublePlantTileItem : TileItem {
	DoublePlantTileItem(int32_t id);
	virtual ~DoublePlantTileItem();

	virtual TextureUVCoordinateSet* getIcon(int32_t aux, int32_t unk, bool_t inHand);
	virtual int32_t getLevelDataForAuxValue(int32_t aux);
	virtual bool_t useOn(ItemInstance* item, Player* player, Level* level, int32_t x, int32_t y, int32_t z, int32_t side, float fx, float fy, float fz);
	virtual std::string getName(const ItemInstance* item);
	virtual std::string getDescriptionId(const ItemInstance* item);
};
