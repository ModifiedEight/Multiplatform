#include <rendering/model/WolfModel.hpp>
#include <math/Mth.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

WolfModel::WolfModel()
	: head(0, 0, 64, 32)
	, body(18, 14, 64, 32)
	, leg0(0, 18, 64, 32)
	, leg1(0, 18, 64, 32)
	, leg2(0, 18, 64, 32)
	, leg3(0, 18, 64, 32)
	, tail(9, 18, 64, 32)
	, mane(21, 0, 64, 32) {
	this->head.setModel(this);
	this->head.addBox(-2.0f, -3.0f, -2.0f, 6, 6, 4);
	this->head.setPos(-1.0f, 13.5f, -7.0f);

	this->head.texOffsetX = 16;
	this->head.texOffsetY = 14;
	this->head.addBox(-2.0f, -5.0f, 0.0f, 2, 2, 1);
	this->head.addBox(2.0f, -5.0f, 0.0f, 2, 2, 1);

	this->head.texOffsetX = 0;
	this->head.texOffsetY = 10;
	this->head.addBox(-0.5f, 0.0f, -5.0f, 3, 3, 4);

	this->body.setModel(this);
	this->body.addBox(-3.0f, -2.0f, -3.0f, 6, 9, 6);
	this->body.setPos(0.0f, 14.0f, 2.0f);
	this->body.xRotAngle = (float)(M_PI / 2.0);

	this->mane.setModel(this);
	this->mane.addBox(-3.0f, -3.0f, -3.0f, 8, 6, 7);
	this->mane.setPos(-1.0f, 14.0f, -3.0f);
	this->mane.xRotAngle = (float)(M_PI / 2.0);

	this->leg0.setModel(this);
	this->leg0.addBox(-1.0f, 0.0f, -1.0f, 2, 8, 2);
	this->leg0.setPos(-2.5f, 16.0f, 7.0f);

	this->leg1.setModel(this);
	this->leg1.addBox(-1.0f, 0.0f, -1.0f, 2, 8, 2);
	this->leg1.setPos(0.5f, 16.0f, 7.0f);

	this->leg2.setModel(this);
	this->leg2.addBox(-1.0f, 0.0f, -1.0f, 2, 8, 2);
	this->leg2.setPos(-2.5f, 16.0f, -4.0f);

	this->leg3.setModel(this);
	this->leg3.addBox(-1.0f, 0.0f, -1.0f, 2, 8, 2);
	this->leg3.setPos(0.5f, 16.0f, -4.0f);

	this->tail.setModel(this);
	this->tail.addBox(-1.0f, 0.0f, -1.0f, 2, 8, 2);
	this->tail.setPos(-1.0f, 12.0f, 8.0f);
	this->tail.xRotAngle = (float)(M_PI / 3.0);
}

WolfModel::~WolfModel() {
}

void WolfModel::setupAnim(float a2, float a3, float a4, float a5, float a6, float a7) {
	this->head.xRotAngle = a6 * (float)(M_PI / 180.0);
	this->head.yRotAngle = a5 * (float)(M_PI / 180.0);
	this->leg0.xRotAngle = Mth::cos(a2 * 0.6662f) * 1.4f * a3;
	this->leg1.xRotAngle = Mth::cos(a2 * 0.6662f + (float)M_PI) * 1.4f * a3;
	this->leg2.xRotAngle = Mth::cos(a2 * 0.6662f + (float)M_PI) * 1.4f * a3;
	this->leg3.xRotAngle = Mth::cos(a2 * 0.6662f) * 1.4f * a3;

	// Tail wagging & alert animation
	this->tail.yRotAngle = Mth::cos(a4 * 0.3f) * 0.25f;
	if (a3 > 0.001f) {
		float bodySway = Mth::sin(a2 * 0.6662f) * a3 * 0.06f;
		this->body.zRotAngle = bodySway;
		this->mane.zRotAngle = bodySway;
		this->head.zRotAngle = -bodySway * 0.5f;
	} else {
		this->body.zRotAngle = 0.0f;
		this->mane.zRotAngle = 0.0f;
		this->head.zRotAngle = 0.0f;
	}
}

void WolfModel::render(Entity* a2, float a3, float a4, float a5, float a6, float a7, float a8) {
	this->setupAnim(a3, a4, a5, a6, a7, a8);
	this->head.render(a8);
	this->body.render(a8);
	this->leg0.render(a8);
	this->leg1.render(a8);
	this->leg2.render(a8);
	this->leg3.render(a8);
	this->tail.render(a8);
	this->mane.render(a8);
}
