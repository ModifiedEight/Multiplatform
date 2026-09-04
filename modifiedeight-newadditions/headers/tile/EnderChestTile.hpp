#pragma once
#include <tile/EntityTile.hpp>
#include <rendering/TextureUVCoordinateSet.hpp>

struct EnderChestTile: EntityTile {
	TextureUVCoordinateSet field_A50;
	TextureUVCoordinateSet field_A68;
	TextureUVCoordinateSet field_A80;

	EnderChestTile(int32_t id);
	virtual ~EnderChestTile();
	virtual bool_t isCubeShaped() override;
	virtual int32_t getRenderShape() override;
	virtual TextureUVCoordinateSet* getTexture(int32_t) override;
	virtual TextureUVCoordinateSet* getTexture(LevelSource*, int32_t, int32_t, int32_t, int32_t) override;
	virtual TextureUVCoordinateSet* getCarriedTexture(int32_t, int32_t) override;
	virtual bool_t isSolidRender() override;
	virtual int32_t getTileEntityType() override;
	virtual void onRemove(Level*, int32_t, int32_t, int32_t) override;
	virtual bool_t use(Level*, int32_t, int32_t, int32_t, Player*) override;
	virtual int32_t getPlacementDataValue(Level*, int32_t, int32_t, int32_t, int32_t, float, float, float, struct Mob*, int32_t) override;
	virtual int32_t getResource(int32_t, Random*) override;
	virtual int32_t getResourceCount(Random*) override;
	virtual void animateTick(Level*, int32_t, int32_t, int32_t, Random*) override;
};
