#include <gui/LoginOption.hpp>
#include <Minecraft.hpp>
#include <gui/buttons/Touch_TButton.hpp>
#include <gui/elements/Label.hpp>
#include <network/mco/LoginInformation.hpp>
#include <network/mco/MCOStringify.hpp>
#include <network/mco/MojangConnector.hpp>
#include <network/mco/RestRequestJob.hpp>
#include <util/Util.hpp>

LoginOption::LoginOption(Minecraft* mc)
	: GuiElementContainer(0, 1, 0, 0, 24, 24) {
	this->field_50 = 0;
	this->beepLabel = std::shared_ptr<Label>(new Label("Created by eqozqq. Thanks to: GameHerobrine, TimofeyLednev, MineDg and others.", mc, 0x808080, 0, 0, 0, 1));
	this->addChild(this->beepLabel);
}

LoginOption::~LoginOption() {
}
void LoginOption::tick(Minecraft* a2) {
	this->field_50 = 0;
	this->setupPositions();
}
void LoginOption::setupPositions() {
	if(this->beepLabel) {
		this->beepLabel->posX = this->posX;
		this->beepLabel->posY = this->posY + 3;
		this->beepLabel->setWidth(this->width);
		this->beepLabel->setupPositions();
		this->height = this->beepLabel->height + 6;
	}
}
void LoginOption::mouseClicked(Minecraft* mc, int32_t a3, int32_t a4, int32_t a5) {
}

void LoginOption::mouseReleased(Minecraft* mc, int32_t a3, int32_t a4, int32_t a5) {
	this->field_50 = 0;
}

