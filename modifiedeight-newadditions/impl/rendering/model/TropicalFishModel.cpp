#include <rendering/model/TropicalFishModel.hpp>
#include <math/Mth.hpp>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

TropicalFishModel::TropicalFishModel()
	: body(0, 0, 32, 32)
	, tail(22, -6, 32, 32)
	, leftFin(2, 16, 32, 32)
	, rightFin(2, 12, 32, 32)
	, topFin(10, -5, 32, 32) {
	this->body.setModel(this);
	this->body.addBox(-1.0f, -1.5f, -3.0f, 2, 3, 6);
	this->body.setPos(0.0f, 22.0f, 0.0f);

	this->tail.setModel(this);
	this->tail.addBox(0.0f, -1.5f, 0.0f, 0, 3, 6);
	this->tail.setPos(0.0f, 22.0f, 3.0f);

	this->leftFin.setModel(this);
	this->leftFin.addBox(-2.0f, -1.0f, 0.0f, 2, 2, 0);
	this->leftFin.setPos(-1.0f, 22.5f, 0.0f);
	this->leftFin.yRotAngle = (float)(M_PI / 4.0);

	this->rightFin.setModel(this);
	this->rightFin.addBox(0.0f, -1.0f, 0.0f, 2, 2, 0);
	this->rightFin.setPos(1.0f, 22.5f, 0.0f);
	this->rightFin.yRotAngle = -(float)(M_PI / 4.0);

	this->topFin.setModel(this);
	this->topFin.addBox(0.0f, -3.0f, 0.0f, 0, 3, 6);
	this->topFin.setPos(0.0f, 20.5f, -3.0f);
}

TropicalFishModel::~TropicalFishModel() {
}

void TropicalFishModel::setupAnim(float a2, float a3, float a4, float a5, float a6, float a7) {
	this->tail.yRotAngle = -0.2f * Mth::sin(0.25f * a4);
}

void TropicalFishModel::render(Entity* a2, float a3, float a4, float a5, float a6, float a7, float a8) {
	this->setupAnim(a3, a4, a5, a6, a7, a8);
	this->body.render(a8);
	this->tail.render(a8);
	this->leftFin.render(a8);
	this->rightFin.render(a8);
	this->topFin.render(a8);
}
