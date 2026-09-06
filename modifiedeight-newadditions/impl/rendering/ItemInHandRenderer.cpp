#include <rendering/ItemInHandRenderer.hpp>
#include <Minecraft.hpp>
#include <entity/player/gamemode/GameMode.hpp>
#include <entity/LocalPlayer.hpp>
#include <inventory/Inventory.hpp>
#include <item/Item.hpp>
#include <level/Level.hpp>
#include <level/biome/Biome.hpp>
#include <math/Mth.hpp>
#include <rendering/EntityRenderDispatcher.hpp>
#include <rendering/EntityTileRenderer.hpp>
#include <rendering/Tesselator.hpp>
#include <rendering/Textures.hpp>
#include <rendering/entity/HumanoidMobRenderer.hpp>
#include <rendering/entity/PlayerRenderer.hpp>
#include <rendering/model/HumanoidModel.hpp>
#include <tile/material/Material.hpp>
#include <rendering/states/DisableState.hpp>
#include <rendering/states/EnableState.hpp>
#include <tile/Tile.hpp>
#include <tile/MobHeadTile.hpp>
#include <rendering/tileentity/MobHeadRenderer.hpp>
#include <utils.h>
#include <rendering/TextureTesselator.hpp>

ItemInHandRenderer::ItemInHandRenderer(struct Minecraft* a2)
	: field_4(0, 1, 0)
	, tileRenderer(0) {
	this->minecraft = a2;
	this->field_1C = 0;
	this->field_20 = 0;
	AppPlatform::_singleton->addListener(this, 1.0);
}
int32_t ItemInHandRenderer::_getFrameID(struct Mob* a2, ItemInstance* a3) {
	int32_t v7 = (a3->getId() | (((uint16_t)a3->getAuxValue()) << 12));
	if(a2) {
		v7 |= a2->getEntityTypeId() << 16; //byte2
		// A stack the id table could not represent has neither an item nor a
		// tile behind it and still reports itself valid, which is how id 0 is
		// spelled.  Frame 0 for it, rather than a null dereference.
		if(a3->itemClass) {
			v7 |= a3->itemClass->getAnimationFrameFor(a2) << 24; //hibyte
		}
	}
	return v7;
}
void ItemInHandRenderer::itemPlaced() {
	this->field_1C = 0;
}
void ItemInHandRenderer::itemUsed() {
	this->field_1C = 0;
}

RenderCall* ItemInHandRenderer::rebuildItem(struct Mob* a2, ItemInstance& a3) {
	//TODO check
	int fid = this->_getFrameID(a2, &a3);
	RenderCall* v7 = new RenderCall();
	this->field_90[fid].reset(v7);

	if(a3.tileClass && !MobHeadTile::isHeadBlock(a3.tileClass->blockID)) {
		int v12 = a3.tileClass->getRenderShape();
		if(TileRenderer::canRender(v12) && a3.tileClass->getRenderShape() != 22) {
			Tesselator::instance.begin(0);
			Tesselator::instance.voidBeginAndEndCalls(1);
			this->tileRenderer.renderTile(a3.tileClass, a3.getAuxValue(), 1);
			Tesselator::instance.voidBeginAndEndCalls(0);
			v7->field_4 = Tesselator::instance.end();
			v7->field_2C = "terrain-atlas.tga";
			v7->field_30 = 0;
			int v24 = a3.tileClass->getRenderLayer();
			bool_t isLight = (a3.itemClass ? a3.itemClass->isEmissive(a3.getAuxValue()) : 0) || a3.getId() == 50 || a3.getId() == 89 || a3.getId() == 91 || a3.getId() == 327;
			if(v24 != 1) {
				if(!a3.tileClass->goodGraphics) {
					v7->field_31 = 0;
					v7->field_32 = isLight;
					return v7;
				}
				if(v24 != 3) {
					v7->field_31 = 0;
					v7->field_32 = isLight;
					return v7;
				}
			}
			v7->field_31 = 1;
			v7->field_32 = isLight;
			return v7;
		}
	}
	int v25 = a3.getId();
	v7->field_30 = 1;
	v7->field_0 = v25;
	if((a3.tileClass && (Tile::enderChest ? a3.tileClass != Tile::enderChest : true)) || !a3.itemClass) {
		v7->field_2C = "terrain-atlas.tga";
	} else {
		v7->field_2C = a3.itemClass->itemTexture;
	}
	TextureUVCoordinateSet* icon;
	if(a2) {
		icon = a2->getItemInHandIcon(&a3, 0);
	} else {
		icon = a3.getIcon(0, 0);
	}

	if (!icon) return v7;

	float width = icon->width;
	float v16 = width * icon->minX;
	int v17 = (int)(float)(width * icon->maxX);
	float height = icon->height;
	float v19 = height * icon->minY;
	int v20 = (int)(float)(height * icon->maxY);
	TextureData* td = this->minecraft->texturesPtr->loadAndGetTextureData(v7->field_2C);
	if (!td || !td->pixels) return v7;

	Color4 colorTint = Color4::WHITE;
	int32_t col = -1;
	int32_t id = a3.getId();
	int32_t aux = a3.getAuxValue();
	if (id == 31 || (Tile::tallgrass && (id == Tile::tallgrass->blockID || (a3.tileClass && a3.tileClass == Tile::tallgrass)))) {
		col = (aux == 2) ? 0x5B8F32 : 0x66A538;
	} else if (id == 106 || (Tile::vine && (id == Tile::vine->blockID || (a3.tileClass && a3.tileClass == Tile::vine)))) {
		col = 0x30BB0B;
	} else if (id == 111 || (Tile::waterLily && (id == Tile::waterLily->blockID || (a3.tileClass && a3.tileClass == Tile::waterLily)))) {
		col = 0x529141;
	} else if (id == 175 || (Tile::doublePlant && (id == Tile::doublePlant->blockID || (a3.tileClass && a3.tileClass == Tile::doublePlant)))) {
		int32_t sub = aux & 7;
		col = (sub == 1) ? 0x5B8F32 : ((sub == 0) ? 0x66A538 : -1);
	} else if (id == 18 || (Tile::leaves && (id == Tile::leaves->blockID || (a3.tileClass && a3.tileClass == Tile::leaves)))) {
		int32_t v5 = aux & 3;
		if (v5 == 1) col = 0x619961;
		else if (v5 == 2) col = 0x80A755;
		else col = 0x48B518;
	} else if (a3.tileClass) {
		col = a3.tileClass->getColor(aux);
	}
	if(col != -1 && (col & 0xFFFFFF) != 0xFFFFFF) {
		float r = (float)((col >> 16) & 0xFF) / 255.0f;
		float g = (float)((col >> 8) & 0xFF) / 255.0f;
		float b = (float)(col & 0xFF) / 255.0f;
		colorTint = Color4(r, g, b, 1.0f);
	}

	v7->colorR = colorTint.r;
	v7->colorG = colorTint.g;
	v7->colorB = colorTint.b;

	TextureTesselator textes(td, (int)v16, (int)v19, v17, v20, Vec3(0, 0, 0), Color4::BLACK, colorTint);
	v7->field_4 = textes.tesselate();
	v7->field_32 = (a3.itemClass ? a3.itemClass->isEmissive(a3.getAuxValue()) : 0) || a3.getId() == 50 || a3.getId() == 89 || a3.getId() == 91 || a3.getId() == 327;
	return v7;
}
void ItemInHandRenderer::renderFirstPersonLegs(float a2) {
	if (!this->minecraft->player) return;
	Player* p = this->minecraft->player;
	if (p->isSleeping() || p->isRiding() || p->isInWater() || p->isUnderLiquid(Material::water)) return;

	float pitch = p->prevPitch + (p->pitch - p->prevPitch) * a2;
	if (pitch < 18.0f) return;

	EntityRenderer* r = EntityRenderDispatcher::getInstance()->getRenderer(p);
	if (!r) return;
	HumanoidModel* hm = ((HumanoidMobRenderer*)r)->hmodel;
	if (!hm) return;

	float walkSpeed = sqrtf(p->motionX * p->motionX + p->motionZ * p->motionZ);
	bool isMoving = walkSpeed > 0.015f || p->moveForward != 0.0f || p->moveStrafe != 0.0f || (p->abilities.flying && !p->onGround);

	bool isSprinting = this->minecraft->player && this->minecraft->player->isSprinting;
	static float s_firstPersonLegTick = 0.0f;
	if (isMoving) {
		s_firstPersonLegTick += (isSprinting ? 0.30f : 0.20f);
	}

	float legSwing = isMoving ? sinf(s_firstPersonLegTick) * (isSprinting ? 0.75f : 0.55f) : 0.0f;
	if (p->abilities.flying && !p->onGround) {
		legSwing = sinf(s_firstPersonLegTick * 0.6f) * 0.35f;
	}

	glPushMatrix();
	PlayerRenderer::updateSkin(p);
	this->minecraft->texturesPtr->loadAndBindTexture(!p->skin.empty() ? p->skin : "mob/char.png");

	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glRotatef(-pitch, 1.0f, 0.0f, 0.0f);
	glTranslatef(0.0f, -1.35f, 0.2f);
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	glScalef(0.85f, 0.85f, 0.85f);

	// Right leg
	glPushMatrix();
	glTranslatef(0.125f, 0.0f, 0.0f);
	glRotatef(legSwing * 40.0f, 1.0f, 0.0f, 0.0f);
	hm->rightLegModel.xRotAngle = 0.0f;
	hm->rightLegModel.yRotAngle = 0.0f;
	hm->rightLegModel.zRotAngle = 0.0f;
	hm->rightLegModel.render(0.0625f);
	glPopMatrix();

	// Left leg
	glPushMatrix();
	glTranslatef(-0.125f, 0.0f, 0.0f);
	glRotatef(-legSwing * 40.0f, 1.0f, 0.0f, 0.0f);
	hm->leftLegModel.xRotAngle = 0.0f;
	hm->leftLegModel.yRotAngle = 0.0f;
	hm->leftLegModel.zRotAngle = 0.0f;
	hm->leftLegModel.render(0.0625f);
	glPopMatrix();

	glPopMatrix();
}

void ItemInHandRenderer::renderSwimmingArms(float a2) {
	if (!this->minecraft->player) return;
	Player* p = this->minecraft->player;

	EntityRenderer* r = EntityRenderDispatcher::getInstance()->getRenderer(p);
	if (!r) return;
	HumanoidModel* hm = ((HumanoidMobRenderer*)r)->hmodel;
	if (!hm) return;

	bool isSprinting = this->minecraft->player && this->minecraft->player->isSprinting;
	static float s_swimPaddle = 0.0f;
	s_swimPaddle += (isSprinting ? 0.22f : 0.15f);

	float strokeL = sinf(s_swimPaddle);
	float strokeR = sinf(s_swimPaddle + 3.14159265f);

	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	if (this->field_4.itemClass) {
		// Render held item in right hand with subtle paddle motion
		glPushMatrix();
		glTranslatef(0.56f, -0.52f + strokeR * 0.08f, -0.72f);
		glRotatef(45.0f - strokeR * 10.0f, 0.0f, 1.0f, 0.0f);
		glScalef(0.4f, 0.4f, 0.4f);
		this->renderItem(p, &this->field_4);
		glPopMatrix();
	} else {
		// Right arm paddling
		glPushMatrix();
		PlayerRenderer::updateSkin(p);
		this->minecraft->texturesPtr->loadAndBindTexture(!p->skin.empty() ? p->skin : "mob/char.png");
		glTranslatef(0.64f, -0.6f + strokeR * 0.15f, -0.72f + cosf(s_swimPaddle + 3.14159265f) * 0.15f);
		glRotatef(45.0f - strokeR * 25.0f, 0.0f, 1.0f, 0.0f);
		glRotatef(20.0f, 0.0f, 0.0f, 1.0f);
		glTranslatef(-1.0f, 3.6f, 3.5f);
		glRotatef(120.0f, 0.0f, 0.0f, 1.0f);
		glRotatef(200.0f, 1.0f, 0.0f, 0.0f);
		glRotatef(-135.0f, 0.0f, 1.0f, 0.0f);
		glScalef(1.0f, 1.0f, 1.0f);
		glTranslatef(5.6f, 0.0f, 0.0f);
		hm->rightArmModel.xRotAngle = 0.0f;
		hm->rightArmModel.yRotAngle = 0.0f;
		hm->rightArmModel.zRotAngle = 0.0f;
		hm->rightArmModel.render(0.0625f);
		glPopMatrix();
	}

	// Left arm paddling
	glPushMatrix();
	PlayerRenderer::updateSkin(p);
	this->minecraft->texturesPtr->loadAndBindTexture(!p->skin.empty() ? p->skin : "mob/char.png");
	glTranslatef(-0.64f, -0.6f + strokeL * 0.15f, -0.72f + cosf(s_swimPaddle) * 0.15f);
	glRotatef(-45.0f + strokeL * 25.0f, 0.0f, 1.0f, 0.0f);
	glRotatef(-20.0f, 0.0f, 0.0f, 1.0f);
	glTranslatef(1.0f, 3.6f, 3.5f);
	glRotatef(120.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(200.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(135.0f, 0.0f, 1.0f, 0.0f);
	glScalef(1.0f, 1.0f, 1.0f);
	glTranslatef(-5.6f, 0.0f, 0.0f);
	hm->leftArmModel.xRotAngle = 0.0f;
	hm->leftArmModel.yRotAngle = 0.0f;
	hm->leftArmModel.zRotAngle = 0.0f;
	hm->leftArmModel.render(0.0625f);
	glPopMatrix();
}

void ItemInHandRenderer::render(float a2) {
	if (!this->minecraft->options.thirdPerson && this->minecraft->player) {
		Player* p = this->minecraft->player;
		if (p->isInWater() || p->isUnderLiquid(Material::water)) {
			float hSpeed = sqrtf(p->motionX * p->motionX + p->motionZ * p->motionZ);
			bool isMovingInWater = (p->moveForward != 0.0f || p->moveStrafe != 0.0f || p->isJumping) && (hSpeed > 0.01f || fabsf(p->motionY) > 0.01f);
			if (isMovingInWater) {
				this->renderSwimmingArms(a2);
				return;
			}
		}
	}
	Mob* player;			  // r4
	float v6;				  // s23
	int v7;					  // r0
	bool_t v8;				  // r8
	float v9;				  // s16
	float v10;				  // s16
	float v11;				  // s25
	float v12;				  // s19
	float v13;				  // s17
	float v14;				  // s19
	float v15;				  // s12
	float v16;				  // s15
	float v17;				  // s16
	float v18;				  // s11
	float v19;				  // s15
	float v20;				  // r0
	float v21;				  // s17
	int maxUseDuration;		  // r6
	float v23;				  // s18
	float v24;				  // s17
	float v25;				  // r0
	float TimeS;			  // s15
	float v27;				  // s13
	float v28;				  // s25
	double v29;				  // d8
	float v30;				  // s18
	float v31;				  // s26
	float v32;				  // s25
	float v33;				  // s18
	float v34;				  // s15
	float v35;				  // r0
	EntityRenderer* renderer; // r4
	float v39;				  // s19

	glPushMatrix();
	player = (Mob*)this->minecraft->player;
	static float s_prevHandSneak = 0.0f;
	static float s_handSneak = 0.0f;
	float handInterp = s_prevHandSneak + (s_handSneak - s_prevHandSneak) * a2;
	if (handInterp > 0.0001f) {
		glTranslatef(0.0f, -0.06f * handInterp, 0.04f * handInterp);
	}
	v6 = this->field_20 + (float)((float)(this->field_1C - this->field_20) * a2);
	static int32_t s_digAnimTick = 0;
	if(!this->field_4.itemClass) {
		v28 = this->minecraft->player->getAttackAnim(a2);
		if(v28 <= 0.001f && this->minecraft->gameMode && this->minecraft->gameMode->field_8 > 0.0f) {
			s_digAnimTick++;
			v28 = fmodf((float)s_digAnimTick + a2, 8.0f) / 8.0f;
		}
		v29 = v28;
		v30 = sqrt(v28);
		v31 = Mth::sin((float)(v28 * v28) * 3.1416);
		v32 = Mth::sin(v28 * 3.1416);
		v33 = Mth::sin(v30 * 3.1416);
		v34 = sqrt(v29);
		v35 = Mth::sin((float)(v34 * 3.1416) + (float)(v34 * 3.1416));
		glTranslatef(-(float)(v33 * 0.3), v35 * 0.4, -(float)(v32 * 0.4));
		glTranslatef(0.64, -0.6 - (float)((float)(1.0 - v6) * 0.6), -0.72);
		glRotatef(45.0, 0.0, 1.0, 0.0);
		glRotatef(v33 * 70.0, 0.0, 1.0, 0.0);
		glRotatef(-(float)(v31 * 20.0), 0.0, 0.0, 1.0);
		PlayerRenderer::updateSkin(this->minecraft->player);
		this->minecraft->texturesPtr->loadAndBindTexture(!this->minecraft->player->skin.empty() ? this->minecraft->player->skin : "mob/char.png");
		glTranslatef(-1.0, 3.6, 3.5);
		glRotatef(120.0, 0.0, 0.0, 1.0);
		glRotatef(200.0, 1.0, 0.0, 0.0);
		glRotatef(-135.0, 0.0, 1.0, 0.0);
		glScalef(1.0, 1.0, 1.0);
		glTranslatef(5.6, 0.0, 0.0);
		if (this->minecraft->player) {
			this->minecraft->player->setupLighting(this->minecraft->options.graphics, a2);
		}
		renderer = EntityRenderDispatcher::getInstance()->getRenderer(this->minecraft->player);
		glScalef(1.0, 1.0, 1.0);
		if (renderer) {
			((HumanoidMobRenderer*)renderer)->renderHand();
		}
		goto LABEL_28;
	}
	v7 = this->minecraft->player->getUseItemDuration();
	v8 = v7 > 0;
	if(Item::bow == this->field_4.itemClass && Item::bow) {
		if(v7 <= 0) {
LABEL_29:
			v39 = player->getAttackAnim(a2);
			if(v39 <= 0.001f && this->minecraft->gameMode && this->minecraft->gameMode->field_8 > 0.0f) {
				s_digAnimTick++;
				v39 = fmodf((float)s_digAnimTick + a2, 8.0f) / 8.0f;
			}
			goto LABEL_7;
		}
	} else if(v7 <= 0 || !this->field_4.getUseAnimation()) {
		goto LABEL_29;
	}
	v39 = 0.0;
LABEL_7:
	v9 = sqrt(v39);
	v10 = v9 * 3.1416;
	v11 = Mth::sin((float)(v39 * v39) * 3.1416);
	v12 = Mth::sin(v39 * 3.1416);
	v13 = Mth::sin(v10);
	if(v8) {
		if((unsigned int)(this->field_4.getUseAnimation() - 1) <= 1) {
			v14 = (float)((float)player->getUseItemDuration() - a2) + 1.0;
			v15 = 1.0 - (float)(v14 / (float)this->field_4.getMaxUseDuration());
			v16 = (float)((float)(1.0 - v15) * (float)(1.0 - v15)) * (float)(1.0 - v15);
			v17 = 1.0 - (float)((float)((float)((float)(v16 * v16) * v16) * (float)((float)(v16 * v16) * v16)) * (float)((float)(v16 * v16) * v16));
			v18 = fabsf(Mth::cos((float)((float)(v14 * 0.25) * 3.1416)) * 0.1);
			if(v15 > 0.2) {
				v19 = 1.0;
			} else {
				v19 = 0.0;
			}
			glTranslatef(0.0, v18 * v19, 0.0);
			glTranslatef(v17 * 0.6, -(float)(v17 * 0.5), 0.0);
			glRotatef(v17 * 90.0, 0.0, 1.0, 0.0);
			glRotatef(v17 * 10.0, 1.0, 0.0, 0.0);
			glRotatef(v17 * 30.0, 0.0, 0.0, 1.0);
		}
	} else {
		v20 = Mth::sin(v10 + v10);
		glTranslatef(-(float)(v13 * 0.4), v20 * 0.2, -(float)(v12 * 0.2));
	}
	glTranslatef(0.56, -0.52 - (float)((float)(1.0 - v6) * 0.6), -0.72);
	glRotatef(45.0, 0.0, 1.0, 0.0);
	v21 = -v13;
	glRotatef(-(float)(v11 * 20.0), 0.0, 1.0, 0.0);
	glRotatef(v21 * 20.0, 0.0, 0.0, 1.0);
	glRotatef(v21 * 80.0, 1.0, 0.0, 0.0);
	glScalef(0.4, 0.4, 0.4);
	if(player->getUseItemDuration() <= 0 || this->field_4.getUseAnimation() != 4) {
		goto LABEL_22;
	}
	glRotatef(-18.0, 0.0, 0.0, 1.0);
	glRotatef(-12.0, 0.0, 1.0, 0.0);
	glRotatef(-8.0, 1.0, 0.0, 0.0);
	glTranslatef(-0.9, 0.2, 0.0);
	maxUseDuration = this->field_4.getMaxUseDuration();
	v23 = (float)maxUseDuration - (float)((float)((float)player->getUseItemDuration() - a2) + 1.0);
	v24 = (float)((float)((float)(v23 / 20.0) + (float)(v23 / 20.0)) + (float)((float)(v23 / 20.0) * (float)(v23 / 20.0))) / 3.0;
	if(v24 > 1.0) {
		v24 = 1.0;
LABEL_20:
		v25 = Mth::sin((float)(v23 - 0.1) * 1.3);
		glTranslatef(0.0, (float)(v25 * 0.01) * (float)(v24 - 0.1), 0.0);
		goto LABEL_21;
	}
	if(v24 > 0.1) {
		goto LABEL_20;
	}
LABEL_21:
	glTranslatef(0.0, 0.0, v24 * 0.1);
	glRotatef(-335.0, 0.0, 0.0, 1.0);
	glRotatef(-50.0, 0.0, 1.0, 0.0);
	glTranslatef(0.0, 0.5, 0.0);
	glScalef(1.0, 1.0, (float)(v24 * 0.2) + 1.0);
	glTranslatef(0.0, -0.5, 0.0);
	glRotatef(50.0, 0.0, 1.0, 0.0);
	glRotatef(335.0, 0.0, 0.0, 1.0);
LABEL_22:
	if(this->field_4.itemClass && this->field_4.itemClass->isMirroredArt()) {
		glRotatef(180.0, 0.0, 1.0, 0.0);
	}
	if(this->minecraft->options.viewBobbing) {
		TimeS = getTimeS();
		v27 = sinf(TimeS + TimeS);
		glTranslatef(0.0, v27 * 0.011, 0.0);
		glRotatef((float)((float)(v27 * 0.011) * 360.0) * 0.075, 1.0, 0.0, 0.0);
	}
	{
		int heldId = this->field_4.getId();
		if (heldId == 50 || heldId == 89 || heldId == 91 || heldId == 327) {
			GLfloat lpos[] = { 0.2f, 0.2f, -0.4f, 1.0f };
			GLfloat ldiff[] = { 1.2f, heldId == 89 ? 1.15f : 0.95f, heldId == 89 ? 0.95f : 0.6f, 1.0f };
			GLfloat lamb[] = { 0.8f, heldId == 89 ? 0.75f : 0.65f, heldId == 89 ? 0.65f : 0.4f, 1.0f };
			glLightfv(0x4000, GL_POSITION, lpos);
			glLightfv(0x4000, GL_DIFFUSE, ldiff);
			glLightfv(0x4000, GL_AMBIENT, lamb);
		}
	}
	this->renderItem(player, &this->field_4);
LABEL_28:
	glPopMatrix();
}
void ItemInHandRenderer::renderFire(float a2) {
	int32_t v2;	  // r6
	float minX;	  // r9
	float maxX;	  // r8
	float minY;	  // r5
	float maxY;	  // r10
	int32_t v7;	  // r3
	float width;  // [sp+18h] [bp-30h]
	float height; // [sp+1Ch] [bp-2Ch]

	glColor4f(1.0, 1.0, 1.0, 0.9);
	v2 = 1;
	do {
		glPushMatrix();
		minX = Tile::fire->textureUV.minX;
		maxX = Tile::fire->textureUV.maxX;
		width = Tile::fire->textureUV.width;
		height = Tile::fire->textureUV.height;
		minY = Tile::fire->textureUV.minY;
		maxY = Tile::fire->textureUV.maxY;
		glTranslatef((float)v2 * 0.24, -0.3, 0.0);
		v7 = -v2;
		v2 -= 2;
		glRotatef((float)v7 * 10.0, 0.0, 1.0, 0.0);
		Tesselator::instance.begin(4);
		Tesselator::instance.vertexUV(-0.5, -0.5, -0.5, maxX, maxY);
		Tesselator::instance.vertexUV(0.5, -0.5, -0.5, minX, maxY);
		Tesselator::instance.vertexUV(0.5, 0.5, -0.5, minX, minY);
		Tesselator::instance.vertexUV(-0.5, 0.5, -0.5, maxX, minY);
		Tesselator::instance.draw(1);
		glPopMatrix();
	} while(v2 != -3);
	glColor4f(1.0, 1.0, 1.0, 1.0);
}
void ItemInHandRenderer::renderItem(struct Mob* a2, ItemInstance* a3) {
	Tile* tileClass;	  // r4
	RenderCall* v7;		  // r4
	int graphics;		  // r7
	Minecraft* minecraft; // r3
	GLenum v12;			  // r0
	unsigned int frameId; // r0

	tileClass = a3->tileClass;
	if(tileClass && MobHeadTile::isHeadBlock(tileClass->blockID)) {
		int htype = MobHeadTile::getHeadType(tileClass->blockID);
		if(MobHeadRenderer::instance) {
			glPushMatrix();
			glTranslatef(0.5f, 0.25f, 0.5f);
			MobHeadRenderer::instance->renderHead(htype, 0.0f, 0.0f, 0.0f, 0.0f, 0.0625f);
			glPopMatrix();
		}
	} else if(tileClass && tileClass->getRenderShape() == 22) {
		EntityTileRenderer::instance->render(tileClass, a3->getAuxValue(), 1.0);
	} else {
		frameId = this->_getFrameID(a2, a3);
		auto&& p = this->field_90.find(frameId);
		if(p == this->field_90.end()) {
			this->rebuildItem(a2, *a3); //TODO check
			p = this->field_90.find(frameId);
		}
		v7 = p->second.get();
		glPushMatrix();
		if(v7->field_30) {
			glScalef(1.5, 1.5, 1.5);
			glRotatef(50.0, 0.0, 1.0, 0.0);
			glRotatef(335.0, 0.0, 0.0, 1.0);
			glTranslatef(0.075, -0.245, -0.1);
			glScalef(0.0625, 0.0625, 0.0625);
			glRotatef(90.0, 0.0, 0.0, 1.0);
			glRotatef(90.0, 1.0, 0.0, 0.0);
			glRotatef(-180.0, 1.0, 0.0, 0.0);
			glRotatef(-90.0, 0.0, 1.0, 0.0);
			glTranslatef(0.0, 0.0, -16.0);
		}
		this->minecraft->texturesPtr->loadAndBindTexture(v7->field_2C);

		{
			EnableState v8(v7->field_31 ? GL_ALPHA_TEST : 0);
			DisableState v18(v7->field_31 ? GL_CULL_FACE : 0);
			graphics = v7->field_31;
			if(v7->field_31) {
				minecraft = this->minecraft;
				graphics = minecraft->options.graphics;
				if(minecraft->options.graphics) {
					glLightModelf(0xB52u, 1.0);
					graphics = 1;
				}
			}
			if(v7->field_32 && this->minecraft->options.graphics) {
				v12 = 2896;
			} else {
				v12 = 0;
			}
			DisableState v19(v12);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_COLOR_MATERIAL);
			glColor4f(v7->colorR, v7->colorG, v7->colorB, 1.0f);
			v7->field_4.render();
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glDisable(GL_COLOR_MATERIAL);
			if(graphics) {
				glLightModelf(0xB52u, 0.0);
			}
			glDisable(GL_BLEND);
		}

		glPopMatrix();
	}
}
void ItemInHandRenderer::renderScreenEffect(float a2) {
	Minecraft* minecraft; // r2
	Player* player;		  // r3
	float v7;			  // s15
	int32_t v9;			  // r6
	bool_t v10;			  // fnf
	float v11;			  // s15
	int32_t v12;		  // r5
	bool_t v13;			  // fnf
	float v14;			  // s15
	int32_t v15;		  // r3
	Tile* v16;			  // r0
	int32_t v18;		  // [sp+4h] [bp-2Ch]

	if(this->minecraft->player->isOnFire()) {
		this->minecraft->texturesPtr->loadAndBindTexture("terrain-atlas.tga");
		this->renderFire(a2);
	}
	if(this->minecraft->player->isInWall()) {
		minecraft = this->minecraft;
		player = minecraft->player;
		v7 = player->posX;
		v9 = (int32_t)v7;
		v10 = v7 < (float)(int32_t)v7;
		v11 = player->posY;
		v12 = (int32_t)v11;
		if(v10) {
			--v9;
		}
		v13 = v11 < (float)(int32_t)v11;
		v14 = player->posZ;
		v15 = (int32_t)v14;
		if(v13) {
			--v12;
		}
		if(v14 < (float)(int32_t)v14) {
			--v15;
		}
		v18 = v15;
		minecraft->texturesPtr->loadAndBindTexture("terrain-atlas.tga");
		v16 = Tile::tiles[this->minecraft->level->getTile(v9, v12, v18)];
		if(v16) {
			this->renderTex(a2, *v16->getTexture(2));
		}
	}
}
void ItemInHandRenderer::renderTex(float a2, const TextureUVCoordinateSet& a3) {
	struct xy
	{
		float x, y, z;
	};
	DisableState v12(GL_BLEND);
	glDepthFunc(GL_ALWAYS);
	glColor4f(0.3, 0.3, 0.3, 0.5);
	float minX = a3.minX;
	float minY = a3.minY;
	float maxX = a3.maxX + 0.0;
	float maxY = a3.maxY + 0.0;

	Tesselator::instance.begin(16);
	for(auto&& p: {xy{-1.0f, -1.0f, 0.0f}, xy{0.0f, -1.0f, 0.0f}, xy{0.0f, 0.0f, 0.0f}, xy{-1.0f, 0.0f, 0.0f}}) {
		Tesselator::instance.vertexUV(p.x + 0.0, p.y + 0.0, -0.5, maxX, maxY);
		Tesselator::instance.vertexUV(p.x + 1.0, p.y + 0.0, -0.5, minX, maxY);
		Tesselator::instance.vertexUV(p.x + 1.0, p.y + 1.0, -0.5, minX, minY);
		Tesselator::instance.vertexUV(p.x + 0.0, p.y + 1.0, -0.5, maxX, minY);
	}
	Tesselator::instance.draw(1);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glDepthFunc(GL_LEQUAL);
}
void ItemInHandRenderer::renderWater(float a2) {
	float v3;			 // r0
	LocalPlayer* player; // r3
	float v5;			 // s18
	float v6;			 // s16
	float v7;			 // s17
	float v8;			 // s18
	float v9;			 // s16

	v3 = this->minecraft->player->getBrightness(a2);
	Biome* b = this->minecraft->level ? this->minecraft->level->getBiome((int)floorf(this->minecraft->player->posX), (int)floorf(this->minecraft->player->posZ)) : nullptr;
	if (b == Biome::swampland) {
		glColor4f(v3 * 0.35f, v3 * 0.55f, v3 * 0.22f, 0.65f);
	} else {
		glColor4f(v3 * 0.4f, v3 * 0.7f, v3, 0.5f);
	}
	glPushMatrix();
	player = this->minecraft->player;
	v5 = -(float)(player->yaw * 0.015625);
	v6 = player->pitch * 0.015625;
	Tesselator::instance.begin(4);
	v7 = v5 + 4.0;
	Tesselator::instance.vertexUV(-1.0, -1.0, -0.5, v5 + 4.0, v6 + 4.0);
	v8 = v5 + 0.0;
	Tesselator::instance.vertexUV(1.0, -1.0, -0.5, v8, v6 + 4.0);
	v9 = v6 + 0.0;
	Tesselator::instance.vertexUV(1.0, 1.0, -0.5, v8, v9);
	Tesselator::instance.vertexUV(-1.0, 1.0, -0.5, v7, v9);
	Tesselator::instance.draw(1);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glPopMatrix();
}
void ItemInHandRenderer::tick() {
	static float s_prevHandSneak = 0.0f;
	static float s_handSneak = 0.0f;
	s_prevHandSneak = s_handSneak;
	float targetHand = (this->minecraft && this->minecraft->player && this->minecraft->player->isSneaking()) ? 1.0f : 0.0f;
	s_handSneak += (targetHand - s_handSneak) * 0.35f;

	ItemInstance* sel; // r0
	float v11;		   // s14
	float v12;		   // s15

	this->field_20 = this->field_1C;
	sel = this->minecraft->player->inventory->getSelected();
	if(sel && sel->count > 0) {
		this->field_4 = *sel;
	} else {
		this->field_4 = ItemInstance();
	}
	v11 = this->field_1C;
	v12 = 1.0 - v11;
	if((float)(1.0 - v11) < -0.4) {
		v12 = -0.4;
	} else if(v12 > 0.4) {
		v12 = 0.4;
	}
	this->field_1C = v11 + v12;
}

ItemInHandRenderer::~ItemInHandRenderer() {
	AppPlatform::_singleton->removeListener(this);
}
void ItemInHandRenderer::onAppSuspended() {
	this->field_90.clear();
	//TODO do something with field_90: (i+8) = -1
}
