#pragma once
#include <level/gen/feature/Feature.hpp>

struct HugeMushroomFeature : public Feature {
	int32_t mushroomType;

	HugeMushroomFeature(int32_t type);
	virtual ~HugeMushroomFeature();
	virtual bool_t place(Level* level, Random* random, int32_t x, int32_t y, int32_t z);
};

