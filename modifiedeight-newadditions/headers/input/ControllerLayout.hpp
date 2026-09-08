#pragma once
#include <_types.h>
#include <input/Gamepad.hpp>
#include <string>

enum ControllerAction {
	CA_JUMP = 0,
	CA_DROP_ITEM = 1,
	CA_ATTACK = 2,
	CA_USE_ITEM = 3,
	CA_CRAFTING = 4,
	CA_INVENTORY = 5,
	CA_CYCLE_LEFT = 6,
	CA_CYCLE_RIGHT = 7,
	CA_TOGGLE_PERSPECTIVE = 8,
	CA_SNEAK = 9,
	CA_GAME_MENU = 10,
	CA_CHAT = 11,
	CA_MOVE_AXIS_X = 12,
	CA_MOVE_AXIS_Y = 13,
	CA_LOOK_AXIS_X = 14,
	CA_LOOK_AXIS_Y = 15,
	CA_COUNT = 16
};

struct ControllerLayout {
	static Binding bindings[CA_COUNT];
	static std::string filePath;
	static bool_t loaded;

	static void init(const std::string&);
	static void resetToDefaults();
	static void load();
	static void save();

	static Binding get(int32_t);
	static void set(int32_t, const Binding&);
	static void clearConflicts(int32_t, const Binding&);
	static bool_t isAxisAction(int32_t);
	static const char_t* actionName(int32_t);
	static const char_t* actionKey(int32_t);
	static std::string describe(int32_t);
};
