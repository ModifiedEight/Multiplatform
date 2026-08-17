#pragma once

#include <level/gen/feature/Feature.hpp>

struct JungleBushFeature : public Feature {
	int32_t logMeta;
	int32_t leavesMeta;

	JungleBushFeature(int32_t logMeta, int32_t leavesMeta);
	virtual ~JungleBushFeature();

	virtual bool_t place(Level* level, Random* random, int32_t x, int32_t y, int32_t z);
};
