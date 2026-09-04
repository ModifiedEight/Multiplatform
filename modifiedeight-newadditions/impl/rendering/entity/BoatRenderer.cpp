#include <rendering/entity/BoatRenderer.hpp>
#include <entity/Boat.hpp>
#include <unigl.h>
#include <cmath>

static const char* _boatTextures[] = {
	"entity/boat/oak.png",
	"entity/boat/spruce.png",
	"entity/boat/birch.png",
	"entity/boat/jungle.png"
};

BoatRenderer::BoatRenderer()
	: EntityRenderer() {
}

BoatRenderer::~BoatRenderer() {
}

void BoatRenderer::render(Entity* entity, float x, float y, float z, float rot, float a6) {
	Boat* boat = (Boat*)entity;
	if (!boat) return;

	glPushMatrix();
	glTranslatef(x, y + 0.375f, z);
	glRotatef(180.0f - rot, 0.0f, 1.0f, 0.0f);

	if (boat->damageTime > 0) {
		float hurtTime = (float)boat->damageTime - a6;
		float hurtD = boat->damageTaken - a6;
		if (hurtD < 0.0f) hurtD = 0.0f;
		if (hurtTime > 0.0f) {
			glRotatef(std::sin(hurtTime) * hurtTime * hurtD / 10.0f * (float)boat->damageDir, 1.0f, 0.0f, 0.0f);
		}
	}

	glScalef(-1.0f, -1.0f, 1.0f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

	int32_t type = boat->getBoatType();
	if (type < 0 || type >= 4) type = 0;
	this->bindTexture(_boatTextures[type]);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	this->model.render(boat, a6, 0.0f, -0.1f, 0.0f, 0.0f, 0.0625f);
	glPopMatrix();
}
