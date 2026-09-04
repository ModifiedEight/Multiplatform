#pragma once
#include <rendering/entity/MobRenderer.hpp>
#include <rendering/model/OcelotModel.hpp>

struct OcelotRenderer: MobRenderer {
	OcelotRenderer(OcelotModel*, float);
	virtual ~OcelotRenderer();
};
