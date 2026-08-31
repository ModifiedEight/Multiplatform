#include <rendering/entity/SquidRenderer.hpp>
#include <entity/Squid.hpp>
#include <unigl.h>

SquidRenderer::SquidRenderer(SquidModel* model, float shadowRadius)
	: MobRenderer(model, shadowRadius) {
}

SquidRenderer::~SquidRenderer() {
}

void SquidRenderer::setupRotations(Entity* entity, float f, float g, float h) {
	Squid* squid = (Squid*)entity;
	float xRot = squid->xBodyRotO + (squid->xBodyRot - squid->xBodyRotO) * h;
	float zRot = squid->zBodyRotO + (squid->zBodyRot - squid->zBodyRotO) * h;
	glTranslatef(0.0f, 0.5f, 0.0f);
	glRotatef(180.0f - g, 0.0f, 1.0f, 0.0f);
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(zRot, 0.0f, 1.0f, 0.0f);
	glTranslatef(0.0f, -1.2f, 0.0f);
}

float SquidRenderer::getBob(Mob* mob, float f) {
	Squid* squid = (Squid*)mob;
	return squid->oldTentacleAngle + (squid->tentacleAngle - squid->oldTentacleAngle) * f;
}

float SquidRenderer::getAttackAnim(Mob* mob, float f) {
	return 0.0f;
}
