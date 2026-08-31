#pragma once
#include <rendering/entity/HumanoidMobRenderer.hpp>

struct GiantRenderer: HumanoidMobRenderer {
	GiantRenderer(HumanoidModel* model, float shadow)
		: HumanoidMobRenderer(model, shadow) {}

	virtual void scale(Mob* mob, float partialTicks) override {
		glScalef(6.0f, 6.0f, 6.0f);
	}
};
