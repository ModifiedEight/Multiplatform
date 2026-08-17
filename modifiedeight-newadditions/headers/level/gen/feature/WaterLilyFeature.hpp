#pragma once
#include <level/gen/feature/Feature.hpp>

class WaterLilyFeature : public Feature {
public:
	WaterLilyFeature();
	virtual ~WaterLilyFeature();
	virtual bool_t place(Level*, Random*, int32_t, int32_t, int32_t);
};
