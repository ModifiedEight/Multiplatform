#pragma once
#include <_types.h>
#include <string>

enum GamepadButton {
	GP_BTN_A = 0,
	GP_BTN_B = 1,
	GP_BTN_X = 2,
	GP_BTN_Y = 3,
	GP_BTN_LB = 4,
	GP_BTN_RB = 5,
	GP_BTN_BACK = 6,
	GP_BTN_START = 7,
	GP_BTN_L3 = 8,
	GP_BTN_R3 = 9,
	GP_BTN_LT = 10,
	GP_BTN_RT = 11,
	GP_BTN_DPAD_UP = 12,
	GP_BTN_DPAD_DOWN = 13,
	GP_BTN_DPAD_LEFT = 14,
	GP_BTN_DPAD_RIGHT = 15,
	GP_BTN_MODE = 16,
	GP_BTN_C = 17,
	GP_BTN_Z = 18,
	GP_BTN_EXTRA_0 = 19,
	GP_BTN_EXTRA_1 = 20,
	GP_BTN_EXTRA_2 = 21,
	GP_BTN_EXTRA_3 = 22,
	GP_BTN_EXTRA_4 = 23,
	GP_BTN_COUNT = 24
};

enum GamepadAxis {
	GP_AXIS_LX = 0,
	GP_AXIS_LY = 1,
	GP_AXIS_RX = 2,
	GP_AXIS_RY = 3,
	GP_AXIS_LT = 4,
	GP_AXIS_RT = 5,
	GP_AXIS_COUNT = 6
};

enum BindingSource {
	BIND_NONE = 0,
	BIND_BUTTON = 1,
	BIND_AXIS_POSITIVE = 2,
	BIND_AXIS_NEGATIVE = 3,
	BIND_AXIS_FULL = 4
};

struct Binding {
	int32_t source;
	int32_t code;
	int32_t raw;

	Binding();
	Binding(int32_t, int32_t);
	bool_t isSet() const;
	bool_t equals(const Binding&) const;
	std::string describe() const;
	std::string serialize() const;
	static Binding parse(const std::string&);
};

struct Gamepad {
	static bool_t connected;
	static bool_t buttons[GP_BTN_COUNT];
	static bool_t pressLatch[GP_BTN_COUNT];
	static bool_t releaseLatch[GP_BTN_COUNT];
	static float axes[GP_AXIS_COUNT];
	static float prevAxes[GP_AXIS_COUNT];
	static int32_t buttonRaw[GP_BTN_COUNT];
	static int32_t axisRaw[GP_AXIS_COUNT];
	static float deadzone;
	static float triggerThreshold;

	static void reset();
	static void beginFrame();
	static void setConnected(bool_t);
	static void feedButton(int32_t, bool_t, int32_t raw = -1);
	static void feedAxis(int32_t, float, int32_t raw = -1);
	static void feedHat(bool_t, bool_t, bool_t, bool_t);
	static void syncDerivedButtons();
	static int32_t mapRawButton(int32_t, int32_t);
	static int32_t mapSdlButton(int32_t, int32_t, int32_t);

	static float applyDeadzone(float);
	static bool_t isButtonDown(int32_t);
	static bool_t wasButtonPressed(int32_t);
	static bool_t wasButtonReleased(int32_t);
	static float getAxis(int32_t);
	static float getPrevAxis(int32_t);

	static bool_t isBindingDown(const Binding&);
	static bool_t wasBindingPressed(const Binding&);
	static bool_t wasBindingReleased(const Binding&);
	static float getBindingValue(const Binding&);
	static bool_t detectPressedBinding(Binding&, bool_t);
	static const char_t* buttonName(int32_t);
	static const char_t* axisName(int32_t);
};
