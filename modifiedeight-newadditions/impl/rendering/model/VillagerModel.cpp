#include <rendering/model/VillagerModel.hpp>
#include <math/Mth.hpp>
#include <cmath>

VillagerModel::VillagerModel(float f)
	: head(0, 0, 64, 64)
	, hat(32, 0, 64, 64)
	, hatRim(30, 47, 64, 64)
	, body(16, 20, 64, 64)
	, jacket(0, 38, 64, 64)
	, arms(40, 38, 64, 64)
	, leg0(0, 22, 64, 64)
	, leg1(0, 22, 64, 64)
	, nose(24, 0, 64, 64) {
	this->head.setPos(0.0f, 0.0f, 0.0f);
	this->head.addBox(-4.0f, -10.0f, -4.0f, 8, 10, 8, f);

	this->nose.setPos(0.0f, -2.0f, 0.0f);
	this->nose.addBox(-1.0f, -1.0f, -6.0f, 2, 4, 2, f);

	this->body.setPos(0.0f, 0.0f, 0.0f);
	this->body.addBox(-4.0f, 0.0f, -3.0f, 8, 12, 6, f);

	this->jacket.setPos(0.0f, 0.0f, 0.0f);
	this->jacket.addBox(-4.0f, 0.0f, -3.0f, 8, 18, 6, f + 0.5f);

	this->arms.setPos(0.0f, 3.0f, -1.0f);
	this->arms.addBox(-4.0f, 2.0f, -2.0f, 8, 4, 4, f);
	this->arms.texOffs(44, 22);
	this->arms.addBox(-8.0f, -2.0f, -2.0f, 4, 8, 4, f);
	this->arms.addBox(4.0f, -2.0f, -2.0f, 4, 8, 4, f);

	this->leg0.setPos(-2.0f, 12.0f, 0.0f);
	this->leg0.addBox(-2.0f, 0.0f, -2.0f, 4, 12, 4, f);

	this->leg1.mirror = 1;
	this->leg1.setPos(2.0f, 12.0f, 0.0f);
	this->leg1.addBox(-2.0f, 0.0f, -2.0f, 4, 12, 4, f);
}

VillagerModel::~VillagerModel() {
}

void VillagerModel::setupAnim(float time, float r, float bob, float yRot, float xRot, float scale) {
	this->head.yRotAngle = yRot * (3.14159265f / 180.0f);
	this->head.xRotAngle = xRot * (3.14159265f / 180.0f);
	this->nose.yRotAngle = this->head.yRotAngle;
	this->nose.xRotAngle = this->head.xRotAngle;

	this->arms.setPos(0.0f, 3.0f, -1.0f);
	this->arms.xRotAngle = -0.75f;

	this->leg0.xRotAngle = Mth::cos(time * 0.6662f) * 1.4f * r * 0.5f;
	this->leg1.xRotAngle = Mth::cos(time * 0.6662f + 3.14159265f) * 1.4f * r * 0.5f;
	this->leg0.yRotAngle = 0.0f;
	this->leg1.yRotAngle = 0.0f;
}

void VillagerModel::render(Entity* ent, float time, float r, float bob, float yRot, float xRot, float scale) {
	this->setupAnim(time, r, bob, yRot, xRot, scale);
	this->head.render(scale);
	this->nose.render(scale);
	this->body.render(scale);
	this->jacket.render(scale);
	this->arms.render(scale);
	this->leg0.render(scale);
	this->leg1.render(scale);
}
