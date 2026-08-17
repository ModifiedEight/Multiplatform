#pragma once
#include <tile/Bush.hpp>

struct SeagrassTile : Bush {
	TextureUVCoordinateSet textureTallLower;
	TextureUVCoordinateSet textureTallUpper;

	SeagrassTile(int32_t, const std::string&);
	virtual ~SeagrassTile();

	virtual TextureUVCoordinateSet* getTexture(int32_t, int32_t);
	virtual TextureUVCoordinateSet* getCarriedTexture(int32_t, int32_t);
	virtual bool_t canSurvive(Level*, int32_t, int32_t, int32_t);
	virtual bool_t mayPlace(Level*, int32_t, int32_t, int32_t);
	virtual void neighborChanged(Level*, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t);
	virtual void onRemove(Level*, int32_t, int32_t, int32_t);
	virtual int32_t getResource(int32_t, Random*);
	virtual int32_t getResourceCount(Random*);
	virtual int32_t getColor(LevelSource*, int32_t, int32_t, int32_t);
	virtual int32_t getColor(int32_t);
	virtual int32_t getRenderShape();
	virtual int32_t getRenderLayer();
	virtual bool_t isSolidRender();
	virtual AABB* getAABB(Level*, int32_t, int32_t, int32_t);
	virtual bool_t mayPick();
};
