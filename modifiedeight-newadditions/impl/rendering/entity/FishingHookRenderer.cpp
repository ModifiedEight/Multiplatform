#include <rendering/entity/FishingHookRenderer.hpp>
#include <entity/FishingHook.hpp>
#include <entity/Player.hpp>
#include <Minecraft.hpp>
#include <rendering/Tesselator.hpp>
#include <rendering/EntityRenderDispatcher.hpp>
#include <math/Mth.hpp>
#include <unigl.h>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

FishingHookRenderer::FishingHookRenderer()
	: EntityRenderer() {
}

FishingHookRenderer::~FishingHookRenderer() {
}

void FishingHookRenderer::render(Entity* entity, float x, float y, float z, float rot, float a6) {
	FishingHook* hook = (FishingHook*)entity;

	glPushMatrix();
	glTranslatef(x, y + 0.1f, z);
	glEnable(32826);
	glScalef(0.5f, 0.5f, 0.5f);
	if (EntityRenderer::entityRenderDispatcher) {
		glRotatef(180.0f - EntityRenderer::entityRenderDispatcher->field_14, 0.0f, 1.0f, 0.0f);
		glRotatef(-EntityRenderer::entityRenderDispatcher->field_18, 1.0f, 0.0f, 0.0f);
	}

	this->bindTexture("mob/fishing_hook.png");
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.1f);
	glDisable(GL_CULL_FACE);

	Tesselator& t = Tesselator::instance;
	t.begin(7);
	t.normal(0.0f, 1.0f, 0.0f);
	t.color(0xFF, 0xFF, 0xFF, 0xFF);
	t.vertexUV(-0.5f, -0.5f, 0.0f, 0.0f, 1.0f);
	t.vertexUV(0.5f, -0.5f, 0.0f, 1.0f, 1.0f);
	t.vertexUV(0.5f, 0.5f, 0.0f, 1.0f, 0.0f);
	t.vertexUV(-0.5f, 0.5f, 0.0f, 0.0f, 0.0f);
	t.draw(0);
	glEnable(GL_CULL_FACE);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glPopMatrix();

	if (hook->owner) {
		Player* player = hook->owner;
		float o = (player->prevYaw + (player->yaw - player->prevYaw) * a6) * (float)(M_PI / 180.0);
		double p = Mth::sin(o);
		double q = Mth::cos(o);
		double r = 0.35;
		double hx = hook->prevPosX + (hook->posX - hook->prevPosX) * a6;
		double hy = hook->prevPosY + (hook->posY - hook->prevPosY) * a6 + 0.15;
		double hz = hook->prevPosZ + (hook->posZ - hook->prevPosZ) * a6;

		double tx, ty, tz;
		if (EntityRenderer::entityRenderDispatcher && EntityRenderer::entityRenderDispatcher->minecraft && !EntityRenderer::entityRenderDispatcher->minecraft->options.thirdPerson) {
			float pitchRad = (player->prevPitch + (player->pitch - player->prevPitch) * a6) * (float)(M_PI / 180.0);
			double handX = 0.35;
			double handY = -0.35;
			double handZ = 0.55;
			double cosYaw = Mth::cos(o);
			double sinYaw = Mth::sin(o);
			double cosPitch = Mth::cos(pitchRad);
			double sinPitch = Mth::sin(pitchRad);
			tx = (player->prevPosX + (player->posX - player->prevPosX) * a6) - sinYaw * cosPitch * handZ + cosYaw * handX;
			ty = (player->prevPosY + 1.62f + (player->posY - player->prevPosY) * a6) - sinPitch * handZ + handY;
			tz = (player->prevPosZ + (player->posZ - player->prevPosZ) * a6) + cosYaw * cosPitch * handZ + sinYaw * handX;
		} else {
			tx = (player->prevPosX + (player->posX - player->prevPosX) * a6) - q * r - p * 0.7;
			ty = player->prevPosY + 1.62f + (player->posY - player->prevPosY) * a6 - 0.45;
			tz = (player->prevPosZ + (player->posZ - player->prevPosZ) * a6) - p * r + q * 0.7;
		}

		double aa = tx - hx;
		double ab = ty - hy;
		double ac = tz - hz;

		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glLineWidth(2.0f);
		t.begin(3);
		t.color(0x33, 0x33, 0x33, 0xFF);
		for (int ae = 0; ae <= 16; ++ae) {
			float af = (float)ae / 16.0f;
			t.vertex(x + (float)(aa * af), y + 0.15f + (float)(ab * (af * af + af) * 0.5 + 0.15 * (1.0 - af)), z + (float)(ac * af));
		}
		t.draw(0);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glLineWidth(1.0f);
		glEnable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
	}
}
