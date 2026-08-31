#include <rendering/entity/FishRenderer.hpp>
#include <entity/Pufferfish.hpp>
#include <unigl.h>

FishRenderer::FishRenderer(Model* model, const std::string& tex, float shadowRadius)
	: MobRenderer(model, shadowRadius)
	, texturePath(tex) {
}

FishRenderer::~FishRenderer() {
}

void FishRenderer::setupRotations(Entity* entity, float f, float g, float h) {
	MobRenderer::setupRotations(entity, f, g, h);
}

void FishRenderer::render(Entity* entity, float x, float y, float z, float rot, float a6) {
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.1f);
	glDisable(GL_CULL_FACE);

	if (entity->entityRenderId == PUFFERFISH) {
		Pufferfish* puffer = (Pufferfish*)entity;
		float scale = 1.0f;
		if (puffer->puffState == 1) scale = 1.4f;
		else if (puffer->puffState == 2) scale = 1.8f;
		glPushMatrix();
		glScalef(scale, scale, scale);
		MobRenderer::render(entity, x / scale, y / scale, z / scale, rot, a6);
		glPopMatrix();
		glEnable(GL_CULL_FACE);
		glDisable(GL_ALPHA_TEST);
		return;
	}

	MobRenderer::render(entity, x, y, z, rot, a6);
	glEnable(GL_CULL_FACE);
	glDisable(GL_ALPHA_TEST);
}
