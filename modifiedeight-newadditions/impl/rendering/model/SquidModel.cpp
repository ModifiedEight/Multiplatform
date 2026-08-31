#include <rendering/model/SquidModel.hpp>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

SquidModel::SquidModel()
	: body(0, 0, 64, 32) {
	this->body.setModel(this);
	this->body.addBox(-6.0f, -8.0f, -6.0f, 12, 16, 12);
	this->body.yOffset += 8.0f;

	for (int32_t j = 0; j < 8; ++j) {
		this->tentacles[j] = new ModelPart(48, 0, 64, 32);
		this->tentacles[j]->setModel(this);
		double d = (double)j * M_PI * 2.0 / 8.0;
		float f = (float)std::cos(d) * 5.0f;
		float g = (float)std::sin(d) * 5.0f;
		this->tentacles[j]->addBox(-1.0f, 0.0f, -1.0f, 2, 18, 2);
		this->tentacles[j]->setPos(f, 15.0f, g);
		d = (double)j * M_PI * -2.0 / 8.0 + (M_PI / 2.0);
		this->tentacles[j]->yRotAngle = (float)d;
	}
}

SquidModel::~SquidModel() {
	for (int32_t j = 0; j < 8; ++j) {
		delete this->tentacles[j];
	}
}

void SquidModel::setupAnim(float a2, float a3, float a4, float a5, float a6, float a7) {
	for (int32_t i = 0; i < 8; ++i) {
		this->tentacles[i]->xRotAngle = a4;
	}
}

void SquidModel::render(Entity* a2, float a3, float a4, float a5, float a6, float a7, float a8) {
	this->setupAnim(a3, a4, a5, a6, a7, a8);
	this->body.render(a8);
	for (int32_t i = 0; i < 8; ++i) {
		this->tentacles[i]->render(a8);
	}
}
