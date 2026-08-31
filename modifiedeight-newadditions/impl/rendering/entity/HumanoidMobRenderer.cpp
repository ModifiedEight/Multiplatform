#include <rendering/entity/HumanoidMobRenderer.hpp>
#include <entity/Mob.hpp>
#include <item/Item.hpp>
#include <rendering/EntityRenderDispatcher.hpp>
#include <rendering/ItemInHandRenderer.hpp>
#include <rendering/TileRenderer.hpp>
#include <rendering/model/HumanoidModel.hpp>
#include <tile/Tile.hpp>

HumanoidMobRenderer::HumanoidMobRenderer(HumanoidModel* a2, float a3)
	: MobRenderer(a2, a3) {
	this->hmodel = a2;
}
void HumanoidMobRenderer::renderHand() {
	this->hmodel->field_0 = 0.0;
	this->hmodel->setupAnim(0.0, 0.0, 0.0, 0.0, 0.0, 0.0625);
	glEnable(GL_TEXTURE_2D);
	this->hmodel->rightArmModel.render(0.0625f);
}

HumanoidMobRenderer::~HumanoidMobRenderer() {
}
void HumanoidMobRenderer::render(Entity* a2_, float a3, float a4, float a5, float a6, float a7) {
	Mob* a2 = (Mob*)a2_;
	if(a2->getCarriedItem()) {
		this->hmodel->field_319 = 1;
	}
	this->hmodel->field_31A = a2->isSneaking();
	MobRenderer::render(a2, a3, a4, a5, a6, a7);
	this->hmodel->field_319 = 0;
}
void HumanoidMobRenderer::additionalRendering(Mob* a2, float a3) {
	ItemInstance* v5; // r0
	ItemInstance* v6; // r4
	int32_t v9;			  // r0
	float v10;		  // r0
	float v11;		  // r0
	float v12;		  // r2
	float v13;		  // r3

	v5 = (ItemInstance*)a2->getCarriedItem();
	v6 = v5;
	if(v5 && v5->count > 0) {
		glPushMatrix();
		this->hmodel->rightArmModel.translateTo(0.0625);
		glTranslatef(-0.0625, 0.4375, 0.0625);
		if(v6->tileClass) {
			v9 = v6->tileClass->getRenderShape();
			if(TileRenderer::canRender(v9)) {
				glTranslatef(0.0, 0.1875, -0.3125);
				glRotatef(200.0, 1.0, 0.0, 0.0);
				glRotatef(45.0, 0.0, 1.0, 0.0);
				glScalef(0.375, 0.375, 0.375);
				goto LABEL_11;
			}
		}
		if(v6->itemClass == Item::bow) {
			glTranslatef(0.2f, 0.125f, -0.3875f);
			glRotatef(-180.0f, 1.0f, 0.0f, 0.0f);
			glRotatef(10.0f, 0.0f, 1.0f, 0.0f);
			glScalef(0.625f, 0.625f, 0.625f);
			glRotatef(-100.0f, 1.0f, 0.0f, 0.0f);
			glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
		} else {
			if(!v6->itemClass || !v6->itemClass->isHandEquipped()) {
				glTranslatef(0.20f, 0.22f, -0.10f);
				glScalef(0.45f, 0.45f, 0.45f);
				glRotatef(60.0f, 0.0f, 0.0f, 1.0f);
				glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
				glRotatef(20.0f, 0.0f, 0.0f, 1.0f);
			} else {
				glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
				glTranslatef(0.08f, 0.25f, 0.0f);
				glScalef(0.625f, 0.625f, 0.625f);
				glRotatef(80.0f, 1.0f, 0.0f, 0.0f);
				glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
			}
		}
LABEL_11:
		EntityRenderer::entityRenderDispatcher->itemInHandRenderer->renderItem(a2, v6);
		glPopMatrix();
	}
}
