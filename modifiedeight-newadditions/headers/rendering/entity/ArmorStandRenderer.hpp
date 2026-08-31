#pragma once
#include <rendering/entity/MobRenderer.hpp>
#include <rendering/model/ArmorStandModel.hpp>

struct ArmorStandRenderer : MobRenderer {
	ArmorStandRenderer(Model*, float shadow);
	virtual ~ArmorStandRenderer();
	virtual void render(Entity*, float, float, float, float, float);
};
