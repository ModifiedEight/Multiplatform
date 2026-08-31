#include <rendering/model/QuadrupedModel.hpp>
#include <math/Mth.hpp>

QuadrupedModel::QuadrupedModel(int32_t a2, float a3, int32_t a4, int32_t a5)
	: field_18(0, 0, a4, a5)
	, field_98(0, 0, 64, 32)
	, field_118(28, 8, a4, a5)
	, field_198(0, 16, a4, a5)
	, field_218(0, 16, a4, a5)
	, field_298(0, 16, a4, a5)
	, field_318(0, 16, a4, a5) {
	this->field_0 = 0;
	this->field_14 = 1;
	this->field_4 = 0;
	this->field_5 = 0;
	AppPlatform::_singleton->addListener(this, 1.0); //inlined
	this->field_398 = 8;
	this->field_39C = 4;
	this->field_18.setModel(this);
	this->field_118.setModel(this);
	this->field_198.setModel(this);
	this->field_218.setModel(this);
	this->field_298.setModel(this);
	this->field_318.setModel(this);
	this->field_18.addBox(-4.0, -4.0, -8.0, 8, 8, 8, a3);
	this->field_18.setPos(0.0, 18.0 - (float)a2, -6.0);
	this->field_118.addBox(-5.0, -10.0, -7.0, 10, 16, 8, a3);
	this->field_118.setPos(0.0, 17.0 - (float)a2, 2.0);
	this->field_198.addBox(-2.0, 0.0, -2.0, 4, a2, 4, a3);
	float v10 = 24.0 - (float)a2;
	this->field_198.setPos(-3.0, v10, 7.0);
	this->field_218.addBox(-2.0, 0.0, -2.0, 4, a2, 4, a3);
	this->field_218.setPos(3.0, v10, 7.0);
	this->field_298.addBox(-2.0, 0.0, -2.0, 4, a2, 4, a3);
	this->field_298.setPos(-3.0, v10, -5.0);
	this->field_318.addBox(-2.0, 0.0, -2.0, 4, a2, 4, a3);
	this->field_318.setPos(3.0, v10, -5.0);
}
QuadrupedModel::~QuadrupedModel() {
}
void QuadrupedModel::render(Entity* a2, float a3, float a4, float a5, float a6, float a7, float a8){
	this->setupAnim(a3, a4, a5, a6, a7, a8);
	if ( this->field_14 )
	{
		glPushMatrix();
		glTranslatef(0.0, a8 * this->field_398, a8 * this->field_39C);
		this->field_18.render(a8);
		glPopMatrix();
		glPushMatrix();
		glScalef(0.5, 0.5, 0.5);
		glTranslatef(0.0, a8 * 24.0, 0.0);
		this->field_118.render(a8);
		this->field_198.render(a8);
		this->field_218.render(a8);
		this->field_298.render(a8);
		this->field_318.render(a8);
		glPopMatrix();
	}
	else
	{
		this->field_18.render(a8);
		this->field_118.render(a8);
		this->field_198.render(a8);
		this->field_218.render(a8);
		this->field_298.render(a8);
		this->field_318.render(a8);
	}
}
void QuadrupedModel::setupAnim(float a2, float a3, float a4, float a5, float a6, float a7) {
	this->field_18.xRotAngle = a6 / (float)(180.0 / 3.1416);
	this->field_18.yRotAngle = a5 / (float)(180.0 / 3.1416);
	this->field_118.xRotAngle = 90.0 / (float)(180.0 / 3.1416);
	
	float v7 = (float)(Mth::cos(a2 * 0.6662f) * 1.4f) * a3;
	float v8 = (float)(Mth::cos(a2 * 0.6662f + 3.14159265f) * 1.4f) * a3;
	
	this->field_198.xRotAngle = v7;
	this->field_318.xRotAngle = v7;
	this->field_218.xRotAngle = v8;
	this->field_298.xRotAngle = v8;

	if (a3 > 0.001f) {
		float bodySway = Mth::sin(a2 * 0.6662f) * a3 * 0.05f;
		float headBob = Mth::cos(a2 * 0.6662f * 2.0f) * a3 * 0.08f;
		this->field_118.zRotAngle = bodySway;
		this->field_18.xRotAngle += headBob;
		this->field_18.zRotAngle = -bodySway * 0.5f;
	} else {
		this->field_118.zRotAngle = 0.0f;
		this->field_18.zRotAngle = 0.0f;
		this->field_18.xRotAngle += Mth::sin(a4 * 0.06f) * 0.02f;
	}
}
