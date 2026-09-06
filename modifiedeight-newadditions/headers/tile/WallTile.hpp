#pragma once
#include <tile/Tile.hpp>
#include <math/HitResult.hpp>

struct WallTile : Tile {
	Tile* baseTile;
	int32_t baseAux;
	WallTile(int32_t, Tile*, int32_t = 0);
	bool_t connectsTo(LevelSource*, int32_t, int32_t, int32_t);

	virtual ~WallTile();
	virtual bool_t isCubeShaped();
	virtual int32_t getRenderShape();
	virtual void updateShape(LevelSource*, int32_t, int32_t, int32_t);
	virtual bool_t shouldRenderFace(LevelSource*, int32_t, int32_t, int32_t, int32_t);
	virtual TextureUVCoordinateSet* getTexture(int32_t, int32_t);
	virtual TextureUVCoordinateSet* getTexture(LevelSource*, int32_t, int32_t, int32_t, int32_t);
	virtual AABB* getAABB(Level*, int32_t, int32_t, int32_t);
	virtual AABB getTileAABB(Level*, int32_t, int32_t, int32_t);
	virtual bool_t isSolidRender();
	virtual int32_t getSpawnResourcesAuxValue(int32_t);
	virtual HitResult clip(Level*, int32_t, int32_t, int32_t, const Vec3&, const Vec3&);
};

