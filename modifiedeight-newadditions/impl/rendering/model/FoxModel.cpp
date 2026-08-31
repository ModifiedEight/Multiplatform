#include <rendering/model/FoxModel.hpp>
#include <math/Mth.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

FoxModel::FoxModel()
	: head(1, 5, 48, 32)
	, earL(8, 1, 48, 32)
	, earR(15, 1, 48, 32)
	, nose(6, 18, 48, 32)
	, body(24, 15, 48, 32)
	, leg0(13, 24, 48, 32)
	, leg1(4, 24, 48, 32)
	, leg2(13, 24, 48, 32)
	, leg3(4, 24, 48, 32)
	, tail(30, 0, 48, 32) {
	this->head.setModel(this);
	this->head.addBox(-3.0f, -2.0f, -5.0f, 8, 6, 6);
	this->head.setPos(-1.0f, 16.5f, -3.0f);

	this->earL.setModel(this);
	this->earL.addBox(-3.0f, -4.0f, -4.0f, 2, 2, 1);
	this->head.addChild(&this->earL);

	this->earR.setModel(this);
	this->earR.addBox(3.0f, -4.0f, -4.0f, 2, 2, 1);
	this->head.addChild(&this->earR);

	this->nose.setModel(this);
	this->nose.addBox(-1.0f, 2.01f, -8.0f, 4, 2, 3);
	this->head.addChild(&this->nose);

	this->body.setModel(this);
	this->body.addBox(-3.0f, 3.999f, -3.5f, 6, 11, 6);
	this->body.setPos(0.0f, 16.0f, -6.0f);
	this->body.xRotAngle = (float)(M_PI / 2.0);

	this->leg0.setModel(this);
	this->leg0.addBox(2.0f, 0.5f, -1.0f, 2, 6, 2);
	this->leg0.setPos(-5.0f, 17.5f, 7.0f);

	this->leg1.setModel(this);
	this->leg1.addBox(2.0f, 0.5f, -1.0f, 2, 6, 2);
	this->leg1.setPos(-1.0f, 17.5f, 7.0f);

	this->leg2.setModel(this);
	this->leg2.addBox(2.0f, 0.5f, -1.0f, 2, 6, 2);
	this->leg2.setPos(-5.0f, 17.5f, 0.0f);

	this->leg3.setModel(this);
	this->leg3.addBox(2.0f, 0.5f, -1.0f, 2, 6, 2);
	this->leg3.setPos(-1.0f, 17.5f, 0.0f);

	this->tail.setModel(this);
	this->tail.addBox(2.0f, 0.0f, -1.0f, 4, 9, 5);
	this->tail.setPos(-4.0f, 15.0f, -1.0f);
	this->tail.xRotAngle = -0.05235988f;
	this->body.addChild(&this->tail);
}

FoxModel::~FoxModel() {
}

void FoxModel::setupAnim(float a2, float a3, float a4, float a5, float a6, float a7) {
	this->head.xRotAngle = a6 * (float)(M_PI / 180.0);
	this->head.yRotAngle = a5 * (float)(M_PI / 180.0);
	this->leg0.xRotAngle = Mth::cos(a2 * 0.6662f) * 1.4f * a3;
	this->leg1.xRotAngle = Mth::cos(a2 * 0.6662f + (float)M_PI) * 1.4f * a3;
	this->leg2.xRotAngle = Mth::cos(a2 * 0.6662f + (float)M_PI) * 1.4f * a3;
	this->leg3.xRotAngle = Mth::cos(a2 * 0.6662f) * 1.4f * a3;
}

void FoxModel::render(Entity* a2, float a3, float a4, float a5, float a6, float a7, float a8) {
	this->setupAnim(a3, a4, a5, a6, a7, a8);
	this->head.render(a8);
	this->body.render(a8);
	this->leg0.render(a8);
	this->leg1.render(a8);
	this->leg2.render(a8);
	this->leg3.render(a8);
}
