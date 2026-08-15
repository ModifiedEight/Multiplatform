#include <item/DyePowderItem.hpp>
#include <tile/BlockColorRegistry.hpp>
#include <entity/Player.hpp>
#include <level/Level.hpp>
#include <tile/Tile.hpp>
#include <entity/Sheep.hpp>
#include <I18n.hpp>
#include <Options.hpp>
#include <tile/ColoredSlabTile.hpp>
#include <tile/ColoredFenceTile.hpp>
#include <tile/ColoredLogTile.hpp>

std::string DyePowderItem::COLOR_DESCS[] = {"black", "red", "green", "brown", "blue", "purple", "cyan", "silver", "gray", "pink", "lime", "yellow", "lightBlue", "magenta", "orange", "white"};

DyePowderItem::DyePowderItem(int32_t a2)
	: Item(a2)
	, field_48() {
	this->setStackedByData(1);
	this->setMaxDamage(0);
	this->field_48 = *this->getTextureItem("dye_powder");
}

DyePowderItem::~DyePowderItem() {
}

TextureUVCoordinateSet* DyePowderItem::getIcon(int32_t a2, int32_t, bool_t) {
	if(a2 > 0b1111) a2 = 0b1111;
	return this->field_48.getUV(a2);
}

bool_t DyePowderItem::useOn(ItemInstance* item, Player* player, Level* level, int32_t x, int32_t y, int32_t z, int32_t face, float fx, float fy, float fz) {
	int32_t tileId = level->getTile(x, y, z);
	int32_t data = level->getData(x, y, z);

	if (item && item->getAuxValue() == 15) {
		Tile* result = Tile::tiles[tileId];
		if (result && result->onFertilized(level, x, y, z)) {
			if (player && !player->abilities.instabuild) {
				--item->count;
			}
			return 1;
		}
	}

	if (tileId > 0 && tileId < 256) {
		int colorIdx = item->getAuxValue() & 0xF;
		static const uint32_t DYE_COLORS[16] = {
			0x303030,
			0xFF2222,
			0x33CC33,
			0x8B5A2B,
			0x2244FF,
			0x9922FF,
			0x00DDF2,
			0xC0C0C0,
			0x606060,
			0xFF77BB,
			0x66FF33,
			0xFFFF22,
			0x44AAFF,
			0xFF33CC,
			0xFF7700,
			0xFFFFFF
		};

		uint32_t colorHex = DYE_COLORS[colorIdx];
		BlockColorRegistry::setBlockColor(x, y, z, colorHex);

		Tile* tile = Tile::tiles[tileId];
		if (tile && tile->soundType) {
			level->playSound((float)x + 0.5f, (float)y + 0.5f, (float)z + 0.5f, tile->soundType->field_C, 1.0f, 1.0f);
		} else {
			level->playSound((float)x + 0.5f, (float)y + 0.5f, (float)z + 0.5f, "step.stone", 1.0f, 1.0f);
		}

		level->sendTileUpdated(x, y, z);

		if (player && !player->abilities.instabuild) {
			--item->count;
		}
		return 1;
	}

	return 0;
}

void DyePowderItem::interactEnemy(ItemInstance* a2, Mob* a3, Player* a4) {
	int8_t meta; // r7
	int32_t v9; // r7

	if(a3->getEntityTypeId() == 13) {
		meta = a2->getAuxValue();
		if(!((Sheep*)a3)->isSheared()) {
			v9 = ~meta & 0xF;
			if(((Sheep*)a3)->getColor() != v9) {
				((Sheep*)a3)->setColor(v9);
				if(a4) {
					if(!a4->abilities.instabuild) {
						--a2->count;
					}
				}
			}
		}
	}
}

std::string DyePowderItem::getName(const ItemInstance* a2) {
	int32_t meta = a2->getAuxValue();
	int32_t v6 = meta & ~(meta >> 31);
	int32_t v7;
	if(v6 >= 15) {
		v7 = 15;
	} else {
		v7 = v6;
	}
	return I18n::get(Item::getDescriptionId() + "." + DyePowderItem::COLOR_DESCS[v7] + ".name");
}

std::string DyePowderItem::getDescriptionId(const ItemInstance* a2) {
	int32_t meta = a2->getAuxValue();
	int32_t v6 = meta & ~(meta >> 31);
	int32_t v7;
	if(v6 >= 15) {
		v7 = 15;
	} else {
		v7 = v6;
	}
	return Item::getDescriptionId() + "." + DyePowderItem::COLOR_DESCS[v7];
}