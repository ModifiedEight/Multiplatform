#include <item/SaplingTileItem.hpp>
#include <tile/Tile.hpp>
#include <I18n.hpp>

SaplingTileItem::SaplingTileItem(int32_t id)
	: TileItem(id) {
	this->setMaxDamage(0);
	this->setStackedByData(1);
}
SaplingTileItem::~SaplingTileItem() {
}
TextureUVCoordinateSet* SaplingTileItem::getIcon(int32_t a2, int32_t, bool_t) {
	return Tile::sapling->getTexture(0, a2 & 3);
}
int32_t SaplingTileItem::getLevelDataForAuxValue(int32_t a2) {
	return a2 & 3;
}
std::string SaplingTileItem::getName(const ItemInstance* a3) {
	return I18n::get(this->getDescriptionId(a3) + ".name");
}
std::string SaplingTileItem::getDescriptionId(const ItemInstance* a3) {
	int32_t meta = a3 ? a3->getAuxValue() : 0;
	int32_t v6 = meta & 3;
	return TileItem::getDescriptionId() + "." + Tile::WOOD_NAMES[v6];
}
