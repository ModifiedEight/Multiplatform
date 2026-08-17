#pragma once

#include <level/gen/feature/Feature.hpp>

class DesertTempleFeature : public Feature {
public:
	DesertTempleFeature(bool_t update = 0);
	virtual ~DesertTempleFeature();

	virtual bool_t place(Level* level, Random* random, int32_t x, int32_t y, int32_t z);

private:
	void placeBlockRotated(Level* level, int32_t originX, int32_t originY, int32_t originZ, int32_t lx, int32_t ly, int32_t lz, int32_t blockId, int32_t data, int32_t rotation);
	void fillLoot(Level* level, Random* random, int32_t x, int32_t y, int32_t z);
};
