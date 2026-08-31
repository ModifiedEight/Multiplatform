#include <rendering/model/PolarBearModel.hpp>
#include <math/Mth.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

PolarBearModel::PolarBearModel()
	: head(0, 0, 128, 64)
	, body(0, 19, 128, 64)
	, leg0(50, 22, 128, 64)
	, leg1(50, 22, 128, 64)
	, leg2(50, 40, 128, 64)
	, leg3(50, 40, 128, 64) {
	this->head.setModel(this);
	this->head.addBox(-3.5f, -3.0f, -3.0f, 7, 7, 7);
	this->head.setPos(0.0f, 10.0f, -16.0f);

	this->head.texOffsetX = 0;
	this->head.texOffsetY = 44;
	this->head.addBox(-2.5f, 1.0f, -6.0f, 5, 3, 3);

	this->head.texOffsetX = 26;
	this->head.texOffsetY = 0;
	this->head.addBox(-4.5f, -4.0f, -1.0f, 2, 2, 1);
	this->head.addBox(2.5f, -4.0f, -1.0f, 2, 2, 1);

	this->body.setModel(this);
	this->body.texOffsetX = 0;
	this->body.texOffsetY = 19;
	this->body.addBox(-5.0f, -13.0f, -7.0f, 14, 14, 11);

	this->body.texOffsetX = 39;
	this->body.texOffsetY = 0;
	this->body.addBox(-4.0f, -25.0f, -7.0f, 12, 12, 10);
	this->body.setPos(-2.0f, 9.0f, 12.0f);
	this->body.xRotAngle = (float)(M_PI / 2.0);

	this->leg0.setModel(this);
	this->leg0.addBox(-2.0f, 0.0f, -2.0f, 4, 10, 8);
	this->leg0.setPos(-4.5f, 14.0f, 6.0f);

	this->leg1.setModel(this);
	this->leg1.addBox(-2.0f, 0.0f, -2.0f, 4, 10, 8);
	this->leg1.setPos(4.5f, 14.0f, 6.0f);

	this->leg2.setModel(this);
	this->leg2.addBox(-2.0f, 0.0f, -2.0f, 4, 10, 6);
	this->leg2.setPos(-3.5f, 14.0f, -8.0f);

	this->leg3.setModel(this);
	this->leg3.addBox(-2.0f, 0.0f, -2.0f, 4, 10, 6);
	this->leg3.setPos(3.5f, 14.0f, -8.0f);
}

PolarBearModel::~PolarBearModel() {
}

void PolarBearModel::setupAnim(float a2, float a3, float a4, float a5, float a6, float a7) {
	this->head.xRotAngle = a6 * (float)(M_PI / 180.0);
	this->head.yRotAngle = a5 * (float)(M_PI / 180.0);
	this->leg0.xRotAngle = Mth::cos(a2 * 0.6662f) * 1.4f * a3;
	this->leg1.xRotAngle = Mth::cos(a2 * 0.6662f + (float)M_PI) * 1.4f * a3;
	this->leg2.xRotAngle = Mth::cos(a2 * 0.6662f + (float)M_PI) * 1.4f * a3;
	this->leg3.xRotAngle = Mth::cos(a2 * 0.6662f) * 1.4f * a3;
}

void PolarBearModel::render(Entity* a2, float a3, float a4, float a5, float a6, float a7, float a8) {
	this->setupAnim(a3, a4, a5, a6, a7, a8);
	this->head.render(a8);
	this->body.render(a8);
	this->leg0.render(a8);
	this->leg1.render(a8);
	this->leg2.render(a8);
	this->leg3.render(a8);
}
