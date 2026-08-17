#pragma once

#include <level/gen/feature/Feature.hpp>

struct MegaJungleTreeFeature : public Feature {
	int32_t logMeta;
	int32_t leavesMeta;
	int32_t baseHeight;

	MegaJungleTreeFeature(bool_t update, int32_t baseHeight, int32_t logMeta, int32_t leavesMeta);
	virtual ~MegaJungleTreeFeature();

	virtual bool_t place(Level* level, Random* random, int32_t x, int32_t y, int32_t z);

private:
	void placeLeafCluster(Level* level, int32_t cx, int32_t cy, int32_t cz, int32_t radius);
	void addVine(Level* level, int32_t x, int32_t y, int32_t z, int32_t meta);
	void addHangingVine(Level* level, Random* random, int32_t x, int32_t y, int32_t z, int32_t meta, int32_t length);
};
