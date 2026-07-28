#include <item/ColoredPlanksTileItem.hpp>
#include <tile/Tile.hpp>
#include <item/DyePowderItem.hpp>
#include <I18n.hpp>

ColoredPlanksTileItem::ColoredPlanksTileItem(int32_t id)
	: TileItem(id) {
	this->setMaxDamage(0);
	this->setStackedByData(1);
}

ColoredPlanksTileItem::~ColoredPlanksTileItem() {
}

TextureUVCoordinateSet* ColoredPlanksTileItem::getIcon(int32_t a2, int32_t, bool_t) {
	return Tile::coloredPlanks->getTexture(2, a2);
}

int32_t ColoredPlanksTileItem::getLevelDataForAuxValue(int32_t a2) {
	return a2;
}

std::string ColoredPlanksTileItem::getName(const ItemInstance* a3) {
	return I18n::get(this->getDescriptionId() + "." + DyePowderItem::COLOR_DESCS[(~a3->getAuxValue()) & 0xF] + ".name");
}

std::string ColoredPlanksTileItem::getDescription(const ItemInstance* a3) {
	return this->getDescriptionId() + "." + DyePowderItem::COLOR_DESCS[(~a3->getAuxValue()) & 0xF];
}
