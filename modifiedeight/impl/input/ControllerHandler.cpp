#include <input/ControllerHandler.hpp>
#include <Minecraft.hpp>
#include <Options.hpp>
#include <entity/LocalPlayer.hpp>
#include <entity/Player.hpp>
#include <gui/Gui.hpp>
#include <gui/Screen.hpp>
#include <input/IMoveInput.hpp>
#include <input/Keyboard.hpp>
#include <input/Mouse.hpp>
#include <inventory/Inventory.hpp>
#include <math.h>
#include <utils.h>

bool_t ControllerHandler::captureMode = 0;
bool_t ControllerHandler::captureWantsAxis = 0;
bool_t ControllerHandler::hasCaptured = 0;
Binding ControllerHandler::capturedBinding;
float ControllerHandler::cursorX = -1.0f;
float ControllerHandler::cursorY = -1.0f;
int32_t ControllerHandler::scrollRepeat = 0;
int32_t ControllerHandler::lastPadCursorMs = -100000;

void ControllerHandler::hideCursor() {
	ControllerHandler::lastPadCursorMs = -100000;
}

bool_t ControllerHandler::cursorVisible() {
	if(!Gamepad::connected) return 0;
	return (getTimeMs() - ControllerHandler::lastPadCursorMs) < 3000;
}
float ControllerHandler::moveX = 0.0f;
float ControllerHandler::moveY = 0.0f;
bool_t ControllerHandler::jumpHeld = 0;
bool_t ControllerHandler::sneakHeld = 0;
bool_t ControllerHandler::sprintHeld = 0;
bool_t ControllerHandler::attackHeld = 0;
bool_t ControllerHandler::useHeld = 0;
bool_t ControllerHandler::confirmHeld = 0;
float ControllerHandler::frameDt = 1.0f;
double ControllerHandler::lastFrameS = 0;
void ControllerHandler::clampVec(float& x, float& y) {
	float mag2 = x * x + y * y;
	if(mag2 > 1.0f) {
		float inv = 1.0f / sqrtf(mag2);
		x *= inv;
		y *= inv;
	}
}
int32_t ControllerHandler::lastPerspectiveMs = -100000;

static bool_t _chButtonFree(int32_t button) {
	for(int32_t i = 0; i < CA_COUNT; ++i) {
		Binding b = ControllerLayout::get(i);
		if(b.source == BIND_BUTTON && b.code == button) return 0;
	}
	return 1;
}

void ControllerHandler::beginCapture(bool_t wantAxis) {
	ControllerHandler::captureMode = 1;
	ControllerHandler::captureWantsAxis = wantAxis;
	ControllerHandler::hasCaptured = 0;
	ControllerHandler::capturedBinding = Binding();
}

void ControllerHandler::endCapture() {
	ControllerHandler::captureMode = 0;
	ControllerHandler::hasCaptured = 0;
	ControllerHandler::capturedBinding = Binding();
}

bool_t ControllerHandler::takeCaptured(Binding& out) {
	if(!ControllerHandler::hasCaptured) return 0;
	out = ControllerHandler::capturedBinding;
	ControllerHandler::hasCaptured = 0;
	ControllerHandler::captureMode = 0;
	return 1;
}

bool_t ControllerHandler::isEnabled(Minecraft* mc) {
	if(!mc) return 0;
	if(!Gamepad::connected) return 0;
	return 1;
}

void ControllerHandler::releaseAll() {
	ControllerHandler::moveX = 0.0f;
	ControllerHandler::moveY = 0.0f;
	ControllerHandler::jumpHeld = 0;
	ControllerHandler::sneakHeld = 0;
	ControllerHandler::sprintHeld = 0;
	ControllerHandler::scrollRepeat = 0;
	if(ControllerHandler::attackHeld) {
		ControllerHandler::attackHeld = 0;
		if(Options::instance) {
			Keyboard::feed(Options::instance->keyDestroy.keyCode, 0);
		}
	}
	if(ControllerHandler::useHeld) {
		ControllerHandler::useHeld = 0;
		if(Options::instance) {
			Keyboard::feed(Options::instance->keyUse.keyCode, 0);
		}
	}
}

void ControllerHandler::tick(Minecraft* mc) {
	double nowS = getTimeS();
	float dtMs = (float)((nowS - ControllerHandler::lastFrameS) * 1000.0);
	if(ControllerHandler::lastFrameS == 0) dtMs = 17.0f;
	if(dtMs < 0.1f) dtMs = 0.1f;
	if(dtMs > 100.0f) dtMs = 100.0f;
	ControllerHandler::lastFrameS = nowS;
	ControllerHandler::frameDt += ((dtMs / 16.6667f) - ControllerHandler::frameDt) * 0.2f;
	if(!ControllerHandler::isEnabled(mc)) {
		ControllerHandler::releaseAll();
		ControllerHandler::confirmHeld = 0;
		Gamepad::beginFrame();
		return;
	}

	if(ControllerHandler::captureMode) {
		ControllerHandler::releaseAll();
		ControllerHandler::confirmHeld = 0;
		if(!ControllerHandler::hasCaptured) {
			Binding found;
			if(Gamepad::detectPressedBinding(found, ControllerHandler::captureWantsAxis)) {
				ControllerHandler::capturedBinding = found;
				ControllerHandler::hasCaptured = 1;
			}
		}
		Gamepad::beginFrame();
		return;
	}

	if(mc->player && !mc->currentScreen) {
		ControllerHandler::tickGame(mc);
	} else {
		ControllerHandler::releaseAll();
		if(mc->currentScreen) {
			mc->field_D14 = 1;
			ControllerHandler::tickMenu(mc);
			mc->field_D14 = 0;
			if(mc->field_D15) {
				mc->setScreen(mc->field_D18);
				mc->field_D18 = 0;
				mc->field_D15 = 0;
			}
		} else {
			ControllerHandler::confirmHeld = 0;
		}
	}

	Gamepad::beginFrame();
}

void ControllerHandler::tickGame(Minecraft* mc) {
	ControllerHandler::confirmHeld = 0;

	float mx = Gamepad::getBindingValue(ControllerLayout::get(CA_MOVE_AXIS_X));
	float my = Gamepad::getBindingValue(ControllerLayout::get(CA_MOVE_AXIS_Y));
	ControllerHandler::moveX = mx;
	ControllerHandler::moveY = my;

	ControllerHandler::jumpHeld = Gamepad::isBindingDown(ControllerLayout::get(CA_JUMP));
	ControllerHandler::sneakHeld = Gamepad::isBindingDown(ControllerLayout::get(CA_SNEAK));

	static int32_t lastFwdTapMs = 0;
	static bool_t fwdWasDown = 0;
	static bool_t sprintLatch = 0;
	bool_t fwdDown = my < -0.5f;
	if(fwdDown && !fwdWasDown) {
		int32_t nowMs = getTimeMs();
		if(nowMs - lastFwdTapMs < 300) {
			sprintLatch = 1;
		}
		lastFwdTapMs = nowMs;
	}
	if(!fwdDown) {
		sprintLatch = 0;
	}
	fwdWasDown = fwdDown;
	bool_t wantSprint = sprintLatch;
	if(_chButtonFree(GP_BTN_L3) && Gamepad::isButtonDown(GP_BTN_L3)) {
		wantSprint = 1;
	}
	ControllerHandler::sprintHeld = wantSprint;

	float lx = Gamepad::getBindingValue(ControllerLayout::get(CA_LOOK_AXIS_X));
	float ly = Gamepad::getBindingValue(ControllerLayout::get(CA_LOOK_AXIS_Y));
	ControllerHandler::clampVec(lx, ly);
	if(lx != 0.0f || ly != 0.0f) {
		float sens = mc->options.controllerSensitivity * ControllerHandler::frameDt;
		float turnX = lx * sens;
		float turnY = ly * sens * (mc->options.invertMouse ? 1.0f : -1.0f);
		mc->player->turn(turnX, turnY);
	}

	bool_t attack = Gamepad::isBindingDown(ControllerLayout::get(CA_ATTACK));
	if(attack && !ControllerHandler::attackHeld) {
		ControllerHandler::attackHeld = 1;
		Keyboard::feed(mc->options.keyDestroy.keyCode, 1);
	} else if(!attack && ControllerHandler::attackHeld) {
		ControllerHandler::attackHeld = 0;
		Keyboard::feed(mc->options.keyDestroy.keyCode, 0);
	}

	bool_t use = Gamepad::isBindingDown(ControllerLayout::get(CA_USE_ITEM));
	if(use && !ControllerHandler::useHeld) {
		ControllerHandler::useHeld = 1;
		Keyboard::feed(mc->options.keyUse.keyCode, 1);
	} else if(!use && ControllerHandler::useHeld) {
		ControllerHandler::useHeld = 0;
		Keyboard::feed(mc->options.keyUse.keyCode, 0);
	}

	int32_t numSlots = mc->gui.getNumSlots();
	if(numSlots < 3) numSlots = 3;

	if(Gamepad::wasBindingPressed(ControllerLayout::get(CA_CYCLE_LEFT))) {
		int32_t slot = mc->player->inventory->selectedSlot - 1;
		if(slot < 0) slot = numSlots - 2;
		mc->player->inventory->selectSlot(slot);
	}
	if(Gamepad::wasBindingPressed(ControllerLayout::get(CA_CYCLE_RIGHT))) {
		int32_t slot = mc->player->inventory->selectedSlot + 1;
		if(slot >= numSlots - 1) slot = 0;
		mc->player->inventory->selectSlot(slot);
	}

	if(Gamepad::wasBindingPressed(ControllerLayout::get(CA_DROP_ITEM))) {
		mc->player->inventory->dropSlot(mc->player->inventory->selectedSlot, 0, 0);
	}

	if(Gamepad::wasBindingPressed(ControllerLayout::get(CA_CRAFTING))) {
		mc->player->startCrafting((int32_t)mc->player->posX, (int32_t)mc->player->posY, (int32_t)mc->player->posZ, 0);
	}

	if(Gamepad::wasBindingPressed(ControllerLayout::get(CA_INVENTORY))) {
		mc->gui.handleKeyPressed(100);
	}

	if(Gamepad::wasBindingPressed(ControllerLayout::get(CA_TOGGLE_PERSPECTIVE))) {
		int32_t nowMs = getTimeMs();
		if(nowMs - ControllerHandler::lastPerspectiveMs >= 350) {
			ControllerHandler::lastPerspectiveMs = nowMs;
			mc->options.toggle(&Options::Option::THIRD_PERSON, 1);
		}
	}

	if(Gamepad::wasBindingPressed(ControllerLayout::get(CA_GAME_MENU))) {
		mc->pauseGame(1);
	} else if(_chButtonFree(GP_BTN_START) && Gamepad::wasButtonPressed(GP_BTN_START)) {
		mc->pauseGame(1);
	}
}

void ControllerHandler::tickMenu(Minecraft* mc) {
	float sw = (float)mc->field_1C;
	float sh = (float)mc->field_20;
	if(sw < 1.0f) sw = 1.0f;
	if(sh < 1.0f) sh = 1.0f;

	if(ControllerHandler::cursorX < 0.0f || ControllerHandler::cursorX > sw ||
	   ControllerHandler::cursorY < 0.0f || ControllerHandler::cursorY > sh) {
		ControllerHandler::cursorX = sw * 0.5f;
		ControllerHandler::cursorY = sh * 0.5f;
	}

	float mx = Gamepad::getBindingValue(ControllerLayout::get(CA_MOVE_AXIS_X));
	float my = Gamepad::getBindingValue(ControllerLayout::get(CA_MOVE_AXIS_Y));
	ControllerHandler::clampVec(mx, my);
	float speed = sh * 0.02f * mc->options.controllerCursorSensitivity * ControllerHandler::frameDt;
	if(speed < 0.0f) speed = 0.0f;

	if(mx != 0.0f || my != 0.0f) {
		ControllerHandler::cursorX += mx * speed;
		ControllerHandler::cursorY += my * speed;
		if(ControllerHandler::cursorX < 0.0f) ControllerHandler::cursorX = 0.0f;
		if(ControllerHandler::cursorX > sw) ControllerHandler::cursorX = sw;
		if(ControllerHandler::cursorY < 0.0f) ControllerHandler::cursorY = 0.0f;
		if(ControllerHandler::cursorY > sh) ControllerHandler::cursorY = sh;
		Mouse::feed(0, 0, (int16_t)ControllerHandler::cursorX, (int16_t)ControllerHandler::cursorY);
		ControllerHandler::lastPadCursorMs = getTimeMs();
	}

	int16_t cx = (int16_t)ControllerHandler::cursorX;
	int16_t cy = (int16_t)ControllerHandler::cursorY;

	bool_t confirm = Gamepad::isBindingDown(ControllerLayout::get(CA_JUMP));
	if(confirm && !ControllerHandler::confirmHeld) {
		ControllerHandler::confirmHeld = 1;
		ControllerHandler::lastPadCursorMs = getTimeMs();
		Mouse::feed(1, 1, cx, cy);
	} else if(!confirm && ControllerHandler::confirmHeld) {
		ControllerHandler::confirmHeld = 0;
		Mouse::feed(1, 0, cx, cy);
	}

	if(Gamepad::wasBindingPressed(ControllerLayout::get(CA_SNEAK))) {
		mc->handleBack(0);
		return;
	}

	float scroll = Gamepad::getBindingValue(ControllerLayout::get(CA_LOOK_AXIS_Y));
	bool_t up = Gamepad::isBindingDown(ControllerLayout::get(CA_CYCLE_LEFT)) || scroll < -0.4f;
	bool_t down = Gamepad::isBindingDown(ControllerLayout::get(CA_CYCLE_RIGHT)) || scroll > 0.4f;
	if(_chButtonFree(GP_BTN_DPAD_UP) && Gamepad::isButtonDown(GP_BTN_DPAD_UP)) up = 1;
	if(_chButtonFree(GP_BTN_DPAD_DOWN) && Gamepad::isButtonDown(GP_BTN_DPAD_DOWN)) down = 1;

	if(up || down) {
		if(++ControllerHandler::scrollRepeat % 4 == 1) {
			int32_t gx = cx;
			int32_t gy = cy;
			mc->currentScreen->toGUICoordinate(gx, gy);
			mc->currentScreen->mouseClicked(gx, gy, up ? 4 : 5);
		}
	} else {
		ControllerHandler::scrollRepeat = 0;
	}
}

void ControllerHandler::applyMove(IMoveInput* input, Player* player) {
	if(!input || !player) return;
	if(!Gamepad::connected) return;
	if(ControllerHandler::captureMode) return;

	float mx = ControllerHandler::moveX;
	float my = ControllerHandler::moveY;
	bool_t flying = player->abilities.flying;

	if(mx != 0.0f || my != 0.0f) {
		float strafe = -mx;
		float forward = -my;
		if(ControllerHandler::sneakHeld && !flying) {
			strafe = strafe * 0.3f;
			forward = forward * 0.3f;
		}
		input->strafeInput = strafe;
		input->forwardInput = forward;
	}

	if(ControllerHandler::jumpHeld) {
		input->jumpingMaybe = 1;
		if(flying) {
			input->flyUpPressed = 1;
		}
	}

	if(ControllerHandler::sneakHeld) {
		if(flying) {
			input->flyDownPressed = 1;
			input->sneakingMaybe = 0;
		} else {
			input->sneakingMaybe = 1;
		}
	}

	if(player->isLocalPlayer()) {
		LocalPlayer* lp = (LocalPlayer*)player;
		if(ControllerHandler::sprintHeld && input->forwardInput > 0.5f && !ControllerHandler::sneakHeld) {
			lp->setSprinting(1);
		}
	}
}
