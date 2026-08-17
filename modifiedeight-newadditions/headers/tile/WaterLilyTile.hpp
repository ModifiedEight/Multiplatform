#pragma once
#include <tile/Tile.hpp>

class WaterLilyTile : public Tile {
public:
	WaterLilyTile(int32_t id, const std::string& textureName);
	virtual ~WaterLilyTile();
	virtual int32_t getRenderLayer();
	virtual int32_t getRenderShape();
	virtual int32_t getColor(int32_t);
	virtual int32_t getColor(LevelSource*, int32_t, int32_t, int32_t);
	virtual bool_t mayPlace(Level*, int32_t, int32_t, int32_t);
	virtual bool_t canSurvive(Level*, int32_t, int32_t, int32_t);
	virtual void neighborChanged(Level*, int32_t, int32_t, int32_t, int32_t);
	virtual bool_t isSolid();
	virtual bool_t isSolidRender();
	virtual bool_t isCubeShaped();
	virtual AABB* getAABB(Level*, int32_t, int32_t, int32_t);
	virtual void getVisualShape(LevelSource*, int32_t, int32_t, int32_t, AABB&, bool_t);
};
