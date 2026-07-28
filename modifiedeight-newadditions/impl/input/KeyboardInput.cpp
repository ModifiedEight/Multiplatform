#include <input/KeyboardInput.hpp>
#include <Options.hpp>
#include <entity/Player.hpp>
#include <entity/LocalPlayer.hpp>

KeyboardInput::KeyboardInput(Options* a2) {
	this->strafeInput = 0;
	this->forwardInput = 0;
	this->idkWhatIsThis = 0;
	this->jumpingMaybe = 0;
	this->sneakingMaybe = 0;
	this->flyUpPressed = this->flyDownPressed = this->idkWhatIsThis2 = 0;
	for(int32_t i = 0; i != 10; ++i) {
		this->inputs[i] = 0;
	}
	this->options = a2;
	this->forwardTapTimer = 0;
	this->forwardDoubleTapDetected = false;
}

KeyboardInput::~KeyboardInput() {
}
void KeyboardInput::tick(Player* a2) {
	this->strafeInput = 0.0;
	this->forwardInput = 0.0;

	if(this->inputs[0]) {
		this->forwardInput = 1.0;
	}
	if(this->inputs[1]) {
		this->forwardInput = this->forwardInput - 1.0;
	}
	if(this->inputs[2]) {
		this->strafeInput = 1.0;
	}
	if(this->inputs[3]) {
		this->strafeInput = this->strafeInput - 1.0;
	}
	this->jumpingMaybe = this->inputs[4];
	this->sneakingMaybe = this->inputs[5];
	if (a2->abilities.flying) {
		this->flyUpPressed = this->inputs[4];
		this->flyDownPressed = this->inputs[5];
		this->sneakingMaybe = 0;
	} else {
		this->flyUpPressed = 0;
		this->flyDownPressed = 0;
	}
	if(this->sneakingMaybe) {
		this->strafeInput = this->strafeInput * 0.3;
		this->forwardInput = this->forwardInput * 0.3;
	}
	// handle double-tap forward -> sprint
	if(this->forwardTapTimer > 0) --this->forwardTapTimer;
	if(this->forwardDoubleTapDetected) {
		this->forwardDoubleTapDetected = false;
		if(a2 && a2->isLocalPlayer()) {
			((LocalPlayer*)a2)->setSprinting(true);
		}
	}
	// cancel sprint if not moving forward or if sneaking
	if(a2 && a2->isLocalPlayer()) {
		LocalPlayer* lp = (LocalPlayer*)a2;
		if(this->forwardInput <= 0.0f || this->sneakingMaybe) {
			lp->setSprinting(false);
		}
	}
	if(this->inputs[6]) {
		a2->startCrafting((int32_t)a2->posX, (int32_t)a2->posY, (int32_t)a2->posZ, 0);
	}
}
void KeyboardInput::setKey(int32_t a2, bool_t a3) {
	Options* options; // r4
	int32_t v4;		  // r3
	int32_t keyCode;  // r5
	int32_t v6;		  // r4

	options = this->options;
	if(a2 == options->keyForward.keyCode) {
		v4 = 0;
	} else {
		v4 = -1;
	}
	if(a2 == options->keyBack.keyCode) {
		v4 = 1;
	}
	if(a2 == options->keyLeft.keyCode) {
		v4 = 2;
	}
	if(a2 == options->keyRight.keyCode) {
		v4 = 3;
	}
	if(a2 == options->keyJump.keyCode) {
		v4 = 4;
	}
	keyCode = options->keySneak.keyCode;
	v6 = options->keyCrafting.keyCode;
	if(a2 == keyCode || a2 == 10) {
		v4 = 5;
	}
	// detect double-tap on forward key (press)
	if(v4 == 0 && a3) {
		if(this->forwardTapTimer > 0) {
			this->forwardDoubleTapDetected = true;
		} else {
			this->forwardTapTimer = 10; // ~10 ticks window
		}
	}
	if(a2 == v6) {
		v4 = 6;
		goto LABEL_18;
	}
	if(v4 != -1) {
LABEL_18:
		this->inputs[v4] = a3;
	}
}
void KeyboardInput::releaseAllKeys() {
	this->strafeInput = 0;
	this->forwardInput = 0;
	for(int32_t i = 0; i != 10; ++i) {
		this->inputs[i] = 0;
	}
	this->flyDownPressed = 0;
	this->flyUpPressed = 0;
#ifdef DIFFERENTTWEAKS
	//force release keys to prevent jumping/sneaking in guis
	this->jumpingMaybe = 0;
	this->sneakingMaybe = 0;
#endif
	this->forwardTapTimer = 0;
	this->forwardDoubleTapDetected = false;
}
