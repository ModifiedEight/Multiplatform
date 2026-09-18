#include <rendering/tileentity/ChestRenderer.hpp>
#include <tile/entity/ChestTileEntity.hpp>
#include <tile/Tile.hpp>
#include <rendering/Textures.hpp>
#include <rendering/Tesselator.hpp>
#include <unigl.h>

ChestRenderer::ChestRenderer() : TileEntityRenderer(), field_8(0), field_1A8(1){

}
ChestRenderer::~ChestRenderer() {
}
void ChestRenderer::render(struct TileEntity* tee, float x, float y, float z, float a6, bool_t a7) {
	ChestTileEntity* te = (ChestTileEntity*)tee;
	int32_t level;		// r5
	int32_t v12;		// r7
	Tile* tile;			// r10
	int32_t data;		// r0
	float v15;			// r0
	float v16;			// r0
	float v17;			// r1
	float v18;			// r2
	ChestModel* v19;	// r7
	float v20;			// s18
	float modelOffsetX; // r0
	float v22;			// s15

	if(te->isUnpaired) {
		if(te->level) {
			v12 = 0;
			tile = te->getTile();
			data = te->getData();
			level = data;
			if(tile) {
				if(!data) {
					level = te->getData();
				}
			}
		} else {
			v12 = 1;
			level = 0;
		}
		if(!v12) {
			te->setupLighting(a7, a6);
		}
		glPushMatrix();
		glTranslatef(x, y + 1.0, z + 1.0);
		glScalef(1.0, -1.0, -1.0);
		glTranslatef(0.5, 0.5, 0.5);
		if(v12) {
			v16 = -1.0;
			v17 = 0.0;
			v18 = 0.0;
		} else {
			if(level == 2) {
				v15 = 180.0;
			} else if(level == 3) {
				v15 = 0.0;
			} else if(level == 4) {
				v15 = 90.0;
			} else if(level == 5) {
				v15 = -90.0;
			} else {
				v15 = 0.0;
			}
			glRotatef(v15, 0.0, 1.0, 0.0);
			v16 = -0.5;
			v17 = -0.5;
			v18 = -0.5;
		}
		glTranslatef(v16, v17, v18);
		if(te->pair) {
			v19 = &this->field_1A8;
			this->bindTexture("item/chest/double_normal.png");
			if(level == 2 || level == 5) {
				v20 = -1.0;
			} else {
				v20 = 1.0;
			}
			modelOffsetX = te->getModelOffsetX();
			glTranslatef(modelOffsetX * v20, 0.0, 0.0);
		} else {
			v19 = &this->field_8;
			if (te->type == 6 || (te->level && te->getTile() && Tile::enderChest && te->getTile() == Tile::enderChest)) {
				this->bindTexture("item/chest/ender.png");
			} else {
				this->bindTexture("item/chest/normal.png");
			}
		}
		v22 = 1.0 - (float)(te->field_88 + (float)((float)(te->field_84 - te->field_88) * a6));
		v19->field_18.xRotAngle = -(float)(0.5 * (float)((float)(1.0 - (float)((float)(v22 * v22) * v22)) * 3.1416));
		v19->render();
		glPopMatrix();
	}
}

void ChestRenderer::renderGuiChest(Textures* textures, bool isEnder, float x, float y, float scale, float alpha) {
	if (!textures) return;
	textures->loadAndBindTexture(isEnder ? "item/chest/ender.png" : "item/chest/normal.png");

	GLboolean blendWas = glIsEnabled(GL_BLEND);
	GLboolean alphaTestWas = glIsEnabled(GL_ALPHA_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.1f);
	glEnable(GL_TEXTURE_2D);

	float cx = x + 8.0f * scale;
	float cy = y + 5.5f * scale;
	float w = 6.0f * scale;
	float h_top = 3.5f * scale;
	float h_side = 6.5f * scale;

	Tesselator& t = Tesselator::instance;
	t.begin(20);

	float top_u0 = 14.0f / 64.0f, top_v0 = 0.0f / 64.0f;
	float top_u1 = 28.0f / 64.0f, top_v1 = 14.0f / 64.0f;
	t.color(1.0f, 1.0f, 1.0f, alpha);
	t.vertexUV(cx, cy - h_top, 0.0f, top_u0, top_v0);
	t.vertexUV(cx - w, cy, 0.0f, top_u0, top_v1);
	t.vertexUV(cx, cy + h_top, 0.0f, top_u1, top_v1);
	t.vertexUV(cx + w, cy, 0.0f, top_u1, top_v0);

	float lid_s_u0 = 0.0f / 64.0f, lid_s_v0 = 14.0f / 64.0f;
	float lid_s_u1 = 14.0f / 64.0f, lid_s_v1 = 19.0f / 64.0f;
	float base_s_u0 = 0.0f / 64.0f, base_s_v0 = 33.0f / 64.0f;
	float base_s_u1 = 14.0f / 64.0f, base_s_v1 = 43.0f / 64.0f;

	float lid_h = 2.5f * scale;
	float base_h = h_side - lid_h;

	t.color(0.72f, 0.72f, 0.72f, alpha);
	t.vertexUV(cx - w, cy, 0.0f, lid_s_u0, lid_s_v0);
	t.vertexUV(cx - w, cy + lid_h, 0.0f, lid_s_u0, lid_s_v1);
	t.vertexUV(cx, cy + h_top + lid_h, 0.0f, lid_s_u1, lid_s_v1);
	t.vertexUV(cx, cy + h_top, 0.0f, lid_s_u1, lid_s_v0);

	t.color(0.68f, 0.68f, 0.68f, alpha);
	t.vertexUV(cx - w, cy + lid_h, 0.0f, base_s_u0, base_s_v0);
	t.vertexUV(cx - w, cy + h_side, 0.0f, base_s_u0, base_s_v1);
	t.vertexUV(cx, cy + h_top + h_side, 0.0f, base_s_u1, base_s_v1);
	t.vertexUV(cx, cy + h_top + lid_h, 0.0f, base_s_u1, base_s_v0);

	float lid_f_u0 = 14.0f / 64.0f, lid_f_v0 = 14.0f / 64.0f;
	float lid_f_u1 = 28.0f / 64.0f, lid_f_v1 = 19.0f / 64.0f;
	float base_f_u0 = 14.0f / 64.0f, base_f_v0 = 33.0f / 64.0f;
	float base_f_u1 = 28.0f / 64.0f, base_f_v1 = 43.0f / 64.0f;

	t.color(0.55f, 0.55f, 0.55f, alpha);
	t.vertexUV(cx, cy + h_top, 0.0f, lid_f_u0, lid_f_v0);
	t.vertexUV(cx, cy + h_top + lid_h, 0.0f, lid_f_u0, lid_f_v1);
	t.vertexUV(cx + w, cy + lid_h, 0.0f, lid_f_u1, lid_f_v1);
	t.vertexUV(cx + w, cy, 0.0f, lid_f_u1, lid_f_v0);

	t.color(0.50f, 0.50f, 0.50f, alpha);
	t.vertexUV(cx, cy + h_top + lid_h, 0.0f, base_f_u0, base_f_v0);
	t.vertexUV(cx, cy + h_top + h_side, 0.0f, base_f_u0, base_f_v1);
	t.vertexUV(cx + w, cy + h_side, 0.0f, base_f_u1, base_f_v1);
	t.vertexUV(cx + w, cy + lid_h, 0.0f, base_f_u1, base_f_v0);

	float lock_u0 = 0.0f / 64.0f, lock_v0 = 1.0f / 64.0f;
	float lock_u1 = 2.0f / 64.0f, lock_v1 = 5.0f / 64.0f;
	float lock_w = 1.0f * scale;
	float lock_h = 2.0f * scale;
	float lock_cx = cx + w * 0.5f;
	float lock_cy = cy + h_top * 0.5f + lid_h;
	float lock_dx = lock_w * 0.5f;
	float lock_dy = lock_dx * (h_top / w);

	t.color(0.9f, 0.9f, 0.9f, alpha);
	t.vertexUV(lock_cx - lock_dx, lock_cy + lock_dy - 0.7f * scale, 0.0f, lock_u0, lock_v0);
	t.vertexUV(lock_cx - lock_dx, lock_cy + lock_dy + lock_h - 0.7f * scale, 0.0f, lock_u0, lock_v1);
	t.vertexUV(lock_cx + lock_dx, lock_cy - lock_dy + lock_h - 0.7f * scale, 0.0f, lock_u1, lock_v1);
	t.vertexUV(lock_cx + lock_dx, lock_cy - lock_dy - 0.7f * scale, 0.0f, lock_u1, lock_v0);

	t.draw(1);

	if (!alphaTestWas) glDisable(GL_ALPHA_TEST);
	if (!blendWas) glDisable(GL_BLEND);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

