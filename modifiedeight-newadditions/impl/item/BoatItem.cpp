#include <item/BoatItem.hpp>
#include <entity/Boat.hpp>
#include <entity/Player.hpp>
#include <level/Level.hpp>
#include <rendering/TextureAtlas.hpp>
#include <rendering/TextureAtlasTextureItem.hpp>
#include <I18n.hpp>

static const char* _boatItemIcons[] = {
	"oak_boat",
	"spruce_boat",
	"birch_boat",
	"jungle_boat"
};

BoatItem::BoatItem(int32_t id)
	: Item(id) {
	this->maxStackSize = 1;
	this->setCategory(3, 1);
	this->setDescriptionId("boat");
	this->stackedByData = 1;
}

BoatItem::~BoatItem() {
}

TextureUVCoordinateSet* BoatItem::getIcon(int32_t aux, int32_t pass, bool_t inHand) {
	if (aux < 0 || aux >= 4) aux = 0;
	if (Item::_itemTextureAtlas) {
		TextureAtlasTextureItem* texItem = Item::_itemTextureAtlas->getTextureItem(_boatItemIcons[aux]);
		if (texItem) {
			return texItem->getUV(0);
		}
	}
	return &this->iconUV;
}

bool_t BoatItem::useOn(ItemInstance* item, Player* player, Level* level, int32_t x, int32_t y, int32_t z, int32_t side, float fx, float fy, float fz) {
	if (!level) return 0;
	if (!level->isClientMaybe) {
		float spawnX = (float)x + 0.5f;
		float spawnY = (float)y + 1.0f;
		float spawnZ = (float)z + 0.5f;
		Boat* boat = new Boat(level, spawnX, spawnY, spawnZ);
		boat->setBoatType(item ? item->getAuxValue() : 0);
		if (player) {
			boat->yaw = player->yaw;
		}
		level->addEntity(boat);
		if (player && !player->abilities.instabuild && item) {
			item->count--;
		}
	}
	return 1;
}

std::string BoatItem::getName(const ItemInstance* item) {
	int32_t aux = item ? item->getAuxValue() : 0;
	switch (aux) {
		case 1: return "Spruce Boat";
		case 2: return "Birch Boat";
		case 3: return "Jungle Boat";
		default: return "Oak Boat";
	}
}
