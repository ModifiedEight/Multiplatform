#include <rendering/model/SalmonModel.hpp>
#include <math/Mth.hpp>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

SalmonModel::SalmonModel()
	: bodyFront(0, 0, 32, 32)
	, head(22, 0, 32, 32)
	, bodyBack(0, 13, 32, 32)
	, tail(20, 10, 32, 32)
	, finTop(2, 1, 32, 32)
	, finTopBack(0, 2, 32, 32)
	, finSide0(0, 0, 32, 32)
	, finSide1(2, 0, 32, 32) {
	this->bodyFront.setModel(this);
	this->bodyFront.addBox(-1.5f, -2.5f, 0.0f, 3, 5, 8);
	this->bodyFront.setPos(0.0f, 20.0f, 0.0f);

	this->head.setModel(this);
	this->head.addBox(-1.0f, -2.0f, -3.0f, 2, 4, 3);
	this->head.setPos(0.0f, 20.0f, 0.0f);

	this->bodyBack.setModel(this);
	this->bodyBack.addBox(-1.5f, -2.5f, 0.0f, 3, 5, 8);
	this->bodyBack.setPos(0.0f, 20.0f, 8.0f);

	this->tail.setModel(this);
	this->tail.addBox(0.0f, -2.5f, 0.0f, 0, 5, 6);
	this->tail.setPos(0.0f, 20.0f, 16.0f);

	this->finTop.setModel(this);
	this->finTop.addBox(0.0f, -2.0f, 0.0f, 0, 2, 3);
	this->finTop.setPos(0.0f, 18.0f, 5.0f);

	this->finTopBack.setModel(this);
	this->finTopBack.addBox(0.0f, -2.0f, 0.0f, 0, 2, 4);
	this->finTopBack.setPos(0.0f, 18.0f, 10.0f);

	this->finSide0.setModel(this);
	this->finSide0.addBox(-2.0f, 0.0f, 0.0f, 2, 0, 2);
	this->finSide0.setPos(-1.5f, 21.5f, 0.0f);
	this->finSide0.zRotAngle = -(float)(M_PI / 4.0);

	this->finSide1.setModel(this);
	this->finSide1.addBox(0.0f, 0.0f, 0.0f, 2, 0, 2);
	this->finSide1.setPos(1.5f, 21.5f, 0.0f);
	this->finSide1.zRotAngle = (float)(M_PI / 4.0);
}

SalmonModel::~SalmonModel() {
}

void SalmonModel::setupAnim(float a2, float a3, float a4, float a5, float a6, float a7) {
	float f = -0.15f * Mth::sin(0.25f * a4);
	this->bodyBack.yRotAngle = f;
	this->tail.yRotAngle = f * 1.5f;
}

void SalmonModel::render(Entity* a2, float a3, float a4, float a5, float a6, float a7, float a8) {
	this->setupAnim(a3, a4, a5, a6, a7, a8);
	this->bodyFront.render(a8);
	this->head.render(a8);
	this->bodyBack.render(a8);
	this->tail.render(a8);
	this->finTop.render(a8);
	this->finTopBack.render(a8);
	this->finSide0.render(a8);
	this->finSide1.render(a8);
}
