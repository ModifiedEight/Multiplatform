#include <rendering/entity/TurtleRenderer.hpp>

TurtleRenderer::TurtleRenderer(TurtleModel* model, float shadowRadius)
	: MobRenderer(model, shadowRadius) {
}

TurtleRenderer::~TurtleRenderer() {
}
