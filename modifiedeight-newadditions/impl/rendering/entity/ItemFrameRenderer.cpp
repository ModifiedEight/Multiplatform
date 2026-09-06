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

	float zBack = 0.002f;
	float zFront = -0.0625f;
	float zPlate = -0.01f;

	t.begin(4);
	t.vertexUV(-0.375f, -0.375f, zPlate, 0.125f, 0.875f);
	t.vertexUV(0.375f, -0.375f, zPlate, 0.875f, 0.875f);
	t.vertexUV(0.375f, 0.375f, zPlate, 0.875f, 0.125f);
	t.vertexUV(-0.375f, 0.375f, zPlate, 0.125f, 0.125f);

	t.vertexUV(-0.5f, 0.375f, zFront, 0.0f, 0.125f);
	t.vertexUV(0.5f, 0.375f, zFront, 1.0f, 0.125f);
	t.vertexUV(0.5f, 0.5f, zFront, 1.0f, 0.0f);
	t.vertexUV(-0.5f, 0.5f, zFront, 0.0f, 0.0f);

	t.vertexUV(-0.5f, -0.5f, zFront, 0.0f, 1.0f);
	t.vertexUV(0.5f, -0.5f, zFront, 1.0f, 1.0f);
	t.vertexUV(0.5f, -0.375f, zFront, 1.0f, 0.875f);
	t.vertexUV(-0.5f, -0.375f, zFront, 0.0f, 0.875f);

	t.vertexUV(-0.5f, -0.375f, zFront, 0.0f, 0.875f);
	t.vertexUV(-0.375f, -0.375f, zFront, 0.125f, 0.875f);
	t.vertexUV(-0.375f, 0.375f, zFront, 0.125f, 0.125f);
	t.vertexUV(-0.5f, 0.375f, zFront, 0.0f, 0.125f);

	t.vertexUV(0.375f, -0.375f, zFront, 0.875f, 0.875f);
	t.vertexUV(0.5f, -0.375f, zFront, 1.0f, 0.875f);
	t.vertexUV(0.5f, 0.375f, zFront, 1.0f, 0.125f);
	t.vertexUV(0.375f, 0.375f, zFront, 0.875f, 0.125f);

	t.vertexUV(-0.5f, 0.5f, zBack, 0.0f, 0.0625f);
	t.vertexUV(0.5f, 0.5f, zBack, 1.0f, 0.0625f);
	t.vertexUV(0.5f, 0.5f, zFront, 1.0f, 0.0f);
	t.vertexUV(-0.5f, 0.5f, zFront, 0.0f, 0.0f);

	t.vertexUV(-0.5f, -0.5f, zFront, 0.0f, 0.0f);
	t.vertexUV(0.5f, -0.5f, zFront, 1.0f, 0.0f);
	t.vertexUV(0.5f, -0.5f, zBack, 1.0f, 0.0625f);
	t.vertexUV(-0.5f, -0.5f, zBack, 0.0f, 0.0625f);

	t.vertexUV(-0.5f, -0.5f, zFront, 0.0f, 0.0f);
	t.vertexUV(-0.5f, 0.5f, zFront, 1.0f, 0.0f);
	t.vertexUV(-0.5f, 0.5f, zBack, 1.0f, 0.0625f);
	t.vertexUV(-0.5f, -0.5f, zBack, 0.0f, 0.0625f);

	t.vertexUV(0.5f, -0.5f, zBack, 0.0f, 0.0625f);
	t.vertexUV(0.5f, 0.5f, zBack, 1.0f, 0.0625f);
	t.vertexUV(0.5f, 0.5f, zFront, 1.0f, 0.0f);
	t.vertexUV(0.5f, -0.5f, zFront, 0.0f, 0.0f);

	t.vertexUV(-0.375f, 0.375f, zFront, 0.0f, 0.0f);
	t.vertexUV(0.375f, 0.375f, zFront, 1.0f, 0.0f);
	t.vertexUV(0.375f, 0.375f, zPlate, 1.0f, 0.0625f);
	t.vertexUV(-0.375f, 0.375f, zPlate, 0.0f, 0.0625f);

	t.vertexUV(-0.375f, -0.375f, zPlate, 0.0f, 0.0625f);
	t.vertexUV(0.375f, -0.375f, zPlate, 1.0f, 0.0625f);
	t.vertexUV(0.375f, -0.375f, zFront, 1.0f, 0.0f);
	t.vertexUV(-0.375f, -0.375f, zFront, 0.0f, 0.0f);

	t.vertexUV(-0.375f, -0.375f, zPlate, 0.0f, 0.0625f);
	t.vertexUV(-0.375f, 0.375f, zPlate, 1.0f, 0.0625f);
	t.vertexUV(-0.375f, 0.375f, zFront, 1.0f, 0.0f);
	t.vertexUV(-0.375f, -0.375f, zFront, 0.0f, 0.0f);

	t.vertexUV(0.375f, -0.375f, zFront, 0.0f, 0.0f);
	t.vertexUV(0.375f, 0.375f, zFront, 1.0f, 0.0f);
	t.vertexUV(0.375f, 0.375f, zPlate, 1.0f, 0.0625f);
	t.vertexUV(0.375f, -0.375f, zPlate, 0.0f, 0.0625f);
	t.draw(1);

	ItemInstance dispItem = frame->getDisplayedItem();
	if (!dispItem.isNull() && dispItem.count > 0) {
		int32_t id = dispItem.getId();
		int32_t aux = dispItem.getAuxValue();
		Tile* tileClass = dispItem.tileClass;

		if (tileClass && (Tile::enderChest ? tileClass != Tile::enderChest : true) && TileRenderer::canRender(tileClass->getRenderShape())) {
			glPushMatrix();
			glTranslatef(0.0f, 0.0f, -0.09f);
			glRotatef((float)frame->getRotation() * 45.0f, 0.0f, 0.0f, 1.0f);
			glScalef(0.42f, 0.42f, 0.42f);
			glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
			glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
			glTranslatef(-0.5f, -0.5f, -0.5f);
			this->bindTexture("terrain.png");
			if (ItemRenderer::tileRenderer) {
				ItemRenderer::tileRenderer->renderGuiTile(tileClass, aux, 1.0f, 1.0f);
			}
			glPopMatrix();
		} else {
			glPushMatrix();
			glTranslatef(0.0f, 0.0f, -0.045f);
			glRotatef((float)frame->getRotation() * 45.0f, 0.0f, 0.0f, 1.0f);

			if (MobHeadTile::isHeadBlock(id)) {
				int htype = MobHeadTile::getHeadType(id);
				if (MobHeadRenderer::instance) {
					glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
					glScalef(1.2f, 1.2f, 1.2f);
					MobHeadRenderer::instance->renderHead(htype, 0.0f, 0.0f, 0.0f, 0.0f, 0.0625f);
				}
			} else if (Tile::enderChest && id == Tile::enderChest->blockID && Item::items[id]) {
				Item* it = Item::items[id];
				this->bindTexture(it->itemTexture.empty() ? "gui/items.png" : it->itemTexture);
				TextureUVCoordinateSet* uvs = it->getIcon(aux, 0, 0);
				if (uvs) {
					t.begin(4);
					t.vertexUV(-0.5f, -0.5f, 0.0f, uvs->minX, uvs->maxY);
					t.vertexUV(0.5f, -0.5f, 0.0f, uvs->maxX, uvs->maxY);
					t.vertexUV(0.5f, 0.5f, 0.0f, uvs->maxX, uvs->minY);
					t.vertexUV(-0.5f, 0.5f, 0.0f, uvs->minX, uvs->minY);
					t.draw(1);
				}
			} else if (id >= 256 && Item::items[id]) {
				Item* it = Item::items[id];
				this->bindTexture(it->itemTexture.empty() ? "gui/items.png" : it->itemTexture);
				TextureUVCoordinateSet* uvs = it->getIcon(aux, 0, 0);
				if (uvs) {
					t.begin(4);
					t.vertexUV(-0.5f, -0.5f, 0.0f, uvs->minX, uvs->maxY);
					t.vertexUV(0.5f, -0.5f, 0.0f, uvs->maxX, uvs->maxY);
					t.vertexUV(0.5f, 0.5f, 0.0f, uvs->maxX, uvs->minY);
					t.vertexUV(-0.5f, 0.5f, 0.0f, uvs->minX, uvs->minY);
					t.draw(1);
				}
			} else if (id > 0 && id < 256 && Tile::tiles[id]) {
				this->bindTexture("terrain.png");
				TextureUVCoordinateSet* uvs = Tile::tiles[id]->getTexture(2, aux);
				if (uvs) {
					t.begin(4);
					t.vertexUV(-0.5f, -0.5f, 0.0f, uvs->minX, uvs->maxY);
					t.vertexUV(0.5f, -0.5f, 0.0f, uvs->maxX, uvs->maxY);
					t.vertexUV(0.5f, 0.5f, 0.0f, uvs->maxX, uvs->minY);
					t.vertexUV(-0.5f, 0.5f, 0.0f, uvs->minX, uvs->minY);
					t.draw(1);
				}
			}
			glPopMatrix();
		}
	}

	if (!alphaTestWas) glDisable(GL_ALPHA_TEST);
	if (!blendWas) glDisable(GL_BLEND);
	if (cullWas) glEnable(GL_CULL_FACE);
	if (lightingWas) glEnable(GL_LIGHTING);
	if (!depthTestWas) glDisable(GL_DEPTH_TEST);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glPopMatrix();
}
