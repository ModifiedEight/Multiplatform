#pragma once
#include <level/gen/feature/Feature.hpp>

struct EquatorialTreeFeature : public Feature {
	EquatorialTreeFeature(bool_t update = 0);
	virtual ~EquatorialTreeFeature();
	virtual bool_t place(Level*, Random*, int32_t, int32_t, int32_t);
};
