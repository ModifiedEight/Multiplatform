#include <item/BedItem.hpp>
#include <tile/Tile.hpp>
#include <level/Level.hpp>
#include <entity/Player.hpp>

BedItem::BedItem(int32_t id)
	: Item(id) {
	this->setStackedByData(1);
	this->setMaxDamage(0);
}
BedItem::~BedItem() {
}
bool_t BedItem::useOn(ItemInstance* a2, Player* a3, Level* a4, int32_t a5, int32_t a6, int32_t a7, int32_t a8, float a9, float a10, float a11) {
	int32_t v12;   // r6
	float v13;	   // s15
	int32_t v14;   // r5
	int32_t v15;   // r5
	int32_t v16;   // r10
	int32_t v17;   // r9
	int32_t v18;   // r9
	int32_t v19;   // r10
	int32_t v21;   // r0
	Tile* v22;	   // [sp+10h] [bp-30h]

	if(a8 != 1) {
		return 0;
	}
	v12 = a6 + 1;
	v22 = Tile::coloredBeds[a2->getAuxValue() & 15];
	if (!v22) v22 = Tile::coloredBeds[0];
	v13 = (float)((float)(a3->yaw * 4.0) / 360.0) + 0.5;
	v14 = (int32_t)v13;
	if(v13 < (float)(int32_t)v13) {
		v14 = v14 - 1; /*lobyte(v14) = ...*/
	}
	v15 = v14 & 3;
	if(v15) {
		if(v15 == 1) {
			v17 = -1;
			v16 = 0;
		} else if(v15 == 2) {
			v17 = 0;
			v16 = -1;
		} else {
			v16 = 0;
			v17 = 1;
		}
	} else {
		v17 = 0;
		v16 = 1;
	}
	if(!a4->isEmptyTile(a5, a6 + 1, a7)) {
		return 0;
	}
	v18 = v17 + a5;
	v19 = v16 + a7;
	auto isSolidBedBase = [&](int x, int y, int z) {
		if (a4->isSolidBlockingTile(x, y, z)) return true;
		int id = a4->getTile(x, y, z);
		return id == Tile::stoneSlabHalf->blockID || id == Tile::woodSlabHalf->blockID;
	};
	if(!a4->isEmptyTile(v18, a6 + 1, v19) || !isSolidBedBase(a5, a6, a7) || !isSolidBedBase(v18, a6, v19)) {
		return 0;
	}
	a4->setTileAndData(a5, v12, a7, v22->blockID, v15, 3);
	v21 = a4->getTile(a5, v12, a7);
	if(v21 == v22->blockID) {
		a4->setTileAndData(v18, v12, v19, v21, v15 + 8, 3);
	}
	--a2->count;
	return 1;
}

std::string BedItem::getDescriptionId(const struct ItemInstance* item) {
	const char* colors[] = {"white", "orange", "magenta", "lightBlue", "yellow", "lime", "pink", "gray", "silver", "cyan", "purple", "blue", "brown", "green", "red", "black"};
	int color = item->getAuxValue() & 15;
	return "item.bed." + std::string(colors[color]);
}

TextureUVCoordinateSet* BedItem::getIcon(int32_t damage, int32_t a3, bool_t a4) {
	const char* colors[] = {"white", "orange", "magenta", "light_blue", "yellow", "lime", "pink", "gray", "silver", "cyan", "purple", "blue", "brown", "green", "red", "black"};
	int color = damage & 15;
	std::string texName = "bed_" + std::string(colors[color]);
	if (color == 14) {
		texName = "bed"; // Use default bed for red
	}
	TextureAtlasTextureItem* tex = this->getTextureItem(texName);
	if (tex && tex->name != "error") {
		return tex->getUV(0);
	}
	return this->getTextureItem("bed")->getUV(0);
}
