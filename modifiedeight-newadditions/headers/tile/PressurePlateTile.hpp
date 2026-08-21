#pragma once
#include <tile/Tile.hpp>

class PressurePlateTile : public Tile {
public:
	PressurePlateTile(int32_t id, const std::string& textureName, Material* material);
	PressurePlateTile(int32_t id, TextureUVCoordinateSet uv, Material* material);
	virtual ~PressurePlateTile();

	virtual bool_t isCubeShaped();
	virtual bool_t isSolidRender();
	virtual int32_t getRenderShape();
	virtual AABB* getAABB(Level* level, int32_t x, int32_t y, int32_t z);

	virtual void updateDefaultShape();
	virtual void updateShape(LevelSource* level, int32_t x, int32_t y, int32_t z);

	virtual bool_t mayPlace(Level* level, int32_t x, int32_t y, int32_t z);
	virtual void neighborChanged(Level* level, int32_t x, int32_t y, int32_t z, int32_t fromX, int32_t fromY, int32_t fromZ, int32_t fromTileId);
	virtual void onPlace(Level* level, int32_t x, int32_t y, int32_t z);
	virtual void onRemove(Level* level, int32_t x, int32_t y, int32_t z);
	virtual void entityInside(Level* level, int32_t x, int32_t y, int32_t z, Entity* entity);
	virtual void tick(Level* level, int32_t x, int32_t y, int32_t z, Random* random);

	virtual bool_t isSignalSource();
	virtual int32_t getSignal(LevelSource* level, int32_t x, int32_t y, int32_t z, int32_t side);
	virtual int32_t getDirectSignal(Level* level, int32_t x, int32_t y, int32_t z, int32_t side);

private:
	void checkPressed(Level* level, int32_t x, int32_t y, int32_t z);
};
