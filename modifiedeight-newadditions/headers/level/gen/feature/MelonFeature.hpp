#pragma once

#include <level/gen/feature/Feature.hpp>

struct MelonFeature : public Feature {
	MelonFeature();
	virtual ~MelonFeature();

	virtual bool_t place(Level* level, Random* random, int32_t x, int32_t y, int32_t z);
};
