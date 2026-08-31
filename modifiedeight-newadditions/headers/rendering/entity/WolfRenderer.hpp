#pragma once
#include <rendering/entity/MobRenderer.hpp>
#include <rendering/model/WolfModel.hpp>

struct WolfRenderer: MobRenderer {
	WolfRenderer(WolfModel*, float);
	virtual ~WolfRenderer();
};
