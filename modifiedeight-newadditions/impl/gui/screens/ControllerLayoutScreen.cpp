#include <gui/screens/ControllerLayoutScreen.hpp>
#include <I18n.hpp>
#include <Minecraft.hpp>
#include <cpputils.hpp>
#include <gui/Gui.hpp>
#include <gui/NinePatchFactory.hpp>
#include <gui/NinePatchLayer.hpp>
#include <gui/buttons/Touch_TButton.hpp>
#include <gui/screens/OptionsScreen.hpp>
#include <input/ControllerHandler.hpp>
#include <input/Gamepad.hpp>
#include <input/Mouse.hpp>
#include <rendering/Font.hpp>
#include <rendering/Tesselator.hpp>
#include <util/IntRectangle.hpp>
#include <unigl.h>
#include <utils.h>

static const int32_t _clsRowHeight = 30;
static const int32_t _clsRowGap = 1;
static const int32_t _clsStride = 31;
static const int32_t _clsScrollW = 10;
static const int32_t _clsColGap = 2;

ControllerLayoutScreen::ControllerLayoutScreen(bool_t a2) {
	this->isInWorld = a2;
	this->optBtn = 0;
	this->playPressed = 0;
	this->listBg = 0;
	this->panelBg = 0;
	this->buttonSave = 0;
	this->buttonReset = 0;
	this->buttonCancel = 0;
	this->capturingAction = -1;
	this->scrollOffset = 0.0f;
	this->scrollTarget = 0.0f;
	this->contentHeight = CA_COUNT * _clsStride;
	this->listTop = 0;
	this->listBottom = 0;
	this->actionsColumnWidth = 0;
	this->bindingsColumnWidth = 0;
	this->dragStartY = 0;
	this->dragStartOffset = 0.0f;
	this->dragVel = 0.0f;
	this->lastDragMs = 0;
	this->isDragging = 0;
	this->draggingBar = 0;
	this->barGrabDY = 0;
	this->lastAnimMs = 0;
	for(int32_t i = 0; i < CA_COUNT; ++i) {
		this->working[i] = ControllerLayout::get(i);
	}
}

ControllerLayoutScreen::~ControllerLayoutScreen() {
	ControllerHandler::endCapture();
	safeRemove(this->optBtn);
	safeRemove(this->playPressed);
	safeRemove(this->listBg);
	safeRemove(this->panelBg);
	if(this->buttonSave) {
		delete this->buttonSave;
		this->buttonSave = 0;
	}
	if(this->buttonReset) {
		delete this->buttonReset;
		this->buttonReset = 0;
	}
	if(this->buttonCancel) {
		delete this->buttonCancel;
		this->buttonCancel = 0;
	}
}

void ControllerLayoutScreen::init() {
	this->buttons.clear();
	this->field_2C.clear();

	this->buttonSave = new Touch::TButton(1, I18n::get("gui.save"), 0);
	this->buttonReset = new Touch::TButton(2, I18n::get("gui.reset"), 0);
	this->buttonCancel = new Touch::TButton(3, I18n::get("gui.cancel"), 0);

	IntRectangle rOptions;
	rOptions.minX = 112;
	rOptions.minY = 0;
	rOptions.width = 8;
	rOptions.height = 67;
	IntRectangle rOptionsPressed;
	rOptionsPressed.minX = 120;
	rOptionsPressed.minY = 0;
	rOptionsPressed.width = 8;
	rOptionsPressed.height = 67;
	((Touch::TButton*)this->buttonSave)->init(this->minecraft, "gui/spritesheet.png", rOptions, rOptionsPressed, 2, 2, 66, 26);
	((Touch::TButton*)this->buttonReset)->init(this->minecraft, "gui/spritesheet.png", rOptions, rOptionsPressed, 2, 2, 66, 26);
	((Touch::TButton*)this->buttonCancel)->init(this->minecraft, "gui/spritesheet.png", rOptions, rOptionsPressed, 2, 2, 66, 26);

	NinePatchFactory npf(this->minecraft->texturesPtr, "gui/spritesheet.png");
	safeRemove(this->optBtn);
	safeRemove(this->playPressed);
	safeRemove(this->listBg);
	safeRemove(this->panelBg);
	IntRectangle rNormal;
	rNormal.minX = 112;
	rNormal.minY = 0;
	rNormal.width = 8;
	rNormal.height = 67;
	this->optBtn = npf.createSymmetrical(rNormal, 2, 2, 32, 32);
	IntRectangle rBlack;
	rBlack.minX = 0;
	rBlack.minY = 32;
	rBlack.width = 8;
	rBlack.height = 8;
	this->playPressed = npf.createSymmetrical(rBlack, 2, 2, 32, 32);
	this->listBg = npf.createSymmetrical(rBlack, 2, 2, 32, 32);
	IntRectangle rPanel;
	rPanel.minX = 34;
	rPanel.minY = 43;
	rPanel.width = 14;
	rPanel.height = 14;
	this->panelBg = npf.createSymmetrical(rPanel, 3, 3, 32, 32);

	this->buttons.emplace_back(this->buttonSave);
	this->buttons.emplace_back(this->buttonReset);
	this->buttons.emplace_back(this->buttonCancel);
}

void ControllerLayoutScreen::setupPositions() {
	int32_t sideMargin = this->width * 5 / 100;
	if(sideMargin < 10) sideMargin = 10;
	int32_t panelL = sideMargin;
	int32_t panelR = this->width - sideMargin;

	this->contentLeft = panelL + 10;
	this->contentRight = panelR - 10 - _clsScrollW - 4;
	this->scrollBarX = this->contentRight + 4;
	if(this->contentRight < this->contentLeft + 80) {
		this->contentRight = this->contentLeft + 80;
	}
	int32_t total = this->contentRight - this->contentLeft;
	this->actionsColumnWidth = (total * 57) / 100;
	this->bindingsColumnWidth = total - this->actionsColumnWidth;

	int32_t btnH = 26;
	int32_t gap = 6;
	int32_t btnW = (panelR - 10 - this->contentLeft - gap * 2) / 3;
	if(btnW < 20) btnW = 20;
	int32_t btnY = this->height - btnH - 14;

	this->buttonSave->width = btnW;
	this->buttonSave->height = btnH;
	this->buttonSave->posX = this->contentLeft;
	this->buttonSave->posY = btnY;

	this->buttonReset->width = btnW;
	this->buttonReset->height = btnH;
	this->buttonReset->posX = this->contentLeft + btnW + gap;
	this->buttonReset->posY = btnY;

	this->buttonCancel->width = btnW;
	this->buttonCancel->height = btnH;
	this->buttonCancel->posX = this->contentLeft + (btnW + gap) * 2;
	this->buttonCancel->posY = btnY;

	this->listTop = 56;
	this->listBottom = btnY - 10;
	if(this->listBottom < this->listTop + _clsRowHeight) {
		this->listBottom = this->listTop + _clsRowHeight;
	}

	this->contentHeight = CA_COUNT * _clsStride;
	this->clampScroll();
}



void ControllerLayoutScreen::clampScroll() {
	float viewHeight = (float)(this->listBottom - this->listTop);
	float maxScroll = (float)this->contentHeight - viewHeight;
	if(maxScroll < 0.0f) maxScroll = 0.0f;
	if(this->scrollOffset > maxScroll) this->scrollOffset = maxScroll;
	if(this->scrollOffset < 0.0f) this->scrollOffset = 0.0f;
	if(this->scrollTarget > maxScroll) this->scrollTarget = maxScroll;
	if(this->scrollTarget < 0.0f) this->scrollTarget = 0.0f;
}

int32_t ControllerLayoutScreen::rowIndexAt(int32_t a2, int32_t a3) {
	if(a3 < this->listTop || a3 >= this->listBottom) return -1;
	int32_t bindingsX = this->contentLeft + this->actionsColumnWidth + _clsColGap;
	if(a2 < bindingsX || a2 >= this->contentRight) return -1;
	int32_t rel = a3 - this->listTop + (int32_t)this->scrollOffset;
	if(rel < 0) return -1;
	int32_t idx = rel / _clsStride;
	if(idx < 0 || idx >= CA_COUNT) return -1;
	if(rel % _clsStride >= _clsRowHeight) return -1;
	return idx;
}

void ControllerLayoutScreen::closeScreen() {
	this->minecraft->setScreen(new OptionsScreen(this->isInWorld));
}

void ControllerLayoutScreen::render(int32_t a2, int32_t a3, float a4) {
	if(!this->isInWorld) {
		if(this->renderGameBehind()) {
			this->renderMenuBackground(a4);
		}
	}
	this->renderBackground(0);

	int32_t nowMs = getTimeMs();
	int32_t animDt = nowMs - this->lastAnimMs;
	if(this->lastAnimMs == 0 || animDt < 1) animDt = 17;
	if(animDt > 100) animDt = 100;
	this->lastAnimMs = nowMs;
	if(this->draggingBar) {
		int32_t mx = Mouse::getX();
		int32_t my = Mouse::getY();
		this->toGUICoordinate(mx, my);
		int32_t viewHeight = this->listBottom - this->listTop;
		int32_t barY = this->listTop;
		int32_t barH = viewHeight;
		this->calcBar(barY, barH);
		int32_t maxScroll = this->contentHeight - viewHeight;
		int32_t trackTop = this->listTop - 3;
		int32_t trackH = viewHeight + 5;
		if(maxScroll > 0 && trackH - barH > 0) {
			int32_t wantY = my - this->barGrabDY;
			if(wantY < trackTop) wantY = trackTop;
			if(wantY > trackTop + trackH - barH) wantY = trackTop + trackH - barH;
			float t = (float)(wantY - trackTop) / (float)(trackH - barH);
			this->scrollOffset = t * (float)maxScroll;
			this->scrollTarget = this->scrollOffset;
			this->clampScroll();
		}
	} else if(this->isDragging) {
		int32_t mx = Mouse::getX();
		int32_t my = Mouse::getY();
		this->toGUICoordinate(mx, my);
		float prev = this->scrollOffset;
		this->scrollOffset = this->dragStartOffset + (float)(this->dragStartY - my);
		this->clampScroll();
		this->scrollTarget = this->scrollOffset;
		float v = (this->scrollOffset - prev) / (float)animDt;
		this->dragVel = this->dragVel * 0.7f + v * 0.3f;
		this->lastDragMs = nowMs;
	} else {
		float diff = this->scrollTarget - this->scrollOffset;
		if(diff > 0.5f || diff < -0.5f) {
			float k = (float)animDt * 0.025f;
			if(k > 1.0f) k = 1.0f;
			this->scrollOffset += diff * k;
			this->clampScroll();
		} else {
			this->scrollOffset = this->scrollTarget;
		}
	}

	int32_t sideMargin = this->width * 5 / 100;
	if(sideMargin < 10) sideMargin = 10;
	int32_t panelL = sideMargin;
	int32_t panelR = this->width - sideMargin;
	int32_t panelT = 6;
	int32_t panelB = this->height - 6;

	if(this->panelBg) {
		this->panelBg->setSize((float)(panelR - panelL), (float)(panelB - panelT));
		this->panelBg->draw(Tesselator::instance, (float)panelL, (float)panelT);
	}

	this->drawCenteredString(this->font, I18n::get("options.controllerlayout"), this->width / 2, 16, 0xFFFFFFFF);

	int32_t actionsX = this->contentLeft;
	int32_t bindingsX = this->contentLeft + this->actionsColumnWidth + _clsColGap;
	int32_t rightX = this->contentRight;
	int32_t actionsCX = actionsX + this->actionsColumnWidth / 2;
	int32_t bindingsCX = bindingsX + (rightX - bindingsX) / 2;

	this->drawCenteredString(this->font, I18n::get("options.controllerlayout.actions"), actionsCX, 40, 0xFFFFFFFF);
	this->drawCenteredString(this->font, I18n::get("options.controllerlayout.bindings"), bindingsCX, 40, 0xFFFFFFFF);

	if(this->listBg) {
		this->listBg->setSize((float)(this->contentRight - this->contentLeft + 4), (float)(this->listBottom - this->listTop + 5));
		this->listBg->draw(Tesselator::instance, (float)(this->contentLeft - 2), (float)(this->listTop - 3));
	}

	glEnable(0xC11u);
	glScissor((uint32_t)(float)(Gui::GuiScale * (float)this->contentLeft), (uint32_t)(float)(this->minecraft->field_20 - (float)(Gui::GuiScale * (float)this->listBottom)), (uint32_t)(float)(Gui::GuiScale * (float)(this->scrollBarX + _clsScrollW - this->contentLeft)), (uint32_t)(float)(Gui::GuiScale * (float)(this->listBottom - this->listTop)));

	for(int32_t i = 0; i < CA_COUNT; ++i) {
		int32_t rowY = this->listTop + i * _clsStride - (int32_t)this->scrollOffset;
		if(rowY + _clsRowHeight <= this->listTop) continue;
		if(rowY >= this->listBottom) break;

		bool_t capturing = this->capturingAction == i;
		if(this->optBtn) {
			this->optBtn->setSize((float)this->actionsColumnWidth, (float)_clsRowHeight);
			this->optBtn->draw(Tesselator::instance, (float)actionsX, (float)rowY);
		}
		NinePatchLayer* bindLayer = this->playPressed ? this->playPressed : this->optBtn;
		if(bindLayer) {
			bindLayer->setSize((float)(rightX - bindingsX), (float)_clsRowHeight);
			bindLayer->draw(Tesselator::instance, (float)bindingsX, (float)rowY);
		}

		int32_t textY = rowY + (_clsRowHeight - 8) / 2;
		this->drawCenteredString(this->font, ControllerLayout::actionName(i), actionsCX, textY, 0xFFE0E0E0);

		std::string label;
		uint32_t labelColor;
		if(capturing) {
			label = I18n::get("options.controllerlayout.press");
			labelColor = 0xFFFFA0;
		} else if(this->working[i].isSet()) {
			label = this->working[i].describe();
			labelColor = 0xFFE0E0E0;
		} else {
			label = I18n::get("options.controllerlayout.unbound");
			labelColor = 0xFFA0A0A0;
		}
		this->drawCenteredString(this->font, label, bindingsCX, textY, labelColor);
	}
	glDisable(0xC11u);

	int32_t barY = this->listTop;
	int32_t barHeight = 0;
	this->calcBar(barY, barHeight);
	int32_t viewHeight = this->listBottom - this->listTop;
	if(this->contentHeight > viewHeight && viewHeight > 0) {
		if(this->playPressed) {
			this->playPressed->setSize((float)_clsScrollW, (float)(viewHeight + 5));
			this->playPressed->draw(Tesselator::instance, (float)this->scrollBarX, (float)(this->listTop - 3));
		}
		if(this->optBtn) {
			this->optBtn->setSize((float)_clsScrollW, (float)barHeight);
			this->optBtn->draw(Tesselator::instance, (float)this->scrollBarX, (float)barY);
		}
	}

	if(!Gamepad::connected) {
		std::string notice = I18n::get("options.controllerlayout.nogamepad");
		int32_t noticeW = this->font->width(notice);
		int32_t noticeCX = (this->contentLeft + this->scrollBarX + _clsScrollW) / 2;
		int32_t noticeCY = (this->listTop + this->listBottom) / 2;
		this->fill(noticeCX - noticeW / 2 - 10, noticeCY - 11, noticeCX + noticeW / 2 + 10, noticeCY + 11, 0xDD000000);
		this->drawCenteredString(this->font, notice, noticeCX, noticeCY - 4, 0xFFFF5555);
	}

	Screen::render(a2, a3, a4);
}

void ControllerLayoutScreen::calcBar(int32_t& outY, int32_t& outH) {
	int32_t viewHeight = this->listBottom - this->listTop;
	outY = this->listTop - 3;
	outH = viewHeight + 5;
	if(this->contentHeight > viewHeight && viewHeight > 0) {
		outH = (viewHeight * viewHeight) / this->contentHeight;
		outH = (outH * 3) / 4;
		if(outH < 10) outH = 10;
		if(outH > viewHeight) outH = viewHeight;
		int32_t maxScroll = this->contentHeight - viewHeight;
		if(maxScroll > 0) {
			outY = this->listTop - 3 + (int32_t)(((float)(viewHeight + 5 - outH) * this->scrollOffset) / (float)maxScroll);
		} else {
			outY = this->listTop - 3;
		}
	}
}

void ControllerLayoutScreen::tick() {
	if(this->capturingAction >= 0) {
		Binding captured;
		if(ControllerHandler::takeCaptured(captured)) {
			int32_t action = this->capturingAction;
			this->capturingAction = -1;
			bool_t axisTarget = ControllerLayout::isAxisAction(action);
			bool_t axisBinding = captured.source == BIND_AXIS_FULL;
			if(axisTarget == axisBinding) {
				for(int32_t i = 0; i < CA_COUNT; ++i) {
					if(i == action) continue;
					if(ControllerLayout::isAxisAction(i) != axisTarget) continue;
					if(this->working[i].equals(captured)) {
						this->working[i] = Binding();
					}
				}
				this->working[action] = captured;
			}
		}
	}
	Screen::tick();
}

bool_t ControllerLayoutScreen::handleBackEvent(bool_t a2) {
	if(!a2) {
		if(this->capturingAction >= 0) {
			this->capturingAction = -1;
			ControllerHandler::endCapture();
			return 1;
		}
		this->closeScreen();
	}
	return 1;
}

bool_t ControllerLayoutScreen::renderGameBehind() {
	return 1;
}

void ControllerLayoutScreen::buttonClicked(Button* a2) {
	if(!a2) return;
	if(a2 == this->buttonSave) {
		for(int32_t i = 0; i < CA_COUNT; ++i) {
			ControllerLayout::set(i, this->working[i]);
		}
		ControllerLayout::save();
		this->closeScreen();
		return;
	}
	if(a2 == this->buttonReset) {
		this->capturingAction = -1;
		ControllerHandler::endCapture();
		Binding backup[CA_COUNT];
		for(int32_t i = 0; i < CA_COUNT; ++i) {
			backup[i] = ControllerLayout::get(i);
		}
		ControllerLayout::resetToDefaults();
		for(int32_t i = 0; i < CA_COUNT; ++i) {
			this->working[i] = ControllerLayout::get(i);
			ControllerLayout::set(i, backup[i]);
		}
		return;
	}
	if(a2 == this->buttonCancel) {
		this->closeScreen();
		return;
	}
}

void ControllerLayoutScreen::mouseClicked(int32_t a2, int32_t a3, int32_t a4) {
	if(a4 == 4 || a4 == 5) {
		this->scrollTarget += (a4 == 4) ? -((float)_clsStride * 2.0f) : ((float)_clsStride * 2.0f);
		this->clampScroll();
		return;
	}

	if(a4 == 1) {
		int32_t barY = this->listTop;
		int32_t barH = 0;
		this->calcBar(barY, barH);
		if(a2 >= this->scrollBarX && a2 < this->scrollBarX + _clsScrollW && a3 >= barY && a3 < barY + barH) {
			this->draggingBar = 1;
			this->barGrabDY = a3 - barY;
			Screen::mouseClicked(a2, a3, a4);
			return;
		}
		int32_t idx = this->rowIndexAt(a2, a3);
		if(idx >= 0) {
			if(this->capturingAction == idx) {
				this->capturingAction = -1;
				ControllerHandler::endCapture();
			} else {
				this->capturingAction = idx;
				ControllerHandler::beginCapture(ControllerLayout::isAxisAction(idx));
			}
		} else if(a3 >= this->listTop && a3 < this->listBottom && a2 >= this->contentLeft && a2 <= this->scrollBarX + _clsScrollW) {
			this->isDragging = 1;
			this->dragStartY = a3;
			this->dragStartOffset = this->scrollOffset;
			this->scrollTarget = this->scrollOffset;
			this->dragVel = 0.0f;
			this->lastDragMs = getTimeMs();
		}
	}

	Screen::mouseClicked(a2, a3, a4);
}

void ControllerLayoutScreen::mouseReleased(int32_t a2, int32_t a3, int32_t a4) {
	if(a4 == 1) {
		this->draggingBar = 0;
		if(this->isDragging) {
			this->isDragging = 0;
			this->scrollTarget = this->scrollOffset + this->dragVel * 140.0f;
			this->dragVel = 0.0f;
			this->clampScroll();
		}
	}
	Screen::mouseReleased(a2, a3, a4);
}

void ControllerLayoutScreen::keyPressed(int32_t a2) {
	if(this->capturingAction >= 0) {
		if(a2 == 255 || a2 == this->minecraft->options.keyMenuCancel.keyCode) {
			this->capturingAction = -1;
			ControllerHandler::endCapture();
		}
		return;
	}
	Screen::keyPressed(a2);
}
