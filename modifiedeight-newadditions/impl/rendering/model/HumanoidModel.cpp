#include <level/LevelHeight.hpp>
#include <rendering/model/HumanoidModel.hpp>
#include <entity/Entity.hpp>
#include <entity/Mob.hpp>
#include <entity/Player.hpp>
#include <entity/LocalPlayer.hpp>
#include <math/Mth.hpp>
#include <level/Level.hpp>
#include <tile/material/Material.hpp>
#include <tile/Tile.hpp>
#include <cmath>
#include <math.h>

HumanoidModel::HumanoidModel(float a2, float a3)
	: headModel(0, 0, 64, 32)
	, bodyModel(16, 16, 64, 32)
	, rightArmModel(40, 16, 64, 32)
	, leftArmModel(40, 16, 64, 32)
	, rightLegModel(0, 16, 64, 32)
	, leftLegModel(0, 16, 64, 32) {
	this->field_0 = 0;
	this->field_14 = 1;
	this->field_4 = 0;
	this->field_5 = 0;
	AppPlatform::_singleton->addListener(this, 1);
	this->field_318 = 0;
	this->field_319 = 0;
	this->field_31A = 0;
	this->isUsingBow = 0;
	this->isSwimming = 0;
	this->headModel.setModel(this);
	this->bodyModel.setModel(this);
	this->rightArmModel.setModel(this);
	this->leftArmModel.setModel(this);
	this->rightLegModel.setModel(this);
	this->leftLegModel.setModel(this);
	this->headModel.addBox(-4.0, -8.0, -4.0, 8, 8, 8, a2);
	this->headModel.setPos(0.0, a3 + 0.0, 0.0);
	this->bodyModel.addBox(-4.0, 0.0, -2.0, 8, 12, 4, a2);
	this->bodyModel.setPos(0.0, a3 + 0.0, 0.0);
	this->rightArmModel.addBox(-3.0, -2.0, -2.0, 4, 12, 4, a2);
	this->rightArmModel.setPos(-5.0, a3 + 2.0, 0.0);
	this->leftArmModel.mirror = 1;
	this->leftArmModel.addBox(-1.0, -2.0, -2.0, 4, 12, 4, a2);
	this->leftArmModel.setPos(5.0, a3 + 2.0, 0.0);
	this->rightLegModel.addBox(-2.0, 0.0, -2.0, 4, 12, 4, a2);
	float v8 = a3 + 12.0;
	this->rightLegModel.setPos(-2.0, v8, 0.0);
	this->leftLegModel.mirror = 1;
	this->leftLegModel.addBox(-2.0, 0.0, -2.0, 4, 12, 4, a2);
	this->leftLegModel.setPos(2.0, v8, 0.0);
}

HumanoidModel::~HumanoidModel() {
}
void HumanoidModel::render(Entity* a2, float a3, float a4, float a5, float a6, float a7, float a8) {
	ItemInstance* item;

	this->curEntity = a2;
	if(a2) {
		this->field_4 = a2->isRiding();
		if(a2->isMob()) {
			Mob* m = (Mob*)a2;
			item = m->getCarriedItem();
			if(item) {
				if(m->getUseItemDuration() > 0 && item->getUseAnimation() == 4) {
					this->isUsingBow = 1;
				}
			}
			this->isSwimming = (m->entityHeight < 0.8f) || (m->isInWater() && !m->onGround && m->isUnderLiquid(Material::water));
		}
	}
	this->setupAnim(a3, a4, a5, a6, a7, a8);
	this->headModel.render(a8);
	this->bodyModel.render(a8);
	this->rightArmModel.render(a8);
	this->leftArmModel.render(a8);
	this->rightLegModel.render(a8);
	this->leftLegModel.render(a8);
	this->isUsingBow = 0;
	this->curEntity = nullptr;
}

void HumanoidModel::renderHorrible(float a2, float a3, float a4, float a5, float a6, float a7) {
	this->setupAnim(a2, a3, a4, a5, a6, a7);
	this->headModel.renderHorrible(a7);
	this->bodyModel.renderHorrible(a7);
	this->rightArmModel.renderHorrible(a7);
	this->leftArmModel.renderHorrible(a7);
	this->rightLegModel.renderHorrible(a7);
	this->leftLegModel.renderHorrible(a7);
}

void HumanoidModel::setupAnim(float a2, float a3, float a4, float a5, float a6, float a7) {
	this->headModel.yRotAngle = a5 / 57.29578f;
	this->headModel.xRotAngle = a6 / 57.29578f;
	this->headModel.zRotAngle = 0.0f;

	this->headModel.xOffset = 0.0f;
	this->headModel.yOffset = 0.0f;
	this->headModel.zOffset = 0.0f;
	this->bodyModel.xOffset = 0.0f;
	this->bodyModel.yOffset = 0.0f;
	this->bodyModel.zOffset = 0.0f;
	this->bodyModel.xRotAngle = 0.0f;
	this->bodyModel.yRotAngle = 0.0f;
	this->bodyModel.zRotAngle = 0.0f;
	this->rightArmModel.xOffset = -5.0f;
	this->rightArmModel.yOffset = 2.0f;
	this->rightArmModel.zOffset = 0.0f;
	this->leftArmModel.xOffset = 5.0f;
	this->leftArmModel.yOffset = 2.0f;
	this->leftArmModel.zOffset = 0.0f;
	this->rightLegModel.xOffset = -2.0f;
	this->rightLegModel.yOffset = 12.0f;
	this->rightLegModel.zOffset = 0.0f;
	this->leftLegModel.xOffset = 2.0f;
	this->leftLegModel.yOffset = 12.0f;
	this->leftLegModel.zOffset = 0.0f;

	float walkSpeed = 0.6662f;
	float limbSwing = a2 * walkSpeed;
	float limbSwingAmount = a3;
	float cosSwing = Mth::cos(limbSwing);
	float sinSwing = Mth::sin(limbSwing);

	this->leftArmModel.xRotAngle = cosSwing * limbSwingAmount;
	this->rightArmModel.xRotAngle = -cosSwing * limbSwingAmount;
	this->rightArmModel.yRotAngle = 0.0f;
	this->leftArmModel.yRotAngle = 0.0f;
	this->rightArmModel.zRotAngle = 0.0f;
	this->leftArmModel.zRotAngle = 0.0f;
	this->rightLegModel.yRotAngle = 0.0f;
	this->leftLegModel.yRotAngle = 0.0f;
	this->rightLegModel.zRotAngle = 0.0f;
	this->leftLegModel.zRotAngle = 0.0f;
	this->rightLegModel.xRotAngle = cosSwing * 1.35f * limbSwingAmount;
	this->leftLegModel.xRotAngle = -cosSwing * 1.35f * limbSwingAmount;

	if (limbSwingAmount < 0.05f && !this->isSwimming && !this->field_4) {
		float breath = Mth::sin(a4 * 0.08f);
		float breathHead = Mth::sin(a4 * 0.04f);
		this->bodyModel.yOffset += breath * 0.2f;
		this->headModel.yOffset += breath * 0.2f;
		this->rightArmModel.zRotAngle += Mth::cos(a4 * 0.08f) * 0.035f + 0.05f;
		this->leftArmModel.zRotAngle -= Mth::cos(a4 * 0.08f) * 0.035f + 0.05f;
		this->headModel.zRotAngle += breathHead * 0.015f;
	} else if (limbSwingAmount >= 0.05f && !this->isSwimming && !this->field_4) {
		float walkSway = sinSwing * limbSwingAmount * 0.055f;
		float walkBob = fabsf(sinSwing) * limbSwingAmount * 0.4f;
		this->bodyModel.xRotAngle += limbSwingAmount * 0.08f;
		this->bodyModel.zRotAngle += walkSway;
		this->bodyModel.yRotAngle += walkSway * 0.7f;
		this->headModel.zRotAngle += walkSway * 0.4f;
		this->bodyModel.yOffset += walkBob;
		this->headModel.yOffset += walkBob;
		this->rightArmModel.yOffset += walkBob;
		this->leftArmModel.yOffset += walkBob;
		this->rightLegModel.yOffset += walkBob;
		this->leftLegModel.yOffset += walkBob;
		this->rightArmModel.zRotAngle += Mth::cos(limbSwing) * limbSwingAmount * 0.055f + 0.05f;
		this->leftArmModel.zRotAngle -= Mth::cos(limbSwing) * limbSwingAmount * 0.055f + 0.05f;
	}

	if(this->field_4) {
		float v14 = -1.570796f;
		this->rightArmModel.xRotAngle = (-cosSwing * limbSwingAmount) + (v14 * 0.4f);
		this->rightLegModel.xRotAngle = v14 * 0.8f;
		this->leftLegModel.xRotAngle = v14 * 0.8f;
		this->rightLegModel.yRotAngle = 0.31416f;
		this->leftLegModel.yRotAngle = -0.31416f;
		this->leftArmModel.xRotAngle = (cosSwing * limbSwingAmount) + (v14 * 0.4f);
	}
	if(this->field_318) {
		this->leftArmModel.xRotAngle = (this->leftArmModel.xRotAngle * 0.5f) - 0.31416f;
	}
	if(this->field_319) {
		this->rightArmModel.xRotAngle = (this->rightArmModel.xRotAngle * 0.5f) - 0.31416f;
	}
	float v15 = this->field_0;
	if(v15 > -9990.0f) {
		float v16 = sqrtf(v15);
		float v17 = Mth::sin(v16 * 6.283185f) * 0.25f;
		this->bodyModel.yRotAngle = v17;
		this->rightArmModel.zOffset = Mth::sin(v17) * 5.0f;
		this->rightArmModel.xOffset = -(Mth::cos(v17) * 5.0f);
		this->leftArmModel.zOffset = -(Mth::sin(v17) * 5.0f);
		float v18 = Mth::cos(v17);
		float v19 = this->field_0;
		this->leftArmModel.yRotAngle += v17;
		this->leftArmModel.xRotAngle += v17;
		this->leftArmModel.xOffset = v18 * 5.0f;
		float v20 = v17 + this->rightArmModel.yRotAngle;
		this->rightArmModel.yRotAngle = v20;
		float v21 = Mth::sin((1.0f - ((1.0f - v19) * (1.0f - v19) * (1.0f - v19) * (1.0f - v19))) * 3.14159265f);
		float v22 = Mth::sin(v19 * 3.14159265f);
		float v23 = this->headModel.xRotAngle - 0.7f;
		this->rightArmModel.yRotAngle = v20 + (v17 * 2.0f);
		this->rightArmModel.xRotAngle -= (- (v23 * v22) * 0.75f) + (v21 * 1.2f);
		this->rightArmModel.zRotAngle = Mth::sin(v19 * 3.14159265f) * -0.4f;
		this->bodyModel.xRotAngle += v22 * 0.1f;
		this->headModel.yRotAngle -= v17 * 0.4f;
		this->leftArmModel.xRotAngle -= v22 * 0.3f;
	}
	if(this->field_31A) {
		float v24 = this->rightArmModel.xRotAngle + 0.4f;
		this->bodyModel.xRotAngle = 0.5f;
		this->rightLegModel.zOffset = 4.0f;
		this->leftLegModel.zOffset = 4.0f;
		this->rightLegModel.yOffset = 9.0f;
		this->leftLegModel.yOffset = 9.0f;
		this->headModel.yOffset = 1.0f;
		this->rightArmModel.xRotAngle = v24;
		this->leftArmModel.xRotAngle += 0.4f;
	}
	float v25 = (Mth::cos(a4 * 0.09f) * 0.05f) + 0.05f;
	float v26 = Mth::sin(a4 * 0.067f);
	this->rightArmModel.zRotAngle += v25;
	float v28 = v26 * 0.05f;
	this->leftArmModel.zRotAngle -= v25;
	this->rightArmModel.xRotAngle += v28;
	this->leftArmModel.xRotAngle -= v28;

	if(this->isUsingBow) {
		float yRotAngle = this->headModel.yRotAngle;
		this->rightArmModel.yRotAngle = yRotAngle - 0.1f;
		this->leftArmModel.yRotAngle = yRotAngle + 0.5f;
		float v30 = this->headModel.xRotAngle - 1.570796f;
		this->rightArmModel.zRotAngle = v25;
		this->leftArmModel.zRotAngle = -v25;
		this->rightArmModel.xRotAngle = v30 + v28;
		this->leftArmModel.xRotAngle = v30 - v28;
	}

	if(this->isSwimming) {
		this->bodyModel.xRotAngle = 1.5708f;
		this->bodyModel.yOffset = 19.0f;
		this->bodyModel.zOffset = 0.0f;
		this->bodyModel.yRotAngle = 0.0f;

		this->headModel.yOffset = 19.0f;
		this->headModel.zOffset = 0.0f;
		this->headModel.xRotAngle = (a6 / 57.29578f) * 0.4f;

		float swimArm = sinf(a4 * 0.35f);
		float swimArm2 = cosf(a4 * 0.35f);

		this->rightArmModel.xOffset = -5.0f;
		this->rightArmModel.yOffset = 20.0f;
		this->rightArmModel.zOffset = 0.0f;
		this->rightArmModel.xRotAngle = 1.5708f + swimArm * 0.6f;
		this->rightArmModel.yRotAngle = -0.2f;
		this->rightArmModel.zRotAngle = 0.35f + swimArm2 * 0.25f;

		this->leftArmModel.xOffset = 5.0f;
		this->leftArmModel.yOffset = 20.0f;
		this->leftArmModel.zOffset = 0.0f;
		this->leftArmModel.xRotAngle = 1.5708f - swimArm * 0.6f;
		this->leftArmModel.yRotAngle = 0.2f;
		this->leftArmModel.zRotAngle = -0.35f - swimArm2 * 0.25f;

		this->rightLegModel.xOffset = -2.0f;
		this->rightLegModel.yOffset = 19.0f;
		this->rightLegModel.zOffset = 12.0f;
		this->rightLegModel.xRotAngle = 1.5708f + sinf(a4 * 0.35f) * 0.35f;
		this->rightLegModel.zRotAngle = 0.05f;
		this->rightLegModel.yRotAngle = 0.0f;

		this->leftLegModel.xOffset = 2.0f;
		this->leftLegModel.yOffset = 19.0f;
		this->leftLegModel.zOffset = 12.0f;
		this->leftLegModel.xRotAngle = 1.5708f - sinf(a4 * 0.35f) * 0.35f;
		this->leftLegModel.zRotAngle = -0.05f;
		this->leftLegModel.yRotAngle = 0.0f;
	}
}
