#include <rendering/model/BoatModel.hpp>
#include <entity/Boat.hpp>
#include <cmath>

BoatModel::BoatModel()
	: Model() {
	this->cubes[0]._init(0, 0);
	this->cubes[0].setTexSize(128, 64);
	this->cubes[0].addBox(-14.0f, -9.0f, -3.0f, 28, 16, 3);
	this->cubes[0].setPos(0.0f, 3.0f, 1.0f);
	this->cubes[0].xRotAngle = 1.5707963f;

	this->cubes[1]._init(0, 19);
	this->cubes[1].setTexSize(128, 64);
	this->cubes[1].addBox(-13.0f, -7.0f, -1.0f, 18, 6, 2);
	this->cubes[1].setPos(-15.0f, 4.0f, 4.0f);
	this->cubes[1].yRotAngle = 4.712389f;

	this->cubes[2]._init(0, 27);
	this->cubes[2].setTexSize(128, 64);
	this->cubes[2].addBox(-8.0f, -7.0f, -1.0f, 16, 6, 2);
	this->cubes[2].setPos(15.0f, 4.0f, 0.0f);
	this->cubes[2].yRotAngle = 1.5707963f;

	this->cubes[3]._init(0, 35);
	this->cubes[3].setTexSize(128, 64);
	this->cubes[3].addBox(-14.0f, -7.0f, -1.0f, 28, 6, 2);
	this->cubes[3].setPos(0.0f, 4.0f, -9.0f);
	this->cubes[3].yRotAngle = 3.14159265f;

	this->cubes[4]._init(0, 43);
	this->cubes[4].setTexSize(128, 64);
	this->cubes[4].addBox(-14.0f, -7.0f, -1.0f, 28, 6, 2);
	this->cubes[4].setPos(0.0f, 4.0f, 9.0f);

	this->paddles[0]._init(62, 0);
	this->paddles[0].setTexSize(128, 64);
	this->paddles[0].addBox(-1.0f, 0.0f, -5.0f, 2, 2, 18);
	this->paddles[0].addBox(-1.001f, -3.0f, 8.0f, 1, 6, 7);
	this->paddles[0].setPos(3.0f, -5.0f, 9.0f);
	this->paddles[0].zRotAngle = 0.1963495f;

	this->paddles[1]._init(62, 20);
	this->paddles[1].setTexSize(128, 64);
	this->paddles[1].addBox(-1.0f, 0.0f, -5.0f, 2, 2, 18);
	this->paddles[1].addBox(0.001f, -3.0f, 8.0f, 1, 6, 7);
	this->paddles[1].setPos(3.0f, -5.0f, -9.0f);
	this->paddles[1].yRotAngle = 3.14159265f;
	this->paddles[1].zRotAngle = 0.1963495f;
}

BoatModel::~BoatModel() {
}

void BoatModel::render(Entity* entity, float a2, float a3, float a4, float a5, float a6, float scale) {
	for (int i = 0; i < 5; i++) {
		this->cubes[i].render(scale);
	}
	Boat* boat = (Boat*)entity;
	if (boat) {
		this->animatePaddle(boat, 0, scale);
		this->animatePaddle(boat, 1, scale);
	}
}

void BoatModel::animatePaddle(Boat* boat, int32_t i, float scale) {
	float h = boat->getRowingTime(i, 0.0f);
	float xRot = -0.5235988f + (-0.261799f - -0.5235988f) * ((std::sin(-h) + 1.0f) * 0.5f);
	float yRot = -0.7853982f + (0.7853982f - -0.7853982f) * ((std::sin(-h + 1.0f) + 1.0f) * 0.5f);
	if (i == 1) {
		yRot = 3.14159265f - yRot;
	}
	this->paddles[i].xRotAngle = xRot;
	this->paddles[i].yRotAngle = yRot;
	this->paddles[i].render(scale);
}
