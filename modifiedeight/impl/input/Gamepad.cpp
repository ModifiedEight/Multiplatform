#include <input/Gamepad.hpp>
#include <stdio.h>
#include <stdlib.h>

bool_t Gamepad::connected = 0;
bool_t Gamepad::buttons[GP_BTN_COUNT];
bool_t Gamepad::pressLatch[GP_BTN_COUNT];
bool_t Gamepad::releaseLatch[GP_BTN_COUNT];
float Gamepad::axes[GP_AXIS_COUNT];
float Gamepad::prevAxes[GP_AXIS_COUNT];
int32_t Gamepad::buttonRaw[GP_BTN_COUNT];
int32_t Gamepad::axisRaw[GP_AXIS_COUNT];
float Gamepad::deadzone = 0.18f;
float Gamepad::triggerThreshold = 0.35f;

static const char_t* _gpButtonNames[GP_BTN_COUNT] = {
	"A", "B", "X", "Y",
	"LB", "RB", "Back", "Start",
	"L3", "R3", "LT", "RT",
	"D-Pad Up", "D-Pad Down", "D-Pad Left", "D-Pad Right",
	"Mode", "C", "Z",
	"Button 19", "Button 20", "Button 21", "Button 22", "Button 23"
};

static const char_t* _gpAxisNames[GP_AXIS_COUNT] = {
	"Left Stick X", "Left Stick Y",
	"Right Stick X", "Right Stick Y",
	"Left Trigger", "Right Trigger"
};

Binding::Binding() {
	this->source = BIND_NONE;
	this->code = 0;
	this->raw = -1;
}

Binding::Binding(int32_t a2, int32_t a3) {
	this->source = a2;
	this->code = a3;
	this->raw = -1;
}

bool_t Binding::isSet() const {
	return this->source != BIND_NONE;
}

bool_t Binding::equals(const Binding& a2) const {
	return this->source == a2.source && this->code == a2.code;
}

std::string Binding::describe() const {
	char_t buf[32];
	switch(this->source) {
		case BIND_BUTTON:
			if(this->code == GP_BTN_LT) {
				return "Trigger 0";
			}
			if(this->code == GP_BTN_RT) {
				return "Trigger 1";
			}
			sprintf(buf, "Button %d", this->raw >= 0 ? this->raw : this->code);
			return buf;
		case BIND_AXIS_POSITIVE:
		case BIND_AXIS_NEGATIVE:
			if(this->code == GP_AXIS_LT) {
				return "Trigger 0";
			}
			if(this->code == GP_AXIS_RT) {
				return "Trigger 1";
			}
			sprintf(buf, "Axis %d %c", this->code, this->source == BIND_AXIS_POSITIVE ? '+' : '-');
			return buf;
		case BIND_AXIS_FULL:
			if(this->code == GP_AXIS_LT) {
				return "Trigger 0";
			}
			if(this->code == GP_AXIS_RT) {
				return "Trigger 1";
			}
			sprintf(buf, "Axis %d", this->code);
			return buf;
		default:
			return "Unbound";
	}
}

std::string Binding::serialize() const {
	char_t buf[32];
	if(this->raw >= 0) {
		sprintf(buf, "%d/%d/%d", this->source, this->code, this->raw);
	} else {
		sprintf(buf, "%d/%d", this->source, this->code);
	}
	return buf;
}

Binding Binding::parse(const std::string& a1) {
	Binding res;
	if(a1.empty()) return res;
	size_t sep = a1.find('/');
	if(sep == std::string::npos) return res;
	int32_t src = atoi(a1.substr(0, sep).c_str());
	std::string rest = a1.substr(sep + 1);
	int32_t code = atoi(rest.c_str());
	int32_t raw = -1;
	size_t sep2 = rest.find('/');
	if(sep2 != std::string::npos) {
		raw = atoi(rest.substr(sep2 + 1).c_str());
	}
	if(src <= BIND_NONE || src > BIND_AXIS_FULL) return res;
	if(src == BIND_BUTTON) {
		if(code < 0 || code >= GP_BTN_COUNT) return res;
	} else {
		if(code < 0 || code >= GP_AXIS_COUNT) return res;
	}
	res.source = src;
	res.code = code;
	res.raw = raw;
	return res;
}

void Gamepad::reset() {
	for(int32_t i = 0; i < GP_BTN_COUNT; ++i) {
		Gamepad::buttons[i] = 0;
		Gamepad::pressLatch[i] = 0;
		Gamepad::releaseLatch[i] = 0;
		Gamepad::buttonRaw[i] = -1;
	}
	for(int32_t i = 0; i < GP_AXIS_COUNT; ++i) {
		Gamepad::axes[i] = 0.0f;
		Gamepad::prevAxes[i] = 0.0f;
		Gamepad::axisRaw[i] = -1;
	}
}

void Gamepad::beginFrame() {
	for(int32_t i = 0; i < GP_BTN_COUNT; ++i) {
		Gamepad::pressLatch[i] = 0;
		Gamepad::releaseLatch[i] = 0;
	}
	for(int32_t i = 0; i < GP_AXIS_COUNT; ++i) {
		Gamepad::prevAxes[i] = Gamepad::axes[i];
	}
}

void Gamepad::setConnected(bool_t a1) {
	if(!a1 && Gamepad::connected) {
		Gamepad::reset();
	}
	Gamepad::connected = a1;
}

void Gamepad::feedButton(int32_t a1, bool_t a2, int32_t raw) {
	if(a1 < 0 || a1 >= GP_BTN_COUNT) return;
	bool_t down = a2 ? 1 : 0;
	if(down && !Gamepad::buttons[a1]) {
		Gamepad::pressLatch[a1] = 1;
	} else if(!down && Gamepad::buttons[a1]) {
		Gamepad::releaseLatch[a1] = 1;
	}
	Gamepad::buttons[a1] = down;
	if(raw >= 0) {
		Gamepad::buttonRaw[a1] = raw;
	}
}

void Gamepad::feedAxis(int32_t a1, float a2, int32_t raw) {
	if(a1 < 0 || a1 >= GP_AXIS_COUNT) return;
	if(a2 > 1.0f) a2 = 1.0f;
	if(a2 < -1.0f) a2 = -1.0f;
	Gamepad::axes[a1] = a2;
	if(raw >= 0) {
		Gamepad::axisRaw[a1] = raw;
	}
}

void Gamepad::feedHat(bool_t up, bool_t down, bool_t left, bool_t right) {
	Gamepad::feedButton(GP_BTN_DPAD_UP, up);
	Gamepad::feedButton(GP_BTN_DPAD_DOWN, down);
	Gamepad::feedButton(GP_BTN_DPAD_LEFT, left);
	Gamepad::feedButton(GP_BTN_DPAD_RIGHT, right);
}

void Gamepad::syncDerivedButtons() {
	if(Gamepad::axes[GP_AXIS_LT] > Gamepad::triggerThreshold) {
		Gamepad::feedButton(GP_BTN_LT, 1);
	} else if(Gamepad::axes[GP_AXIS_LT] < Gamepad::triggerThreshold * 0.5f) {
		Gamepad::feedButton(GP_BTN_LT, 0);
	}
	if(Gamepad::axes[GP_AXIS_RT] > Gamepad::triggerThreshold) {
		Gamepad::feedButton(GP_BTN_RT, 1);
	} else if(Gamepad::axes[GP_AXIS_RT] < Gamepad::triggerThreshold * 0.5f) {
		Gamepad::feedButton(GP_BTN_RT, 0);
	}
}

int32_t Gamepad::mapRawButton(int32_t keyCode, int32_t fallbackIndex) {
	switch(keyCode) {
		case 96: return GP_BTN_A;
		case 97: return GP_BTN_B;
		case 98: return GP_BTN_C;
		case 99: return GP_BTN_X;
		case 100: return GP_BTN_Y;
		case 101: return GP_BTN_Z;
		case 102: return GP_BTN_LB;
		case 103: return GP_BTN_RB;
		case 104: return GP_BTN_LT;
		case 105: return GP_BTN_RT;
		case 106: return GP_BTN_L3;
		case 107: return GP_BTN_R3;
		case 108: return GP_BTN_START;
		case 109: return GP_BTN_BACK;
		case 110: return GP_BTN_MODE;
		case 19: return GP_BTN_DPAD_UP;
		case 20: return GP_BTN_DPAD_DOWN;
		case 21: return GP_BTN_DPAD_LEFT;
		case 22: return GP_BTN_DPAD_RIGHT;
		case 23: return GP_BTN_A;
		case 4: return GP_BTN_B;
		case 82: return GP_BTN_START;
		default:
			if(keyCode >= 188 && keyCode <= 202) {
				int32_t idx = GP_BTN_EXTRA_0 + (keyCode - 188);
				if(idx < GP_BTN_COUNT) return idx;
			}
			return fallbackIndex;
	}
}

int32_t Gamepad::mapSdlButton(int32_t index, int32_t numButtons, int32_t numAxes) {
	if(index < 0) return -1;
	if(numButtons >= 11 && numAxes >= 6) {
		switch(index) {
			case 8: return GP_BTN_MODE;
			case 9: return GP_BTN_L3;
			case 10: return GP_BTN_R3;
			default: break;
		}
	}
	if(index >= GP_BTN_COUNT) return -1;
	return index;
}

float Gamepad::applyDeadzone(float a1) {
	float mag = a1 < 0.0f ? -a1 : a1;
	if(mag < Gamepad::deadzone) return 0.0f;
	float scaled = (mag - Gamepad::deadzone) / (1.0f - Gamepad::deadzone);
	if(scaled > 1.0f) scaled = 1.0f;
	return a1 < 0.0f ? -scaled : scaled;
}

bool_t Gamepad::isButtonDown(int32_t a1) {
	if(a1 < 0 || a1 >= GP_BTN_COUNT) return 0;
	return Gamepad::buttons[a1];
}

bool_t Gamepad::wasButtonPressed(int32_t a1) {
	if(a1 < 0 || a1 >= GP_BTN_COUNT) return 0;
	return Gamepad::pressLatch[a1];
}

bool_t Gamepad::wasButtonReleased(int32_t a1) {
	if(a1 < 0 || a1 >= GP_BTN_COUNT) return 0;
	return Gamepad::releaseLatch[a1];
}

float Gamepad::getAxis(int32_t a1) {
	if(a1 < 0 || a1 >= GP_AXIS_COUNT) return 0.0f;
	return Gamepad::applyDeadzone(Gamepad::axes[a1]);
}

float Gamepad::getPrevAxis(int32_t a1) {
	if(a1 < 0 || a1 >= GP_AXIS_COUNT) return 0.0f;
	return Gamepad::applyDeadzone(Gamepad::prevAxes[a1]);
}

bool_t Gamepad::isBindingDown(const Binding& a1) {
	switch(a1.source) {
		case BIND_BUTTON:
			return Gamepad::isButtonDown(a1.code);
		case BIND_AXIS_POSITIVE:
			return Gamepad::getAxis(a1.code) > Gamepad::triggerThreshold;
		case BIND_AXIS_NEGATIVE:
			return Gamepad::getAxis(a1.code) < -Gamepad::triggerThreshold;
		case BIND_AXIS_FULL:
			{
				float v = Gamepad::getAxis(a1.code);
				return (v > Gamepad::triggerThreshold) || (v < -Gamepad::triggerThreshold);
			}
		default:
			return 0;
	}
}

static bool_t _axisWasDown(int32_t source, int32_t code) {
	float v = Gamepad::getPrevAxis(code);
	switch(source) {
		case BIND_AXIS_POSITIVE:
			return v > Gamepad::triggerThreshold;
		case BIND_AXIS_NEGATIVE:
			return v < -Gamepad::triggerThreshold;
		case BIND_AXIS_FULL:
			return (v > Gamepad::triggerThreshold) || (v < -Gamepad::triggerThreshold);
		default:
			return 0;
	}
}

bool_t Gamepad::wasBindingPressed(const Binding& a1) {
	if(a1.source == BIND_BUTTON) {
		return Gamepad::wasButtonPressed(a1.code);
	}
	if(a1.source == BIND_NONE) {
		return 0;
	}
	if(a1.code == GP_AXIS_LT) {
		return Gamepad::wasButtonPressed(GP_BTN_LT);
	}
	if(a1.code == GP_AXIS_RT) {
		return Gamepad::wasButtonPressed(GP_BTN_RT);
	}
	return Gamepad::isBindingDown(a1) && !_axisWasDown(a1.source, a1.code);
}

bool_t Gamepad::wasBindingReleased(const Binding& a1) {
	if(a1.source == BIND_BUTTON) {
		return Gamepad::wasButtonReleased(a1.code);
	}
	if(a1.source == BIND_NONE) {
		return 0;
	}
	if(a1.code == GP_AXIS_LT) {
		return Gamepad::wasButtonReleased(GP_BTN_LT);
	}
	if(a1.code == GP_AXIS_RT) {
		return Gamepad::wasButtonReleased(GP_BTN_RT);
	}
	return !Gamepad::isBindingDown(a1) && _axisWasDown(a1.source, a1.code);
}

float Gamepad::getBindingValue(const Binding& a1) {
	switch(a1.source) {
		case BIND_BUTTON:
			return Gamepad::isButtonDown(a1.code) ? 1.0f : 0.0f;
		case BIND_AXIS_POSITIVE:
			{
				float v = Gamepad::getAxis(a1.code);
				return v > 0.0f ? v : 0.0f;
			}
		case BIND_AXIS_NEGATIVE:
			{
				float v = Gamepad::getAxis(a1.code);
				return v < 0.0f ? -v : 0.0f;
			}
		case BIND_AXIS_FULL:
			return Gamepad::getAxis(a1.code);
		default:
			return 0.0f;
	}
}

bool_t Gamepad::detectPressedBinding(Binding& out, bool_t wantAxis) {
	if(wantAxis) {
		for(int32_t i = 0; i < GP_AXIS_COUNT; ++i) {
			if(i == GP_AXIS_LT || i == GP_AXIS_RT) continue;
			float v = Gamepad::getAxis(i);
			if(v > 0.6f || v < -0.6f) {
				out.source = BIND_AXIS_FULL;
				out.code = i;
				out.raw = Gamepad::axisRaw[i];
				return 1;
			}
		}
		return 0;
	}
	for(int32_t i = 0; i < GP_BTN_COUNT; ++i) {
		if(Gamepad::wasButtonPressed(i)) {
			out.source = BIND_BUTTON;
			out.code = i;
			out.raw = Gamepad::buttonRaw[i];
			return 1;
		}
	}
	for(int32_t i = 0; i < GP_AXIS_COUNT; ++i) {
		if(i == GP_AXIS_LT || i == GP_AXIS_RT) continue;
		float v = Gamepad::getAxis(i);
		float p = Gamepad::getPrevAxis(i);
		if(v > 0.7f && p <= 0.7f) {
			out.source = BIND_AXIS_POSITIVE;
			out.code = i;
			out.raw = Gamepad::axisRaw[i];
			return 1;
		}
		if(v < -0.7f && p >= -0.7f) {
			out.source = BIND_AXIS_NEGATIVE;
			out.code = i;
			out.raw = Gamepad::axisRaw[i];
			return 1;
		}
	}
	return 0;
}

const char_t* Gamepad::buttonName(int32_t a1) {
	if(a1 < 0 || a1 >= GP_BTN_COUNT) return "Unbound";
	return _gpButtonNames[a1];
}

const char_t* Gamepad::axisName(int32_t a1) {
	if(a1 < 0 || a1 >= GP_AXIS_COUNT) return "Unbound";
	return _gpAxisNames[a1];
}
