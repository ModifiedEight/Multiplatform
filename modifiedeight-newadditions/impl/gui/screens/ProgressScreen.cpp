#include <gui/screens/ProgressScreen.hpp>
#include <Minecraft.hpp>
#include <network/RakNetInstance.hpp>
#include <gui/buttons/Touch_TButton.hpp>
#include <gui/screens/DisconnectionScreen.hpp>
#include <gui/screens/Touch_StartMenuScreen.hpp>
#include <rendering/Font.hpp>
#include <rendering/Tesselator.hpp>
#include <rendering/Textures.hpp>
#include <utils.h>

ProgressScreen::ProgressScreen() {
	this->tickCounter = 0;
	this->cancelButton = 0;
}
void ProgressScreen::exitScreen() {
	this->minecraft->setScreen(new Touch::StartMenuScreen());
}

ProgressScreen::~ProgressScreen() {
	if(this->cancelButton) {
		delete this->cancelButton;
		this->cancelButton = 0;
	}
}
static char_t* _SYMS = "-\\|/";
void ProgressScreen::render(int32_t a2, int32_t a3, float a4) {
	int32_t field_CBC;		   // r11
	int32_t v11;			   // r10
	int32_t v12;			   // r3
	float v13;			   // s16
	int32_t v14;			   // r11
	int32_t width;			   // r10
	Font* font;			   // r5
	int32_t v17;			   // r0
	const char_t* v20;	   // r5
	int32_t v21;			   // r6
	int32_t v22;			   // r11
	int32_t v23;			   // s16
	Font* v25;			   // r10
	int32_t v26;			   // [sp+Ch] [bp-4Ch]

	this->cancelButton->visible = 1;
	if(this->minecraft->isLevelGenerated()) {
		this->minecraft->setScreen(0);
	} else {
		this->renderBackground(0);
		this->minecraft->texturesPtr->loadAndBindTexture("gui/background.png");
		Tesselator::instance.begin(4);
		Tesselator::instance.color(4210752);
		Tesselator::instance.vertexUV(0.0, (float)this->height, 0.0, 0.0, (float)this->height * 0.03125);
		Tesselator::instance.vertexUV((float)this->width, (float)this->height, 0.0, (float)this->width * 0.03125, (float)this->height * 0.03125);
		Tesselator::instance.vertexUV((float)this->width, 0.0, 0.0, (float)this->width * 0.03125, 0.0);
		Tesselator::instance.vertexUV(0.0, 0.0, 0.0, 0.0, 0.0);
		Tesselator::instance.draw(1);


		font = this->minecraft->font;
		glPushMatrix();
		float titleScale = 1.35f;
		glScalef(titleScale, titleScale, 1.0f);
		int32_t tcx = (int32_t)((float)this->width / titleScale);
		int32_t tcy = (int32_t)((float)this->height / titleScale);
		std::string titleStr = "Generating world";
		int32_t titleWidth = font->width(titleStr);
		font->drawShadow(titleStr, (float)((tcx - titleWidth) / 2), (float)(tcy / 2 - 24), 0xFFFFFF);
		glPopMatrix();

		v20 = this->minecraft->getProgressMessage();
		v21 = this->minecraft->font->width(v20);
		v22 = this->height / 2 + 6;
		v23 = (this->width - v21) / 2;
		this->minecraft->font->drawShadow(v20, (float)v23, (float)v22, 0xCCCCCC);
		if(!this->minecraft->getProgressStatusId() || this->minecraft->getProgressStatusId() == 4) {
			v25 = this->minecraft->font;
			this->drawCenteredString(v25, std::string(1, _SYMS[(int32_t)(getTimeS() * 5.5) % 4]), v23 + v21 + 6, v22, -1);
		}
		Screen::render(a2, a3, a4);
		sleepMs(50);
	}
}
void ProgressScreen::init() {
	this->cancelButton = new Touch::TButton(0, "Cancel", 0);
	this->cancelButton->init(this->minecraft);
	this->buttons.push_back(this->cancelButton);
}
void ProgressScreen::setupPositions() {
	this->cancelButton->posX = this->width / 2 - this->cancelButton->width / 2;
	this->cancelButton->posY = this->height / 2 + 42;
}
bool_t ProgressScreen::handleBackEvent(bool_t a2) {
	if(!a2) {
		if (this->minecraft->rakNetInstance) this->minecraft->rakNetInstance->disconnect();
		this->exitScreen();
	}
	return 1;
}
void ProgressScreen::tick() {
	int32_t v2; // r3

	v2 = this->tickCounter + 1;
	this->tickCounter = v2;
	if(v2 == 200 && !this->minecraft->getProgressStatusId()) {
		this->minecraft->setScreen(new DisconnectionScreen("Could not connect to server. Try again."));
	}
}
bool_t ProgressScreen::isInGameScreen() {
	return 0;
}
void ProgressScreen::feedMCOEvent(MCOEvent) {
}
void ProgressScreen::buttonClicked(Button* a2) {
	if(this->cancelButton == a2) {
		if (this->minecraft->rakNetInstance) this->minecraft->rakNetInstance->disconnect();
		this->exitScreen();
	}
}
