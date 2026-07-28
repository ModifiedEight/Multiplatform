#include <tile/ColoredSlabTile.hpp>
#include <tile/material/Material.hpp>
#include <level/Level.hpp>

ColoredSlabTile::Item::Item(int32_t id, Tile* tile, int32_t slabHalfId, int32_t slabFullId)
	: AuxDataTileItem(id, tile), slabHalfId(slabHalfId), slabFullId(slabFullId) {}

ColoredSlabTile::Item::~Item() {}

bool_t ColoredSlabTile::Item::useOn(ItemInstance* a2, Player* a3, Level* a4, int32_t x, int32_t y, int32_t z, int32_t face, float a9, float a10, float a11) {
	if (a2->count == 0) return 0;
	int32_t id = a4->getTile(x, y, z);
	int32_t AuxValue = a2->getAuxValue();
	uint32_t meta = a4->getData(x, y, z);
	uint32_t v16 = meta;
	int32_t v17 = (meta >> 3) & 1;

	if (face == 1) {
		if (v17) {
			return TileItem::useOn(a2, a3, a4, x, y, z, face, a9, a10, a11);
		}
	} else if (face == 0 || !v17) {
		return TileItem::useOn(a2, a3, a4, x, y, z, face, a9, a10, a11);
	}

	if (id == slabHalfId) {
		if ((meta ^ AuxValue) & 7) {
			return 0; // different color!
		}
		const AABB* v18 = Tile::tiles[slabFullId]->getAABB(a4, x, y, z);
		if (a4->isUnobstructed(*v18)) {
			if (a4->setTileAndData(x, y, z, slabFullId, v16 | AuxValue, 3)) {
				a4->playSound((float)x + 0.5f, (float)y + 0.5f, (float)z + 0.5f, Tile::tiles[slabFullId]->soundType->field_C, (float)(Tile::tiles[slabFullId]->soundType->field_0 + 1.0f) * 0.5f, Tile::tiles[slabFullId]->soundType->field_4 * 0.8f);
				--a2->count;
			}
		}
		return 1;
	}
	return TileItem::useOn(a2, a3, a4, x, y, z, face, a9, a10, a11);
}

ColoredSlabTile::ColoredSlabTile(int32_t id, bool_t isFull, int32_t colorOffset)
	: SlabTile(id, "colored_planks", isFull, Material::wood), colorOffset(colorOffset) {
	this->tex = *this->getTextureItem("colored_planks");
	this->textureUV = *this->tex.getUV(colorOffset);
	this->setDestroyTime(2.0f);
	this->setExplodeable(5.0f);
	this->setSoundType(Tile::SOUND_WOOD);
}

ColoredSlabTile::ColoredSlabTile(int32_t id, bool_t isFull, int32_t colorOffset, const std::string& texName, Material* material, const Tile::SoundType& soundType)
	: SlabTile(id, texName, isFull, material), colorOffset(colorOffset) {
	this->tex = *this->getTextureItem(texName);
	this->textureUV = *this->tex.getUV(colorOffset);
	this->setDestroyTime(2.0f);
	this->setExplodeable(5.0f);
	this->setSoundType(soundType);
}

ColoredSlabTile::~ColoredSlabTile() {}

TextureUVCoordinateSet* ColoredSlabTile::getTexture(int32_t face, int32_t data) {
	return this->tex.getUV((data & 7) + colorOffset);
}

int32_t ColoredSlabTile::getResource(int32_t, Random*) {
	return this->isFull ? partnerSlabId : blockID;
}

int32_t ColoredSlabTile::getSpawnResourcesAuxValue(int32_t data) {
	return data & 7;
}

std::string ColoredSlabTile::getTypeDescriptionId(int32_t data) {
	std::string colors[] = {"white", "orange", "magenta", "lightBlue", "yellow", "lime", "pink", "gray", "silver", "cyan", "purple", "blue", "brown", "green", "red", "black"};
	return colors[(data & 7) + colorOffset];
}

