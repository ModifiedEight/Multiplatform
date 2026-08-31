#include <rendering/model/SlimeModel.hpp>
#include <unigl.h>

SlimeModel::SlimeModel()
	: outerCube(0, 0, 64, 32)
	, innerCube(0, 16, 64, 32)
	, eye0(32, 0, 64, 32)
	, eye1(32, 4, 64, 32)
	, mouth(32, 8, 64, 32) {
	this->innerCube.setModel(this);
	this->innerCube.addBox(-3.0f, 17.0f, -3.0f, 6, 6, 6);

	this->eye0.setModel(this);
	this->eye0.addBox(-3.25f, 18.0f, -3.5f, 2, 2, 2);

	this->eye1.setModel(this);
	this->eye1.addBox(1.25f, 18.0f, -3.5f, 2, 2, 2);

	this->mouth.setModel(this);
	this->mouth.addBox(0.0f, 21.0f, -3.5f, 1, 1, 1);

	this->outerCube.setModel(this);
	this->outerCube.addBox(-4.0f, 16.0f, -4.0f, 8, 8, 8);
}

SlimeModel::~SlimeModel() {
}

void SlimeModel::render(Entity* a2, float a3, float a4, float a5, float a6, float a7, float a8) {
	this->innerCube.render(a8);
	this->eye0.render(a8);
	this->eye1.render(a8);
	this->mouth.render(a8);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	this->outerCube.render(a8);
	glDisable(GL_BLEND);
}
