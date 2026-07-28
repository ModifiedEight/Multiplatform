#pragma once
#include <tile/SlabTile.hpp>
#include <item/AuxDataTileItem.hpp>
#include <rendering/TextureAtlasTextureItem.hpp>

struct ColoredSlabTile : SlabTile {
	struct Item : AuxDataTileItem {
		int32_t slabHalfId;
		int32_t slabFullId;

		Item(int32_t id, Tile* tile, int32_t slabHalfId, int32_t slabFullId);
		virtual ~Item();
		virtual bool_t useOn(ItemInstance*, Player*, Level*, int32_t, int32_t, int32_t, int32_t, float, float, float);
	};

	TextureAtlasTextureItem tex;
	int32_t colorOffset;
	int32_t partnerSlabId;

	ColoredSlabTile(int32_t id, bool_t isFull, int32_t colorOffset);
	ColoredSlabTile(int32_t id, bool_t isFull, int32_t colorOffset, const std::string& texName, Material* material, const Tile::SoundType& soundType);

	virtual ~ColoredSlabTile();
	virtual TextureUVCoordinateSet* getTexture(int32_t face, int32_t data);
	virtual int32_t getResource(int32_t, Random*);
	virtual int32_t getSpawnResourcesAuxValue(int32_t data);
	virtual std::string getTypeDescriptionId(int32_t);
};

