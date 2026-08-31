#pragma once
#include <tile/Tile.hpp>

struct MusicPlayerTile : Tile {
	MusicPlayerTile(int32_t id);
	virtual ~MusicPlayerTile();
	virtual TextureUVCoordinateSet* getTexture(int32_t face, int32_t data);
	virtual TextureUVCoordinateSet* getCarriedTexture(int32_t face, int32_t data);
	virtual bool_t use(Level* level, int32_t x, int32_t y, int32_t z, Player* player);
};
