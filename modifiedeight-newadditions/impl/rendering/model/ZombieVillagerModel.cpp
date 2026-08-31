#include <rendering/model/ZombieVillagerModel.hpp>
#include <math/Mth.hpp>
#include <cmath>

ZombieVillagerModel::ZombieVillagerModel(float f)
	: HumanoidModel(f, 0.0f)
	, hatRim(30, 47, 64, 64)
	, nose(24, 0, 64, 64) {
	this->headModel = ModelPart(0, 0, 64, 64);
	this->headModel.addBox(-4.0f, -10.0f, -4.0f, 8, 10, 8, f);

	this->nose = ModelPart(24, 0, 64, 64);
	this->nose.setPos(0.0f, -2.0f, 0.0f);
	this->nose.addBox(-1.0f, -1.0f, -6.0f, 2, 4, 2, f);

	this->bodyModel = ModelPart(16, 20, 64, 64);
	this->bodyModel.addBox(-4.0f, 0.0f, -3.0f, 8, 12, 6, f);
	this->bodyModel.texOffs(0, 38);
	this->bodyModel.addBox(-4.0f, 0.0f, -3.0f, 8, 18, 6, f + 0.05f);

	this->rightArmModel = ModelPart(44, 22, 64, 64);
	this->rightArmModel.addBox(-3.0f, -2.0f, -2.0f, 4, 12, 4, f);
	this->rightArmModel.setPos(-5.0f, 2.0f, 0.0f);

	this->leftArmModel = ModelPart(44, 22, 64, 64);
	this->leftArmModel.mirror = 1;
	this->leftArmModel.addBox(-1.0f, -2.0f, -2.0f, 4, 12, 4, f);
	this->leftArmModel.setPos(5.0f, 2.0f, 0.0f);

	this->rightLegModel = ModelPart(0, 22, 64, 64);
	this->rightLegModel.setPos(-2.0f, 12.0f, 0.0f);
	this->rightLegModel.addBox(-2.0f, 0.0f, -2.0f, 4, 12, 4, f);

	this->leftLegModel = ModelPart(0, 22, 64, 64);
	this->leftLegModel.mirror = 1;
	this->leftLegModel.setPos(2.0f, 12.0f, 0.0f);
	this->leftLegModel.addBox(-2.0f, 0.0f, -2.0f, 4, 12, 4, f);
}

ZombieVillagerModel::~ZombieVillagerModel() {
}

void ZombieVillagerModel::setupAnim(float time, float r, float bob, float yRot, float xRot, float scale) {
	HumanoidModel::setupAnim(time, r, bob, yRot, xRot, scale);

	this->nose.yRotAngle = this->headModel.yRotAngle;
	this->nose.xRotAngle = this->headModel.xRotAngle;

	float n = -3.14159265f / 2.25f;
	this->rightArmModel.xRotAngle = n;
	this->leftArmModel.xRotAngle = n;
	this->rightArmModel.zRotAngle = 0.0f;
	this->leftArmModel.zRotAngle = 0.0f;
	this->rightArmModel.yRotAngle = -0.1f;
	this->leftArmModel.yRotAngle = 0.1f;
	this->rightArmModel.zRotAngle += (Mth::cos(bob * 0.09f) * 0.05f + 0.05f);
	this->leftArmModel.zRotAngle -= (Mth::cos(bob * 0.09f) * 0.05f + 0.05f);
	this->rightArmModel.xRotAngle += Mth::sin(bob * 0.067f) * 0.05f;
	this->leftArmModel.xRotAngle -= Mth::sin(bob * 0.067f) * 0.05f;
}

void ZombieVillagerModel::render(Entity* ent, float time, float r, float bob, float yRot, float xRot, float scale) {
	this->setupAnim(time, r, bob, yRot, xRot, scale);
	this->headModel.render(scale);
	this->nose.render(scale);
	this->bodyModel.render(scale);
	this->rightArmModel.render(scale);
	this->leftArmModel.render(scale);
	this->rightLegModel.render(scale);
	this->leftLegModel.render(scale);
}
