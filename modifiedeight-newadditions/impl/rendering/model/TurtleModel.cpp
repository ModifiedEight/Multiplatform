#include <rendering/model/TurtleModel.hpp>
#include <math/Mth.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

TurtleModel::TurtleModel()
	: head(3, 0, 128, 64)
	, body(0, 0, 128, 64)
	, leg0(1, 23, 128, 64)
	, leg1(1, 12, 128, 64)
	, leg2(27, 30, 128, 64)
	, leg3(27, 24, 128, 64) {
	this->head.setModel(this);
	this->head.addBox(-3.0f, -1.0f, -3.0f, 6, 5, 6);
	this->head.setPos(0.0f, 19.0f, -10.0f);

	this->body.setModel(this);
	this->body.texOffsetX = 7;
	this->body.texOffsetY = 37;
	this->body.addBox(-9.5f, 3.0f, -10.0f, 19, 20, 6);

	this->body.texOffsetX = 31;
	this->body.texOffsetY = 1;
	this->body.addBox(-5.5f, 3.0f, -13.0f, 11, 18, 3);
	this->body.setPos(0.0f, 11.0f, -10.0f);
	this->body.xRotAngle = (float)(M_PI / 2.0);

	this->leg0.setModel(this);
	this->leg0.addBox(-2.0f, 0.0f, 0.0f, 4, 1, 10);
	this->leg0.setPos(-3.5f, 22.0f, 11.0f);

	this->leg1.setModel(this);
	this->leg1.addBox(-2.0f, 0.0f, 0.0f, 4, 1, 10);
	this->leg1.setPos(3.5f, 22.0f, 11.0f);

	this->leg2.setModel(this);
	this->leg2.addBox(-13.0f, 0.0f, -2.0f, 13, 1, 5);
	this->leg2.setPos(-5.0f, 21.0f, -4.0f);

	this->leg3.setModel(this);
	this->leg3.addBox(0.0f, 0.0f, -2.0f, 13, 1, 5);
	this->leg3.setPos(5.0f, 21.0f, -4.0f);
}

TurtleModel::~TurtleModel() {
}

void TurtleModel::setupAnim(float a2, float a3, float a4, float a5, float a6, float a7) {
	this->head.xRotAngle = a6 * (float)(M_PI / 180.0);
	this->head.yRotAngle = a5 * (float)(M_PI / 180.0);
	this->leg0.xRotAngle = Mth::cos(a2 * 0.6662f * 0.6f) * 0.5f * a3;
	this->leg1.xRotAngle = Mth::cos(a2 * 0.6662f * 0.6f + (float)M_PI) * 0.5f * a3;
	this->leg2.zRotAngle = Mth::cos(a2 * 0.6662f * 0.6f + (float)M_PI) * 0.5f * a3;
	this->leg3.zRotAngle = Mth::cos(a2 * 0.6662f * 0.6f) * 0.5f * a3;
}

void TurtleModel::render(Entity* a2, float a3, float a4, float a5, float a6, float a7, float a8) {
	this->setupAnim(a3, a4, a5, a6, a7, a8);
	this->head.render(a8);
	this->body.render(a8);
	this->leg0.render(a8);
	this->leg1.render(a8);
	this->leg2.render(a8);
	this->leg3.render(a8);
}
