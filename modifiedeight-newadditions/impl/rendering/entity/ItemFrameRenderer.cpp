#include <rendering/entity/ItemFrameRenderer.hpp>
#include <entity/ItemFrame.hpp>
#include <rendering/Tesselator.hpp>
#include <rendering/EntityRenderDispatcher.hpp>
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

	GLboolean lightingWas = glIsEnabled(GL_LIGHTING);
	GLboolean fogWas = glIsEnabled(GL_FOG);
	if (lightingWas) glDisable(GL_LIGHTING);
	if (fogWas) glDisable(GL_FOG);

	glEnable(GL_TEXTURE_2D);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.1f);

	Tesselator& t = Tesselator::instance;

	this->bindTexture("terrain-atlas.tga");
	TextureUVCoordinateSet boardUV = Tile::getTextureUVCoordinateSet("itemframe_background", 0);
	if (boardUV.minX == 0.0f && boardUV.maxX == 0.0f && boardUV.minY == 0.0f && boardUV.maxY == 0.0f) {
		boardUV = Tile::getTextureUVCoordinateSet("planks", 2);
	}
	TextureUVCoordinateSet rimUV = Tile::getTextureUVCoordinateSet("planks", 0);

	glColor4f(0.93f, 0.93f, 0.93f, 1.0f);
	t.begin(0);
	t.vertexUV(-0.375f, -0.375f, -0.47f, boardUV.minX, boardUV.maxY);
	t.vertexUV(0.375f, -0.375f, -0.47f, boardUV.maxX, boardUV.maxY);
	t.vertexUV(0.375f, 0.375f, -0.47f, boardUV.maxX, boardUV.minY);
	t.vertexUV(-0.375f, 0.375f, -0.47f, boardUV.minX, boardUV.minY);
	t.draw(1);

	glColor4f(0.6f, 0.47f, 0.33f, 1.0f);
	t.begin(0);
	t.vertexUV(-0.4375f, -0.4375f, -0.48f, rimUV.minX, rimUV.maxY);
	t.vertexUV(0.4375f, -0.4375f, -0.48f, rimUV.maxX, rimUV.maxY);
	t.vertexUV(0.4375f, -0.375f, -0.48f, rimUV.maxX, rimUV.minY);
	t.vertexUV(-0.4375f, -0.375f, -0.48f, rimUV.minX, rimUV.minY);
	t.vertexUV(-0.4375f, 0.375f, -0.48f, rimUV.minX, rimUV.maxY);
	t.vertexUV(0.4375f, 0.375f, -0.48f, rimUV.maxX, rimUV.maxY);
	t.vertexUV(0.4375f, 0.4375f, -0.48f, rimUV.maxX, rimUV.minY);
	t.vertexUV(-0.4375f, 0.4375f, -0.48f, rimUV.minX, rimUV.minY);
	t.vertexUV(-0.4375f, -0.375f, -0.48f, rimUV.minX, rimUV.maxY);
	t.vertexUV(-0.375f, -0.375f, -0.48f, rimUV.maxX, rimUV.maxY);
	t.vertexUV(-0.375f, 0.375f, -0.48f, rimUV.maxX, rimUV.minY);
	t.vertexUV(-0.4375f, 0.375f, -0.48f, rimUV.minX, rimUV.minY);
	t.vertexUV(0.375f, -0.375f, -0.48f, rimUV.minX, rimUV.maxY);
	t.vertexUV(0.4375f, -0.375f, -0.48f, rimUV.maxX, rimUV.maxY);
	t.vertexUV(0.4375f, 0.375f, -0.48f, rimUV.maxX, rimUV.minY);
	t.vertexUV(0.375f, 0.375f, -0.48f, rimUV.minX, rimUV.minY);
	t.draw(1);

	ItemInstance dispItem = frame->getDisplayedItem();
	if (!dispItem.isNull() && dispItem.count > 0) {
		glPushMatrix();
		glTranslatef(0.0f, 0.0f, -0.495f);
		glRotatef((float)frame->getRotation() * 45.0f, 0.0f, 0.0f, 1.0f);
		glScalef(0.45f, 0.45f, 0.45f);

		int32_t id = dispItem.getId();
		int32_t aux = dispItem.getAuxValue();

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		if (Tile::enderChest && id == Tile::enderChest->blockID && Item::items[id]) {
			Item* it = Item::items[id];
			this->bindTexture(it->itemTexture.empty() ? "gui/items.png" : it->itemTexture);
			TextureUVCoordinateSet* uvs = it->getIcon(aux, 0, 0);
			if (uvs) {
				t.begin(0);
				t.vertexUV(-0.5f, -0.5f, 0.0f, uvs->minX, uvs->maxY);
				t.vertexUV(0.5f, -0.5f, 0.0f, uvs->maxX, uvs->maxY);
				t.vertexUV(0.5f, 0.5f, 0.0f, uvs->maxX, uvs->minY);
				t.vertexUV(-0.5f, 0.5f, 0.0f, uvs->minX, uvs->minY);
				t.draw(1);
			}
		} else if (id > 0 && id < 256 && Tile::tiles[id]) {
			this->bindTexture("terrain-atlas.tga");
			TextureUVCoordinateSet* uvs = Tile::tiles[id]->getTexture(2, aux);
			if (uvs) {
				t.begin(0);
				t.vertexUV(-0.5f, -0.5f, 0.0f, uvs->minX, uvs->maxY);
				t.vertexUV(0.5f, -0.5f, 0.0f, uvs->maxX, uvs->maxY);
				t.vertexUV(0.5f, 0.5f, 0.0f, uvs->maxX, uvs->minY);
				t.vertexUV(-0.5f, 0.5f, 0.0f, uvs->minX, uvs->minY);
				t.draw(1);
			} else if (Item::items[id]) {
				Item* it = Item::items[id];
				this->bindTexture(it->itemTexture.empty() ? "items-opaque.png" : it->itemTexture);
				TextureUVCoordinateSet* iuvs = it->getIcon(aux, 0, 0);
				if (iuvs) {
					t.begin(0);
					t.vertexUV(-0.5f, -0.5f, 0.0f, iuvs->minX, iuvs->maxY);
					t.vertexUV(0.5f, -0.5f, 0.0f, iuvs->maxX, iuvs->maxY);
					t.vertexUV(0.5f, 0.5f, 0.0f, iuvs->maxX, iuvs->minY);
					t.vertexUV(-0.5f, 0.5f, 0.0f, iuvs->minX, iuvs->minY);
					t.draw(1);
				}
			}
		} else if (id >= 256 && Item::items[id]) {
			Item* it = Item::items[id];
			this->bindTexture(it->itemTexture.empty() ? "items-opaque.png" : it->itemTexture);
			TextureUVCoordinateSet* uvs = it->getIcon(aux, 0, 0);
			if (uvs) {
				t.begin(0);
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
	if (lightingWas) glEnable(GL_LIGHTING);
	if (fogWas) glEnable(GL_FOG);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glPopMatrix();
}
