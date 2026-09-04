#include <rendering/model/OcelotModel.hpp>
#include <entity/Ocelot.hpp>
#include <math/Mth.hpp>
#include <cmath>

OcelotModel::OcelotModel()
	: head(0, 0, 64, 32)
	, body(20, 0, 64, 32)
	, leg0(8, 13, 64, 32)
	, leg1(8, 13, 64, 32)
	, leg2(40, 0, 64, 32)
	, leg3(40, 0, 64, 32)
	, tail1(0, 15, 64, 32)
	, tail2(4, 15, 64, 32) {
	this->head.setModel(this);
	this->head.addBox(-2.5f, -2.0f, -3.0f, 5, 4, 5);
	this->head.addBox(-1.5f, 0.0f, -4.0f, 3, 2, 2);
	this->head.addBox(-2.0f, -3.0f, 0.0f, 1, 1, 2);
	this->head.addBox(1.0f, -3.0f, 0.0f, 1, 1, 2);
	this->head.setPos(0.0f, 15.0f, -9.0f);

	this->body.setModel(this);
	this->body.addBox(-2.0f, 3.0f, -8.0f, 4, 16, 6);
	this->body.setPos(0.0f, 12.0f, -10.0f);

	this->tail1.setModel(this);
	this->tail1.addBox(-0.5f, 0.0f, 0.0f, 1, 8, 1);
	this->tail1.setPos(0.0f, 15.0f, 8.0f);

	this->tail2.setModel(this);
	this->tail2.addBox(-0.5f, 0.0f, 0.0f, 1, 8, 1);
	this->tail2.setPos(0.0f, 20.0f, 14.0f);

	this->leg0.setModel(this);
	this->leg0.addBox(-1.0f, 0.0f, 1.0f, 2, 6, 2);
	this->leg0.setPos(1.1f, 18.0f, 5.0f);

	this->leg1.setModel(this);
	this->leg1.addBox(-1.0f, 0.0f, 1.0f, 2, 6, 2);
	this->leg1.setPos(-1.1f, 18.0f, 5.0f);

	this->leg2.setModel(this);
	this->leg2.addBox(-1.0f, 0.0f, 0.0f, 2, 10, 2);
	this->leg2.setPos(1.2f, 13.8f, -5.0f);

	this->leg3.setModel(this);
	this->leg3.addBox(-1.0f, 0.0f, 0.0f, 2, 10, 2);
	this->leg3.setPos(-1.2f, 13.8f, -5.0f);
}

OcelotModel::~OcelotModel() {
}

void OcelotModel::render(Entity* entity, float a3, float a4, float a5, float a6, float a7, float a8) {
	this->setupAnim(a3, a4, a5, a6, a7, a8);
	this->head.render(a8);
	this->body.render(a8);
	this->leg0.render(a8);
	this->leg1.render(a8);
	this->leg2.render(a8);
	this->leg3.render(a8);
	this->tail1.render(a8);
	this->tail2.render(a8);
}

void OcelotModel::setupAnim(float a2, float a3, float a4, float a5, float a6, float a7) {
	this->head.xRotAngle = a6 / 57.29578f;
	this->head.yRotAngle = a5 / 57.29578f;

	this->body.xRotAngle = 1.5707964f;
	this->body.yRotAngle = 0.0f;
	this->body.zRotAngle = 0.0f;

	this->leg0.xRotAngle = Mth::cos(a2 * 0.6662f) * 1.0f * a3;
	this->leg1.xRotAngle = Mth::cos(a2 * 0.6662f + 3.14159f) * 1.0f * a3;
	this->leg2.xRotAngle = Mth::cos(a2 * 0.6662f + 3.14159f) * 1.0f * a3;
	this->leg3.xRotAngle = Mth::cos(a2 * 0.6662f) * 1.0f * a3;

	this->tail1.xRotAngle = 0.9f;
	this->tail2.xRotAngle = 1.1f;
}
