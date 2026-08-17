#pragma once
#include <level/gen/feature/Feature.hpp>

struct DoublePlantFeature : public Feature {
	int32_t type;

	DoublePlantFeature(int32_t type);
	virtual ~DoublePlantFeature();
	virtual bool_t place(Level* level, Random* random, int32_t x, int32_t y, int32_t z);
};
