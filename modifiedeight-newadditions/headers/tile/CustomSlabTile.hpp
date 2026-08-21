#pragma once
#include <tile/SlabTile.hpp>
#include <item/AuxDataTileItem.hpp>
#include <rendering/TextureAtlasTextureItem.hpp>

struct CustomSlabTile : SlabTile {
	struct Item : AuxDataTileItem {
		int32_t slabHalfId;
		int32_t slabFullId;

		Item(int32_t id, Tile* tile, int32_t slabHalfId, int32_t slabFullId);
		virtual ~Item();
		virtual bool_t useOn(ItemInstance*, Player*, Level*, int32_t, int32_t, int32_t, int32_t, float, float, float);
	};

	int32_t slabType; // 0 = dirt, 1 = grass, 2 = stone (rock)
	int32_t partnerSlabId;

	CustomSlabTile(int32_t id, bool_t isFull, int32_t slabType);
	virtual ~CustomSlabTile();
	virtual TextureUVCoordinateSet* getTexture(int32_t face, int32_t data);
	virtual TextureUVCoordinateSet* getTexture(LevelSource* level, int32_t x, int32_t y, int32_t z, int32_t face);
	virtual int32_t getColor(LevelSource* level, int32_t x, int32_t y, int32_t z);
	virtual int32_t getColor(int32_t data);
	virtual int32_t getResource(int32_t data, Random* rand);
	virtual int32_t getSpawnResourcesAuxValue(int32_t data);
};
