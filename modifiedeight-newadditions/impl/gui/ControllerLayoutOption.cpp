#include <gui/ControllerLayoutOption.hpp>
#include <I18n.hpp>
#include <Minecraft.hpp>
#include <gui/buttons/Touch_TButton.hpp>
#include <gui/screens/ControllerLayoutScreen.hpp>
#include <rendering/Font.hpp>

ControllerLayoutOption::ControllerLayoutOption(Minecraft* a2, bool_t a3)
	: GuiElementContainer(0, 1, 0, 0, 24, 20) {
	this->isInWorld = a3;
	this->pressedButton = 0;
	this->customizeButton = std::shared_ptr<Button>(new Touch::TButton(9999998, 0, 0, 66, 20, I18n::get("options.controllerlayout.customize"), a2));
	this->addChild(this->customizeButton);
}

ControllerLayoutOption::~ControllerLayoutOption() {
}

void ControllerLayoutOption::render(Minecraft* a2, int32_t a3, int32_t a4) {
	int32_t v9 = (this->height - 8) / 2;
	a2->font->drawShadow(I18n::get("options.controllerlayout"), this->posX, (float)this->posY + (float)v9, 0xFFBBBBBB);
	GuiElementContainer::render(a2, a3, a4);
}

void ControllerLayoutOption::setupPositions() {
	int32_t v3 = 0;
	for(auto c: this->children) {
		c->posX = this->posX + this->width - c->width - 15;
		c->posY = this->posY + v3;
		v3 += c->height;
	}
	this->height = v3;
}

void ControllerLayoutOption::mouseClicked(Minecraft* a2, int32_t a3, int32_t a4, int32_t a5) {
	if(a5 != 1) return;
	if(this->customizeButton->clicked(a2, a3, a4)) {
		this->pressedButton = this->customizeButton.get();
		this->pressedButton->setPressed();
	}
}

void ControllerLayoutOption::mouseReleased(Minecraft* a2, int32_t a3, int32_t a4, int32_t a5) {
	if(this->pressedButton == this->customizeButton.get() && this->pressedButton->clicked(a2, a3, a4)) {
		a2->setScreen(new ControllerLayoutScreen(this->isInWorld));
	}
	this->pressedButton = 0;
	this->customizeButton->released(a3, a4);
}
