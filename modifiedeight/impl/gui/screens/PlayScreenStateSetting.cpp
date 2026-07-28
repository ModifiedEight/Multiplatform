#include <gui/screens/PlayScreenStateSetting.hpp>
PlayScreenStateSetting::PlayScreenStateSetting() {
	this->field_C = "";
	this->showEditButton = 0;
	this->field_1 = 0;
	this->field_2 = 0;
	this->field_3 = 0;
	this->showExternalButton = 0;
	this->showNewButton = 0;
	this->panel = PlayScreenPanel::NONE;
}
PlayScreenStateSetting::PlayScreenStateSetting(bool_t a3, bool_t a4, bool_t a5, bool_t a6, bool_t a7, bool_t a8, PlayScreenPanel a9) {
	this->field_C = "";
	this->showEditButton = a3;
	this->field_1 = a4;
	this->field_2 = a5;
	this->field_3 = a6;
	this->showExternalButton = a7;
	this->showNewButton = a8;
	this->panel = a9;
}
