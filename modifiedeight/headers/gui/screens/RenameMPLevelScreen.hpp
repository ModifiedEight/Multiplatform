#pragma once
#include <gui/Screen.hpp>
#include <memory>

struct Label;
struct TextBox;
struct NinePatchLayer;

struct RenameMPLevelScreen: Screen
{
	std::shared_ptr<Button> closeScreenButton;
	std::shared_ptr<Button> field_5C;
	std::shared_ptr<Button> renameButton;
	std::shared_ptr<Label> worldNameLabel;
	std::shared_ptr<TextBox> worldNameTextBox;
	std::shared_ptr<Label> gameModeLabel;
	class CategoryButton* survivalButton;
	class CategoryButton* creativeButton;
	Button* selectedGameMode;
	std::shared_ptr<Label> timeSliderLabel;
	std::shared_ptr<struct Slider> timeSlider;
	std::shared_ptr<Label> timeFreezeLabel;
	class CategoryButton* timeFreezeOnButton;
	class CategoryButton* timeFreezeOffButton;
	Button* selectedTimeFreeze;
	std::shared_ptr<NinePatchLayer> field_A4;
	std::shared_ptr<NinePatchLayer> btnBgUnpressed;
	std::shared_ptr<NinePatchLayer> btnBgPressed;
	
	std::shared_ptr<Button> makeInfiniteButton;
	std::shared_ptr<Label> coordsLabel;
	std::shared_ptr<TextBox> xTextBox;
	std::shared_ptr<TextBox> yTextBox;
	std::shared_ptr<TextBox> zTextBox;
	int32_t originalGeneratorVersion;
	struct CompoundTag* playerTagBackup;
	
	std::string name;
	std::string displayName;

	RenameMPLevelScreen(const std::string& folderName, const std::string& displayName = "");
	void closeScreen();

	virtual ~RenameMPLevelScreen();
	virtual void render(int32_t, int32_t, float);
	virtual void init();
	virtual void setupPositions();
	virtual bool_t handleBackEvent(bool_t);
	virtual void buttonClicked(Button*);
};
