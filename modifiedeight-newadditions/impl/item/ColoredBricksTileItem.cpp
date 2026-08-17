#include <item/ColoredBricksTileItem.hpp>
#include <tile/Tile.hpp>
#include <item/DyePowderItem.hpp>
#include <I18n.hpp>

ColoredBricksTileItem::ColoredBricksTileItem(int32_t id)
	: TileItem(id) {
	this->setMaxDamage(0);
	this->setStackedByData(1);
}

ColoredBricksTileItem::~ColoredBricksTileItem() {
}

TextureUVCoordinateSet* ColoredBricksTileItem::getIcon(int32_t a2, int32_t, bool_t) {
	return Tile::coloredBricks->getTexture(2, a2);
}

int32_t ColoredBricksTileItem::getLevelDataForAuxValue(int32_t a2) {
	return a2;
}

std::string ColoredBricksTileItem::getName(const ItemInstance* a3) {
	return I18n::get(this->getDescriptionId(a3) + ".name");
}

std::string ColoredBricksTileItem::getDescriptionId(const ItemInstance* a3) {
	return TileItem::getDescriptionId() + "." + DyePowderItem::COLOR_DESCS[(~a3->getAuxValue()) & 0xF];
}
