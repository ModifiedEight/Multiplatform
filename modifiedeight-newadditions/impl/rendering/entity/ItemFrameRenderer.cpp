#include <rendering/entity/ItemFrameRenderer.hpp>
#include <entity/ItemFrame.hpp>
#include <rendering/Tesselator.hpp>
#include <rendering/EntityRenderDispatcher.hpp>
#include <rendering/entity/ItemRenderer.hpp>
#include <rendering/TileRenderer.hpp>
#include <item/Item.hpp>
#include <tile/Tile.hpp>
#include <tile/MobHeadTile.hpp>
#include <rendering/tileentity/MobHeadRenderer.hpp>
#include <unigl.h>

ItemFrameRenderer::ItemFrameRenderer()
	: EntityRenderer() {
}

ItemFrameRenderer::~ItemFrameRenderer() {
}

void ItemFrameRenderer::render(Entity* entity, float x, float y, float z, float rot, float a6) {
	ItemFrame* frame = (ItemFrame*)entity;
	if (!frame) return;

	glPushMatrix();
	glTranslatef(x, y, z);
	glRotatef(frame->yaw, 0.0f, 1.0f, 0.0f);
	if (frame->direction == 4) {
		glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	} else if (frame->direction == 5) {
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	}

	GLboolean lightingWas = glIsEnabled(GL_LIGHTING);
	GLboolean cullWas = glIsEnabled(GL_CULL_FACE);
	GLboolean blendWas = glIsEnabled(GL_BLEND);
	GLboolean alphaTestWas = glIsEnabled(GL_ALPHA_TEST);
	GLboolean depthTestWas = glIsEnabled(GL_DEPTH_TEST);

	if (lightingWas) glDisable(GL_LIGHTING);
	if (cullWas) glDisable(GL_CULL_FACE);
	if (!depthTestWas) glEnable(GL_DEPTH_TEST);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.1f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	Tesselator& t = Tesselator::instance;
	this->bindTexture("item_frame.png");

	t.begin(4);
	t.vertexUV(-0.5f, -0.5f, -0.0625f, 0.0f, 1.0f);
	t.vertexUV(0.5f, -0.5f, -0.0625f, 1.0f, 1.0f);
	t.vertexUV(0.5f, 0.5f, -0.0625f, 1.0f, 0.0f);
	t.vertexUV(-0.5f, 0.5f, -0.0625f, 0.0f, 0.0f);

	t.vertexUV(-0.5f, 0.5f, 0.0f, 0.0f, 0.0f);
	t.vertexUV(0.5f, 0.5f, 0.0f, 1.0f, 0.0f);
	t.vertexUV(0.5f, 0.5f, -0.0625f, 1.0f, 0.0625f);
	t.vertexUV(-0.5f, 0.5f, -0.0625f, 0.0f, 0.0625f);

	t.vertexUV(-0.5f, -0.5f, -0.0625f, 0.0f, 0.9375f);
	t.vertexUV(0.5f, -0.5f, -0.0625f, 1.0f, 0.9375f);
	t.vertexUV(0.5f, -0.5f, 0.0f, 1.0f, 1.0f);
	t.vertexUV(-0.5f, -0.5f, 0.0f, 0.0f, 1.0f);

	t.vertexUV(-0.5f, -0.5f, 0.0f, 0.0f, 1.0f);
	t.vertexUV(-0.5f, 0.5f, 0.0f, 0.0f, 0.0f);
	t.vertexUV(-0.5f, 0.5f, -0.0625f, 0.0625f, 0.0f);
	t.vertexUV(-0.5f, -0.5f, -0.0625f, 0.0625f, 1.0f);

	t.vertexUV(0.5f, -0.5f, -0.0625f, 0.9375f, 1.0f);
	t.vertexUV(0.5f, 0.5f, -0.0625f, 0.9375f, 0.0f);
	t.vertexUV(0.5f, 0.5f, 0.0f, 1.0f, 0.0f);
	t.vertexUV(0.5f, -0.5f, 0.0f, 1.0f, 1.0f);

	t.vertexUV(0.5f, -0.5f, 0.0f, 1.0f, 1.0f);
	t.vertexUV(-0.5f, -0.5f, 0.0f, 0.0f, 1.0f);
	t.vertexUV(-0.5f, 0.5f, 0.0f, 0.0f, 0.0f);
	t.vertexUV(0.5f, 0.5f, 0.0f, 1.0f, 0.0f);
	t.draw(1);

	ItemInstance dispItem = frame->getDisplayedItem();
	if (!dispItem.isNull() && dispItem.count > 0) {
		int32_t id = dispItem.getId();
		int32_t aux = dispItem.getAuxValue();

		glPushMatrix();
		glTranslatef(0.0f, 0.0f, -0.07f);
		glRotatef((float)frame->getRotation() * 45.0f, 0.0f, 0.0f, 1.0f);

		float s = 0.28f;
		if (MobHeadTile::isHeadBlock(id)) {
			int htype = MobHeadTile::getHeadType(id);
			if (MobHeadRenderer::instance) {
				glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
				glScalef(0.45f, 0.45f, 0.45f);
				MobHeadRenderer::instance->renderHead(htype, 0.0f, 0.0f, 0.0f, 0.0f, 0.0625f);
			}
		} else if (Tile::enderChest && id == Tile::enderChest->blockID && Item::items[id]) {
			Item* it = Item::items[id];
			this->bindTexture(it->itemTexture.empty() ? "gui/items.png" : it->itemTexture);
			TextureUVCoordinateSet* uvs = it->getIcon(aux, 0, 0);
			if (uvs) {
				t.begin(4);
				t.vertexUV(-s, -s, 0.0f, uvs->minX, uvs->maxY);
				t.vertexUV(s, -s, 0.0f, uvs->maxX, uvs->maxY);
				t.vertexUV(s, s, 0.0f, uvs->maxX, uvs->minY);
				t.vertexUV(-s, s, 0.0f, uvs->minX, uvs->minY);
				t.draw(1);
			}
		} else if (id >= 256 && Item::items[id]) {
			Item* it = Item::items[id];
			this->bindTexture(it->itemTexture.empty() ? "gui/items.png" : it->itemTexture);
			TextureUVCoordinateSet* uvs = it->getIcon(aux, 0, 0);
			if (uvs) {
				t.begin(4);
				t.vertexUV(-s, -s, 0.0f, uvs->minX, uvs->maxY);
				t.vertexUV(s, -s, 0.0f, uvs->maxX, uvs->maxY);
				t.vertexUV(s, s, 0.0f, uvs->maxX, uvs->minY);
				t.vertexUV(-s, s, 0.0f, uvs->minX, uvs->minY);
				t.draw(1);
			}
		} else if (id > 0 && id < 256 && Tile::tiles[id]) {
			Tile* tile = Tile::tiles[id];
			this->bindTexture("terrain.png");
			int shape = tile->getRenderShape();
			if (shape == 0 || tile->isCubeShaped()) {
				float bs = 0.22f;
				float x0 = -bs, x1 = bs;
				float y0 = -bs, y1 = bs;
				float z0 = 0.0f, z1 = -bs;

				TextureUVCoordinateSet* u0 = tile->getTexture(0, aux);
				TextureUVCoordinateSet* u1 = tile->getTexture(1, aux);
				TextureUVCoordinateSet* u2 = tile->getTexture(2, aux);
				TextureUVCoordinateSet* u3 = tile->getTexture(3, aux);
				TextureUVCoordinateSet* u4 = tile->getTexture(4, aux);
				TextureUVCoordinateSet* u5 = tile->getTexture(5, aux);

				if (u2) {
					t.begin(4);
					t.colorABGR(0xFFFFFFFF);
					t.vertexUV(x0, y0, z1, u2->minX, u2->maxY);
					t.vertexUV(x1, y0, z1, u2->maxX, u2->maxY);
					t.vertexUV(x1, y1, z1, u2->maxX, u2->minY);
					t.vertexUV(x0, y1, z1, u2->minX, u2->minY);
					t.draw(1);
				}
				if (u1) {
					float u1_midY = u1->minY + (u1->maxY - u1->minY) * 0.5f;
					t.begin(4);
					t.colorABGR(0xFFE8E8E8);
					t.vertexUV(x0, y1, z1, u1->minX, u1_midY);
					t.vertexUV(x1, y1, z1, u1->maxX, u1_midY);
					t.vertexUV(x1, y1, z0, u1->maxX, u1->minY);
					t.vertexUV(x0, y1, z0, u1->minX, u1->minY);
					t.draw(1);
				}
				if (u0) {
					float u0_midY = u0->minY + (u0->maxY - u0->minY) * 0.5f;
					t.begin(4);
					t.colorABGR(0xFF989898);
					t.vertexUV(x0, y0, z0, u0->minX, u0->maxY);
					t.vertexUV(x1, y0, z0, u0->maxX, u0->maxY);
					t.vertexUV(x1, y0, z1, u0->maxX, u0_midY);
					t.vertexUV(x0, y0, z1, u0->minX, u0_midY);
					t.draw(1);
				}
				if (u4) {
					float u4_midX = u4->minX + (u4->maxX - u4->minX) * 0.5f;
					t.begin(4);
					t.colorABGR(0xFFC8C8C8);
					t.vertexUV(x0, y0, z0, u4->minX,  u4->maxY);
					t.vertexUV(x0, y0, z1, u4_midX, u4->maxY);
					t.vertexUV(x0, y1, z1, u4_midX, u4->minY);
					t.vertexUV(x0, y1, z0, u4->minX,  u4->minY);
					t.draw(1);
				}
				if (u5) {
					float u5_midX = u5->maxX - (u5->maxX - u5->minX) * 0.5f;
					t.begin(4);
					t.colorABGR(0xFFC8C8C8);
					t.vertexUV(x1, y0, z1, u5_midX, u5->maxY);
					t.vertexUV(x1, y0, z0, u5->maxX,  u5->maxY);
					t.vertexUV(x1, y1, z0, u5->maxX,  u5->minY);
					t.vertexUV(x1, y1, z1, u5_midX, u5->minY);
					t.draw(1);
				}
			} else {
				TextureUVCoordinateSet* uvs = tile->getTexture(2, aux);
				if (uvs) {
					t.begin(4);
					t.colorABGR(0xFFFFFFFF);
					t.vertexUV(-s, -s, 0.0f, uvs->minX, uvs->maxY);
					t.vertexUV(s, -s, 0.0f, uvs->maxX, uvs->maxY);
					t.vertexUV(s, s, 0.0f, uvs->maxX, uvs->minY);
					t.vertexUV(-s, s, 0.0f, uvs->minX, uvs->minY);
					t.draw(1);
				}
			}
		}
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glPopMatrix();
	}

	if (!alphaTestWas) glDisable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.5f);
	if (!blendWas) glDisable(GL_BLEND);
	if (cullWas) glEnable(GL_CULL_FACE);
	if (lightingWas) glEnable(GL_LIGHTING);
	if (!depthTestWas) glDisable(GL_DEPTH_TEST);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glPopMatrix();
}
