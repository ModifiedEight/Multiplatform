#include <rendering/model/ArmorStandModel.hpp>
#include <entity/ArmorStand.hpp>

ArmorStandModel::ArmorStandModel()
	: HumanoidModel(0.0f, 0.0f)
	, rightBodyStick(16, 0, 64, 64)
	, leftBodyStick(48, 16, 64, 64)
	, shoulderStick(0, 48, 64, 64)
	, basePlate(0, 32, 64, 64) {
	this->headModel = ModelPart(0, 0, 64, 64);
	this->headModel.addBox(-1.0f, -7.0f, -1.0f, 2, 7, 2, 0.0f);
	this->headModel.setPos(0.0f, 1.0f, 0.0f);

	this->bodyModel = ModelPart(0, 26, 64, 64);
	this->bodyModel.addBox(-6.0f, 0.0f, -1.5f, 12, 3, 3, 0.0f);
	this->bodyModel.setPos(0.0f, 0.0f, 0.0f);

	this->rightArmModel = ModelPart(24, 0, 64, 64);
	this->rightArmModel.addBox(-2.0f, -2.0f, -1.0f, 2, 12, 2, 0.0f);
	this->rightArmModel.setPos(-5.0f, 2.0f, 0.0f);

	this->leftArmModel = ModelPart(32, 16, 64, 64);
	this->leftArmModel.mirror = 1;
	this->leftArmModel.addBox(0.0f, -2.0f, -1.0f, 2, 12, 2, 0.0f);
	this->leftArmModel.setPos(5.0f, 2.0f, 0.0f);

	this->rightLegModel = ModelPart(8, 0, 64, 64);
	this->rightLegModel.addBox(-1.0f, 0.0f, -1.0f, 2, 11, 2, 0.0f);
	this->rightLegModel.setPos(-1.9f, 12.0f, 0.0f);

	this->leftLegModel = ModelPart(40, 16, 64, 64);
	this->leftLegModel.mirror = 1;
	this->leftLegModel.addBox(-1.0f, 0.0f, -1.0f, 2, 11, 2, 0.0f);
	this->leftLegModel.setPos(1.9f, 12.0f, 0.0f);

	this->rightBodyStick.addBox(-3.0f, 3.0f, -1.0f, 2, 7, 2, 0.0f);
	this->leftBodyStick.addBox(1.0f, 3.0f, -1.0f, 2, 7, 2, 0.0f);
	this->shoulderStick.addBox(-4.0f, 10.0f, -1.0f, 8, 2, 2, 0.0f);

	this->basePlate.addBox(-6.0f, 11.0f, -6.0f, 12, 1, 12, 0.0f);
	this->basePlate.setPos(0.0f, 12.0f, 0.0f);
}

ArmorStandModel::~ArmorStandModel() {
}

void ArmorStandModel::render(Entity* entity, float a2, float a3, float a4, float a5, float a6, float a7) {
	ArmorStand* stand = (ArmorStand*)entity;
	this->headModel.render(a7);
	this->bodyModel.render(a7);
	this->rightLegModel.render(a7);
	this->leftLegModel.render(a7);
	this->rightBodyStick.render(a7);
	this->leftBodyStick.render(a7);
	this->shoulderStick.render(a7);
	if (!stand || !stand->noBasePlate) {
		this->basePlate.render(a7);
	}
	if (stand && stand->showArms) {
		this->rightArmModel.render(a7);
		this->leftArmModel.render(a7);
	}
}
