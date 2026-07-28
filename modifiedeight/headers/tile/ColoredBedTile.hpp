#pragma once
#include <tile/BedTile.hpp>

struct ColoredBedTile : BedTile {
	int colorIndex;

	ColoredBedTile(int32_t id, int colorIndex);
	virtual ~ColoredBedTile();
	
	virtual TextureUVCoordinateSet* getTexture(int32_t face, int32_t data);
	virtual int32_t getResource(int32_t, Random*);
	virtual void spawnResources(Level*, int32_t, int32_t, int32_t, int32_t, float);
	virtual std::string getTypeDescriptionId(int32_t data);
};
