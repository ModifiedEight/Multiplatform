#include <rendering/entity/HumanoidMobRenderer.hpp>
#include <entity/Mob.hpp>
#include <entity/Player.hpp>
#include <item/Item.hpp>
#include <rendering/EntityRenderDispatcher.hpp>
#include <rendering/ItemInHandRenderer.hpp>
#include <rendering/TileRenderer.hpp>
#include <rendering/model/HumanoidModel.hpp>
#include <rendering/tileentity/MobHeadRenderer.hpp>
#include <tile/Tile.hpp>

static int getMobHeadType(Tile* t) {
	if (t == Tile::head_steve) return 0;
	if (t == Tile::head_creeper) return 1;
	if (t == Tile::head_zombie) return 2;
	if (t == Tile::head_skeleton) return 3;
	if (t == Tile::head_spider) return 4;
	if (t == Tile::head_pigzombie) return 5;
	if (t == Tile::head_slime) return 6;
	if (t == Tile::head_cow) return 7;
	if (t == Tile::head_pig) return 8;
	if (t == Tile::head_sheep) return 9;
	if (t == Tile::head_chicken) return 10;
	if (t == Tile::head_villager) return 11;
	if (t == Tile::head_ocelot) return 12;
	if (t == Tile::head_polarbear) return 13;
	if (t == Tile::head_turtle) return 14;
	if (t == Tile::head_giant) return 15;
	if (t == Tile::head_wolf) return 16;
	if (t == Tile::head_fox) return 17;
	return -1;
}

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
	bool hasHead = false;
	if (a2->isPlayer()) {
		Player* p = (Player*)a2;
		ItemInstance* headArmor = p->getArmor(0);
		if (headArmor && headArmor->tileClass && getMobHeadType(headArmor->tileClass) >= 0) {
			hasHead = true;
			this->hmodel->headModel.field_1D = 0;
		}
	}
	MobRenderer::render(a2, a3, a4, a5, a6, a7);
	if (hasHead) {
		this->hmodel->headModel.field_1D = 1;
	}
	this->hmodel->field_319 = 0;
}
void HumanoidMobRenderer::additionalRendering(Mob* a2, float a3) {
	if (a2 && a2->isPlayer()) {
		Player* p = (Player*)a2;
		ItemInstance* headArmor = p->getArmor(0);
		if (headArmor && headArmor->tileClass) {
			int ht = getMobHeadType(headArmor->tileClass);
			if (ht >= 0 && MobHeadRenderer::instance) {
				float s = 1.0625f;
				float yOff = -0.25f;
				if (ht == 9) { s = 1.25f; }
				else if (ht == 10) { s = 1.6f; yOff = -0.22f; }
				else if (ht == 11) { s = 1.0f; yOff = -0.3125f; }
				else if (ht == 12) { s = 1.25f; yOff = -0.125f; }
				else if (ht == 13) { s = 1.15f; }
				else if (ht == 14) { s = 1.25f; yOff = -0.16f; }
				else if (ht == 16) { s = 1.25f; }
				else if (ht == 17) { s = 1.25f; }

				glPushMatrix();
				this->hmodel->headModel.translateTo(0.0625f);
				glTranslatef(0.0f, yOff, 0.0f);
				glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
				glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
				glScalef(s, s, s);
				MobHeadRenderer::instance->renderHead(ht, 0.0f, 0.0f, 0.0f, 0.0f, 0.0625f);
				glPopMatrix();
			}
		}
	}

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
