#include <rendering/model/FishModel.hpp>
#include <math/Mth.hpp>

FishModel::FishModel()
	: body(0, 0, 32, 32)
	, tail(22, -6, 32, 32)
	, finRight(2, 16, 32, 32)
	, finLeft(2, 16, 32, 32)
	, finTop(20, -6, 32, 32) {
	this->body.setModel(this);
	this->body.addBox(-1.0f, -2.0f, -3.0f, 2, 4, 6);
	this->body.setPos(0.0f, 20.0f, 0.0f);

	this->tail.setModel(this);
	this->tail.addBox(0.0f, -1.5f, 0.0f, 0, 3, 4);
	this->tail.setPos(0.0f, 20.0f, 3.0f);

	this->finRight.setModel(this);
	this->finRight.addBox(-2.0f, 0.0f, -1.0f, 2, 0, 2);
	this->finRight.setPos(-1.0f, 21.5f, -1.0f);
	this->finRight.zRotAngle = -0.7853982f;

	this->finLeft.setModel(this);
	this->finLeft.addBox(0.0f, 0.0f, -1.0f, 2, 0, 2);
	this->finLeft.setPos(1.0f, 21.5f, -1.0f);
	this->finLeft.zRotAngle = 0.7853982f;

	this->finTop.setModel(this);
	this->finTop.addBox(0.0f, -1.0f, -1.0f, 0, 1, 4);
	this->finTop.setPos(0.0f, 18.0f, -1.0f);
}

FishModel::~FishModel() {
}

void FishModel::setupAnim(float a2, float a3, float a4, float a5, float a6, float a7) {
	float f = 1.0f;
	if (a3 > 0.05f) {
		f = a3 * 1.5f;
	}
	this->tail.yRotAngle = -f * 0.45f * Mth::sin(0.6f * a4);
}

void FishModel::render(Entity* a2, float a3, float a4, float a5, float a6, float a7, float a8) {
	this->setupAnim(a3, a4, a5, a6, a7, a8);
	this->body.render(a8);
	this->tail.render(a8);
	this->finRight.render(a8);
	this->finLeft.render(a8);
	this->finTop.render(a8);
}
