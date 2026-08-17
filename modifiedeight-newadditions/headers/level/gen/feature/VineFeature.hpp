#pragma once

#include <level/gen/feature/Feature.hpp>

struct VineFeature : public Feature {
	VineFeature();
	virtual ~VineFeature();

	virtual bool_t place(Level* level, Random* random, int32_t x, int32_t y, int32_t z);
};
