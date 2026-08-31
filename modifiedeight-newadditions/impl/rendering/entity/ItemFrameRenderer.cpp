#include <rendering/entity/ItemFrameRenderer.hpp>
#include <entity/ItemFrame.hpp>
#include <rendering/Tesselator.hpp>
#include <rendering/EntityRenderDispatcher.hpp>
#include <rendering/entity/ItemSpriteRenderer.hpp>
#include <item/Item.hpp>
#include <tile/Tile.hpp>
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
	glRotatef(frame->pitch, 1.0f, 0.0f, 0.0f);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	this->bindTexture("item_frame.png");

	Tesselator& t = Tesselator::instance;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.1f);

	t.begin(0);
	t.normal(0.0f, 0.0f, -1.0f);
	t.color(0xFF, 0xFF, 0xFF, 0xFF);
	t.vertexUV(-0.4375f, -0.4375f, -0.47f, 0.0f, 1.0f);
	t.vertexUV(0.4375f, -0.4375f, -0.47f, 1.0f, 1.0f);
	t.vertexUV(0.4375f, 0.4375f, -0.47f, 1.0f, 0.0f);
	t.vertexUV(-0.4375f, 0.4375f, -0.47f, 0.0f, 0.0f);
	t.draw(1);

	if (!frame->item.isNull() && frame->item.count > 0) {
		glPushMatrix();
		glTranslatef(0.0f, 0.0f, -0.49f);
		glRotatef((float)frame->rotation * 45.0f, 0.0f, 0.0f, 1.0f);
		glScalef(0.5f, 0.5f, 0.5f);

		Item* it = Item::items[frame->item.getId()];
		if (it) {
			TextureUVCoordinateSet* uvs = it->getIcon(frame->item.getAuxValue(), 0, 0);
			if (uvs) {
				this->bindTexture("gui/items.png");
				t.begin(0);
				t.color(0xFF, 0xFF, 0xFF, 0xFF);
				t.vertexUV(-0.5f, -0.5f, 0.0f, uvs->minX, uvs->maxY);
				t.vertexUV(0.5f, -0.5f, 0.0f, uvs->maxX, uvs->maxY);
				t.vertexUV(0.5f, 0.5f, 0.0f, uvs->maxX, uvs->minY);
				t.vertexUV(-0.5f, 0.5f, 0.0f, uvs->minX, uvs->minY);
				t.draw(1);
			}
		}
		glPopMatrix();
	}

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glPopMatrix();
}
