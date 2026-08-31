#include <rendering/model/PufferfishModel.hpp>
#include <math/Mth.hpp>

PufferfishModel::PufferfishModel()
	: cube(0, 27, 32, 32)
	, eye0(24, 6, 32, 32)
	, eye1(28, 6, 32, 32)
	, fin0(25, 0, 32, 32)
	, fin1(25, 0, 32, 32)
	, finBack(-3, 0, 32, 32) {
	this->cube.setModel(this);
	this->cube.addBox(-1.5f, -2.0f, -1.5f, 3, 2, 3);
	this->cube.setPos(0.0f, 23.0f, 0.0f);

	this->eye0.setModel(this);
	this->eye0.addBox(-1.5f, 0.0f, -1.5f, 1, 1, 1);
	this->eye0.setPos(0.0f, 20.0f, 0.0f);

	this->eye1.setModel(this);
	this->eye1.addBox(0.5f, 0.0f, -1.5f, 1, 1, 1);
	this->eye1.setPos(0.0f, 20.0f, 0.0f);

	this->finBack.setModel(this);
	this->finBack.addBox(-1.5f, 0.0f, 0.0f, 3, 0, 3);
	this->finBack.setPos(0.0f, 22.0f, 1.5f);

	this->fin0.setModel(this);
	this->fin0.addBox(-1.0f, 0.0f, 0.0f, 1, 0, 2);
	this->fin0.setPos(-1.5f, 22.0f, -1.5f);

	this->fin1.setModel(this);
	this->fin1.addBox(0.0f, 0.0f, 0.0f, 1, 0, 2);
	this->fin1.setPos(1.5f, 22.0f, -1.5f);
}

PufferfishModel::~PufferfishModel() {
}

void PufferfishModel::setupAnim(float a2, float a3, float a4, float a5, float a6, float a7) {
	this->fin0.zRotAngle = -0.2f + 0.4f * Mth::sin(a4 * 0.2f);
	this->fin1.zRotAngle = 0.2f - 0.4f * Mth::sin(a4 * 0.2f);
}

void PufferfishModel::render(Entity* a2, float a3, float a4, float a5, float a6, float a7, float a8) {
	this->setupAnim(a3, a4, a5, a6, a7, a8);
	this->cube.render(a8);
	this->eye0.render(a8);
	this->eye1.render(a8);
	this->finBack.render(a8);
	this->fin0.render(a8);
	this->fin1.render(a8);
}
