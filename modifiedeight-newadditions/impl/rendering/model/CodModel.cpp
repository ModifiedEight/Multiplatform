#include <rendering/model/CodModel.hpp>
#include <math/Mth.hpp>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

CodModel::CodModel()
	: body(0, 0, 32, 32)
	, head(11, 0, 32, 32)
	, nose(0, 0, 32, 32)
	, finSide0(22, 1, 32, 32)
	, finSide1(22, 4, 32, 32)
	, finBack(22, 3, 32, 32)
	, finTop(20, -6, 32, 32) {
	this->body.setModel(this);
	this->body.addBox(-1.0f, -2.0f, 0.0f, 2, 4, 7);
	this->body.setPos(0.0f, 22.0f, 0.0f);

	this->head.setModel(this);
	this->head.addBox(-1.0f, -2.0f, -3.0f, 2, 4, 3);
	this->head.setPos(0.0f, 22.0f, 0.0f);

	this->nose.setModel(this);
	this->nose.addBox(-1.0f, -2.0f, -1.0f, 2, 3, 1);
	this->nose.setPos(0.0f, 22.0f, -3.0f);

	this->finSide0.setModel(this);
	this->finSide0.addBox(-2.0f, 0.0f, -1.0f, 2, 0, 2);
	this->finSide0.setPos(-1.0f, 23.0f, 0.0f);
	this->finSide0.zRotAngle = -(float)(M_PI / 4.0);

	this->finSide1.setModel(this);
	this->finSide1.addBox(0.0f, 0.0f, -1.0f, 2, 0, 2);
	this->finSide1.setPos(1.0f, 23.0f, 0.0f);
	this->finSide1.zRotAngle = (float)(M_PI / 4.0);

	this->finBack.setModel(this);
	this->finBack.addBox(0.0f, -2.0f, 0.0f, 0, 4, 4);
	this->finBack.setPos(0.0f, 22.0f, 7.0f);

	this->finTop.setModel(this);
	this->finTop.addBox(0.0f, -1.0f, -1.0f, 0, 1, 6);
	this->finTop.setPos(0.0f, 20.0f, 0.0f);
}

CodModel::~CodModel() {
}

void CodModel::setupAnim(float a2, float a3, float a4, float a5, float a6, float a7) {
	this->finBack.yRotAngle = -0.25f * Mth::sin(0.25f * a4);
}

void CodModel::render(Entity* a2, float a3, float a4, float a5, float a6, float a7, float a8) {
	this->setupAnim(a3, a4, a5, a6, a7, a8);
	this->body.render(a8);
	this->head.render(a8);
	this->nose.render(a8);
	this->finSide0.render(a8);
	this->finSide1.render(a8);
	this->finBack.render(a8);
	this->finTop.render(a8);
}
