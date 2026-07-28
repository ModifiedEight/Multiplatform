#include <tile/ColoredBedTile.hpp>
#include <item/Item.hpp>
#include <rendering/TextureAtlasTextureItem.hpp>

ColoredBedTile::ColoredBedTile(int32_t id, int colorIdx)
	: BedTile(id) {
	this->colorIndex = colorIdx;
	
	const char* colors[] = {"white", "orange", "magenta", "light_blue", "yellow", "lime", "pink", "gray", "silver", "cyan", "purple", "blue", "brown", "green", "red", "black"};
	std::string texName = "bed_" + std::string(colors[colorIdx]);
	
	TextureAtlasTextureItem v35(*this->getTextureItem(texName));
	// Based on BedTile.cpp constructor:
	this->field_B0 = *v35.getUV(0);
	this->field_80 = *v35.getUV(1);
	this->field_98 = *v35.getUV(2);
	this->field_F8 = *v35.getUV(3);
	this->field_C8 = *v35.getUV(4);
	this->field_E0 = *v35.getUV(5);
}

ColoredBedTile::~ColoredBedTile() {
}

TextureUVCoordinateSet* ColoredBedTile::getTexture(int32_t a2, int32_t a3) {
	return BedTile::getTexture(a2, a3);
}

int32_t ColoredBedTile::getResource(int32_t a2, Random* a3) {
	if(BedTile::isHeadPiece(a2)) return 0;
	return Item::bed->itemID;
}

void ColoredBedTile::spawnResources(Level* a2, int32_t a3, int32_t a4, int32_t a5, int32_t a6, float a7){
	if(!BedTile::isHeadPiece(a6)){
		this->popResource(a2, a3, a4, a5, ItemInstance(Item::bed, 1, this->colorIndex));
	}
}

std::string ColoredBedTile::getTypeDescriptionId(int32_t data) {
	return "";
}
