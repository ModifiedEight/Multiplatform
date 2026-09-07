#include <rendering/entity/TurtleRenderer.hpp>
#include <unigl.h>

TurtleRenderer::TurtleRenderer(TurtleModel* model, float shadowRadius)
	: MobRenderer(model, shadowRadius) {
}

TurtleRenderer::~TurtleRenderer() {
}

void TurtleRenderer::render(Entity* entity, float x, float y, float z, float rot, float a6) {
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.1f);
	glDisable(GL_CULL_FACE);
	MobRenderer::render(entity, x, y, z, rot, a6);
	glEnable(GL_CULL_FACE);
	glDisable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.5f);
}
