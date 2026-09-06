#include <input/ControllerLayout.hpp>
#include <stdio.h>
#include <string.h>

Binding ControllerLayout::bindings[CA_COUNT];
std::string ControllerLayout::filePath = "";
bool_t ControllerLayout::loaded = 0;

static const char_t* _caNames[CA_COUNT] = {
	"Jump/Fly Up",
	"Drop Item",
	"Attack/Destroy",
	"Use Item/Place Block",
	"Crafting",
	"Inventory",
	"Cycle Item Left",
	"Cycle Item Right",
	"Toggle Perspective",
	"Sneak/Fly Down",
	"Game Menu",
	"Move Left/Right",
	"Move Forward/Back",
	"Look Left/Right",
	"Look Up/Down"
};

static const char_t* _caKeys[CA_COUNT] = {
	"jump",
	"drop",
	"attack",
	"use",
	"crafting",
	"inventory",
	"cycleleft",
	"cycleright",
	"perspective",
	"sneak",
	"gamemenu",
	"movex",
	"movey",
	"lookx",
	"looky"
};

void ControllerLayout::init(const std::string& a1) {
	ControllerLayout::filePath = a1 + "/controller_layout.txt";
	ControllerLayout::resetToDefaults();
	ControllerLayout::load();
}

void ControllerLayout::resetToDefaults() {
	ControllerLayout::bindings[CA_JUMP] = Binding(BIND_BUTTON, GP_BTN_A);
	ControllerLayout::bindings[CA_DROP_ITEM] = Binding(BIND_BUTTON, GP_BTN_R3);
	ControllerLayout::bindings[CA_ATTACK] = Binding(BIND_AXIS_POSITIVE, GP_AXIS_RT);
	ControllerLayout::bindings[CA_USE_ITEM] = Binding(BIND_AXIS_POSITIVE, GP_AXIS_LT);
	ControllerLayout::bindings[CA_CRAFTING] = Binding(BIND_BUTTON, GP_BTN_X);
	ControllerLayout::bindings[CA_INVENTORY] = Binding(BIND_BUTTON, GP_BTN_Y);
	ControllerLayout::bindings[CA_CYCLE_LEFT] = Binding(BIND_BUTTON, GP_BTN_LB);
	ControllerLayout::bindings[CA_CYCLE_RIGHT] = Binding(BIND_BUTTON, GP_BTN_RB);
	ControllerLayout::bindings[CA_TOGGLE_PERSPECTIVE] = Binding(BIND_BUTTON, GP_BTN_DPAD_DOWN);
	ControllerLayout::bindings[CA_SNEAK] = Binding(BIND_BUTTON, GP_BTN_B);
	ControllerLayout::bindings[CA_MOVE_AXIS_X] = Binding(BIND_AXIS_FULL, GP_AXIS_LX);
	ControllerLayout::bindings[CA_MOVE_AXIS_Y] = Binding(BIND_AXIS_FULL, GP_AXIS_LY);
	ControllerLayout::bindings[CA_LOOK_AXIS_X] = Binding(BIND_AXIS_FULL, GP_AXIS_RX);
	ControllerLayout::bindings[CA_LOOK_AXIS_Y] = Binding(BIND_AXIS_FULL, GP_AXIS_RY);
	ControllerLayout::bindings[CA_GAME_MENU] = Binding(BIND_BUTTON, GP_BTN_START);
}

void ControllerLayout::load() {
	if(ControllerLayout::filePath.empty()) return;
	FILE* f = fopen(ControllerLayout::filePath.c_str(), "r");
	if(!f) {
		ControllerLayout::loaded = 1;
		return;
	}
	char_t line[128];
	while(fgets(line, sizeof(line), f)) {
		size_t len = strlen(line);
		while(len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r' || line[len - 1] == ' ' || line[len - 1] == '\t')) {
			line[--len] = 0;
		}
		if(len < 3) continue;
		std::string s = line;
		size_t sep = s.find(':');
		if(sep == std::string::npos) continue;
		std::string key = s.substr(0, sep);
		std::string val = s.substr(sep + 1);
		for(int32_t i = 0; i < CA_COUNT; ++i) {
			if(key == _caKeys[i]) {
				ControllerLayout::bindings[i] = Binding::parse(val);
				break;
			}
		}
	}
	fclose(f);
	ControllerLayout::loaded = 1;
}

void ControllerLayout::save() {
	if(ControllerLayout::filePath.empty()) return;
	FILE* f = fopen(ControllerLayout::filePath.c_str(), "w");
	if(!f) return;
	for(int32_t i = 0; i < CA_COUNT; ++i) {
		fprintf(f, "%s:%s\n", _caKeys[i], ControllerLayout::bindings[i].serialize().c_str());
	}
	fclose(f);
}

Binding ControllerLayout::get(int32_t a1) {
	if(a1 < 0 || a1 >= CA_COUNT) return Binding();
	return ControllerLayout::bindings[a1];
}

void ControllerLayout::set(int32_t a1, const Binding& a2) {
	if(a1 < 0 || a1 >= CA_COUNT) return;
	ControllerLayout::bindings[a1] = a2;
}

void ControllerLayout::clearConflicts(int32_t a1, const Binding& a2) {
	if(!a2.isSet()) return;
	bool_t axisTarget = ControllerLayout::isAxisAction(a1);
	for(int32_t i = 0; i < CA_COUNT; ++i) {
		if(i == a1) continue;
		if(ControllerLayout::isAxisAction(i) != axisTarget) continue;
		if(ControllerLayout::bindings[i].equals(a2)) {
			ControllerLayout::bindings[i] = Binding();
		}
	}
}

bool_t ControllerLayout::isAxisAction(int32_t a1) {
	return a1 >= CA_MOVE_AXIS_X && a1 <= CA_LOOK_AXIS_Y;
}

const char_t* ControllerLayout::actionName(int32_t a1) {
	if(a1 < 0 || a1 >= CA_COUNT) return "";
	return _caNames[a1];
}

const char_t* ControllerLayout::actionKey(int32_t a1) {
	if(a1 < 0 || a1 >= CA_COUNT) return "";
	return _caKeys[a1];
}

std::string ControllerLayout::describe(int32_t a1) {
	if(a1 < 0 || a1 >= CA_COUNT) return "Unbound";
	return ControllerLayout::bindings[a1].describe();
}
