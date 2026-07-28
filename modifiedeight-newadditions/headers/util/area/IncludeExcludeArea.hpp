#pragma once
#include <util/area/IArea.hpp>
#include <vector>

struct IncludeExcludeArea: IArea
{
	std::vector<IArea*> includeAreas;
	std::vector<IArea*> excludeAreas;

	virtual ~IncludeExcludeArea();
	virtual bool_t isInside(float x, float y);
	virtual void clear();
};
