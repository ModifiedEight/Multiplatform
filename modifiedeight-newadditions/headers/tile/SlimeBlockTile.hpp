#pragma once
#include <tile/Tile.hpp>

struct SlimeBlockTile: Tile {
	TextureAtlasTextureItem tex;

	SlimeBlockTile(int32_t, const std::string&);
	virtual ~SlimeBlockTile();
	virtual TextureUVCoordinateSet* getTexture(int32_t, int32_t);
	virtual TextureUVCoordinateSet* getCarriedTexture(int32_t, int32_t);
	virtual void fallOn(Level*, int32_t, int32_t, int32_t, Entity*, float);
	virtual void stepOn(Level*, int32_t, int32_t, int32_t, Entity*);
	virtual int32_t getRenderLayer();
	virtual bool_t isSolidRender();
};
