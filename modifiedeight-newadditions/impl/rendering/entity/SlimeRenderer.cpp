#include <rendering/entity/SlimeRenderer.hpp>
#include <entity/Slime.hpp>
#include <unigl.h>

SlimeRenderer::SlimeRenderer(SlimeModel* model, float shadowRadius)
	: MobRenderer(model, shadowRadius) {
}

SlimeRenderer::~SlimeRenderer() {
}

void SlimeRenderer::scale(Mob* mob, float a3) {
	Slime* slime = (Slime*)mob;
	float size = (float)slime->slimeSize;
	float squish = slime->oSquish + (slime->squish - slime->oSquish) * a3;
	float squishFactor = 1.0f / (squish + 1.0f);
	glScalef(squishFactor * size * 0.5f, (1.0f / squishFactor) * size * 0.5f, squishFactor * size * 0.5f);
}
