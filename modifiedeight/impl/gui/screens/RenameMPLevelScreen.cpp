#include <gui/screens/RenameMPLevelScreen.hpp>
#include <Minecraft.hpp>
#include <level/storage/LevelStorageSource.hpp>
#include <util/Util.hpp>
#include <gui/elements/Label.hpp>
#include <gui/elements/TextBox.hpp>
#include <gui/elements/Slider.hpp>
#include <gui/buttons/CategoryButton.hpp>
#include <level/storage/ExternalFileLevelStorage.hpp>
#include <level/storage/LevelData.hpp>
#include <level/storage/ExternalFileLevelStorageSource.hpp>
#include <gui/screens/PlayScreen.hpp>
#include <rendering/Tesselator.hpp>
#include <gui/NinePatchFactory.hpp>
#include <util/IntRectangle.hpp>
#include <stdlib.h>
#include <nbt/CompoundTag.hpp>
#include <nbt/ListTag.hpp>
#include <nbt/FloatTag.hpp>

RenameMPLevelScreen::RenameMPLevelScreen(const std::string& folderName, const std::string& displayName)
	: name(folderName)
	, displayName(displayName)
	, playerTagBackup(nullptr)
	, originalGeneratorVersion(0) {
}

void RenameMPLevelScreen::closeScreen() {
	this->minecraft->setScreen(new PlayScreen(1));
}

RenameMPLevelScreen::~RenameMPLevelScreen() {
	delete this->survivalButton;
	delete this->creativeButton;
	delete this->timeFreezeOnButton;
	delete this->timeFreezeOffButton;
	if (this->playerTagBackup) delete this->playerTagBackup;
}

void RenameMPLevelScreen::render(int32_t a2, int32_t a3, float a4) {
	if (this->timeSlider) {
		float progress = this->timeSlider->progress;
		int32_t hour = (int32_t)(progress * 24.0f + 6.0f) % 24;
		int32_t minute = (int32_t)(progress * 1440.0f) % 60;
		const char* period = "Day";
		if (hour >= 18 && hour < 20) {
			period = "Sunset";
		} else if (hour >= 20 || hour < 5) {
			period = "Night";
		} else if (hour >= 5 && hour < 6) {
			period = "Sunrise";
		}
		char buf[64];
		sprintf(buf, "Time of Day: %02d:%02d (%s)", hour, minute, period);
		this->timeSliderLabel->setText(buf);
	}
	if(this->supppressedBySubWindow()) {
		this->renderBackground(0);
	} else {
		this->renderMenuBackground(a4);
		this->field_A4->draw(Tesselator::instance, 5, this->field_5C->height + 5);
	}
	Screen::render(a2, a3, a4);
}

void RenameMPLevelScreen::init() {
	this->field_5C = std::shared_ptr<Button>(new Touch::THeader(0, "Edit World"));
	this->closeScreenButton = std::shared_ptr<Button>(new Touch::TButton(1, "Back", this->minecraft));
	this->renameButton = std::shared_ptr<Button>(new Touch::TButton(2, "Save", this->minecraft));
	this->closeScreenButton->width = 38;
	this->closeScreenButton->height = 18;
	
	this->worldNameLabel = std::shared_ptr<Label>(new Label("World Name", this->minecraft, -1, 0, 0, 0, 1));
	const char* extAscii = TextBox::extendedAcsii ? TextBox::extendedAcsii : " !\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
	this->worldNameTextBox = std::shared_ptr<TextBox>(new TextBox(this->minecraft, "World Name", 16, extAscii, strlen(extAscii), 0, 0, 0, 0));
	this->worldNameTextBox->setText(this->displayName);

	NinePatchFactory factory(this->minecraft->texturesPtr, "gui/spritesheet.png");
	this->field_A4 = std::shared_ptr<NinePatchLayer>(factory.createSymmetrical(IntRectangle{34, 43, 14, 14}, 3, 3, 32, 32));
	this->btnBgUnpressed = std::shared_ptr<NinePatchLayer>(factory.createSymmetrical(IntRectangle{8, 32, 8, 8}, 2, 2, 38, 26));
	this->btnBgPressed = std::shared_ptr<NinePatchLayer>(factory.createSymmetrical(IntRectangle{0, 32, 8, 8}, 2, 2, 38, 26));
	
	this->gameModeLabel = std::shared_ptr<Label>(new Label("Game Mode", this->minecraft, -1, 0, 0, 0, 1));
	this->survivalButton = new CategoryButton("Survival", 3, this->btnBgUnpressed.get(), this->btnBgPressed.get(), &this->selectedGameMode);
	this->creativeButton = new CategoryButton("Creative", 4, this->btnBgUnpressed.get(), this->btnBgPressed.get(), &this->selectedGameMode);
	
	this->timeSliderLabel = std::shared_ptr<Label>(new Label("Time of Day", this->minecraft, -1, 0, 0, 0, 1));
	this->timeSlider = std::shared_ptr<Slider>(new Slider(this->minecraft, nullptr, 0.0f, 1.0f));

	this->timeFreezeLabel = std::shared_ptr<Label>(new Label("Time Freeze", this->minecraft, -1, 0, 0, 0, 1));
	this->timeFreezeOnButton = new CategoryButton("On", 5, this->btnBgUnpressed.get(), this->btnBgPressed.get(), &this->selectedTimeFreeze);
	this->timeFreezeOffButton = new CategoryButton("Off", 6, this->btnBgUnpressed.get(), this->btnBgPressed.get(), &this->selectedTimeFreeze);

	this->makeInfiniteButton = std::shared_ptr<Button>(new Touch::TButton(7, "Make Infinite", this->minecraft));
	this->coordsLabel = std::shared_ptr<Label>(new Label("Coordinates", this->minecraft, -1, 0, 0, 0, 1));
	this->xTextBox = std::shared_ptr<TextBox>(new TextBox(this->minecraft, "X", 16, "-0123456789.", 12, 0, 0, 0, 0));
	this->yTextBox = std::shared_ptr<TextBox>(new TextBox(this->minecraft, "Y", 16, "-0123456789.", 12, 0, 0, 0, 0));
	this->zTextBox = std::shared_ptr<TextBox>(new TextBox(this->minecraft, "Z", 16, "-0123456789.", 12, 0, 0, 0, 0));

	LevelData data;
	LevelStorageSource* ls = this->minecraft->getLevelSource();
	ExternalFileLevelStorageSource* els = (ExternalFileLevelStorageSource*)ls;
	std::string path = els->getFullPath(this->name);
	int32_t levelTime = 0;
	bool timeFrozen = false;
	if (ExternalFileLevelStorage::readLevelData(path, data)) {
		this->originalGeneratorVersion = data.getGeneratorVersion();
		CompoundTag* pTag = data.getLoadedPlayerTag();
		if (pTag) {
			this->playerTagBackup = (CompoundTag*)pTag->copy();
			ListTag* posList = this->playerTagBackup->getList("Pos");
			if (posList && posList->value.size() >= 3) {
				char buf[32];
				sprintf(buf, "%.1f", ((FloatTag*)posList->value[0])->value);
				this->xTextBox->setText(buf);
				sprintf(buf, "%.1f", ((FloatTag*)posList->value[1])->value);
				this->yTextBox->setText(buf);
				sprintf(buf, "%.1f", ((FloatTag*)posList->value[2])->value);
				this->zTextBox->setText(buf);
			}
		}
		if(data.getGameType() == 1) {
			this->selectedGameMode = this->creativeButton;
		} else {
			this->selectedGameMode = this->survivalButton;
		}
		timeFrozen = data.stopTime >= 0;
		if (timeFrozen) {
			levelTime = data.stopTime % 24000;
		} else {
			levelTime = data.getTime() % 24000;
		}
		if (levelTime < 0) levelTime = 0;
	} else {
		this->selectedGameMode = this->survivalButton;
	}

	float progress = (float)levelTime / 24000.0f;
	if (progress < 0.0f) progress = 0.0f;
	if (progress > 1.0f) progress = 1.0f;
	this->timeSlider->progress = progress;
	this->selectedTimeFreeze = timeFrozen ? this->timeFreezeOnButton : this->timeFreezeOffButton;

	this->buttons.emplace_back(this->field_5C.get());
	this->buttons.emplace_back(this->closeScreenButton.get());
	this->buttons.emplace_back(this->renameButton.get());
	this->buttons.emplace_back(this->survivalButton);
	this->buttons.emplace_back(this->creativeButton);
	this->buttons.emplace_back(this->timeFreezeOnButton);
	this->buttons.emplace_back(this->timeFreezeOffButton);
	this->buttons.emplace_back(this->makeInfiniteButton.get());
	
	this->elements.emplace_back(this->worldNameLabel.get());
	this->elements.emplace_back(this->worldNameTextBox.get());
	this->elements.emplace_back(this->gameModeLabel.get());
	this->elements.emplace_back(this->timeSliderLabel.get());
	this->elements.emplace_back(this->timeSlider.get());
	this->elements.emplace_back(this->timeFreezeLabel.get());
	this->elements.emplace_back(this->coordsLabel.get());
	this->elements.emplace_back(this->xTextBox.get());
	this->elements.emplace_back(this->yTextBox.get());
	this->elements.emplace_back(this->zTextBox.get());
}

void RenameMPLevelScreen::setupPositions() {
	this->closeScreenButton->posX = 4;
	this->closeScreenButton->posY = 4;
	this->field_5C->posX = 0;
	this->field_5C->posY = 0;
	this->field_5C->width = this->width;
	this->field_5C->height = this->closeScreenButton->height + 8;

	int startY = this->field_5C->height + 15;
	int leftX = 15;
	int rightX = this->width / 2 + 10;
	int columnWidth = this->width / 2 - 25;

	// Left Column
	this->worldNameLabel->posX = leftX;
	this->worldNameLabel->posY = startY;

	this->worldNameTextBox->posX = leftX;
	this->worldNameTextBox->posY = startY + 12;
	this->worldNameTextBox->width = columnWidth;
	this->worldNameTextBox->height = 20;

	this->gameModeLabel->posX = leftX;
	this->gameModeLabel->posY = startY + 42;

	int btnWidth = columnWidth / 2 - 5;
	if (btnWidth < 60) btnWidth = 60;

	this->survivalButton->width = btnWidth;
	this->survivalButton->height = 24;
	this->survivalButton->posX = leftX;
	this->survivalButton->posY = startY + 54;
	this->survivalButton->setYOffset(this->survivalButton->height / 2 - 4);

	this->creativeButton->width = btnWidth;
	this->creativeButton->height = 24;
	this->creativeButton->posX = leftX + btnWidth + 10;
	this->creativeButton->posY = startY + 54;
	this->creativeButton->setYOffset(this->creativeButton->height / 2 - 4);

	// Right Column
	if (this->timeSliderLabel) {
		this->timeSliderLabel->posX = rightX;
		this->timeSliderLabel->posY = startY;
	}

	if (this->timeSlider) {
		this->timeSlider->posX = rightX;
		this->timeSlider->posY = startY + 12;
		this->timeSlider->width = columnWidth;
		this->timeSlider->height = 20;
	}

	int freezeY = startY + 38;
	if (this->timeFreezeLabel) {
		this->timeFreezeLabel->posX = rightX;
		this->timeFreezeLabel->posY = freezeY;
	}
	int freezeBtnW = columnWidth / 2 - 3;
	if (this->timeFreezeOnButton) {
		this->timeFreezeOnButton->width = freezeBtnW;
		this->timeFreezeOnButton->height = 20;
		this->timeFreezeOnButton->posX = rightX;
		this->timeFreezeOnButton->posY = freezeY + 12;
		this->timeFreezeOnButton->setYOffset(this->timeFreezeOnButton->height / 2 - 4);
	}
	if (this->timeFreezeOffButton) {
		this->timeFreezeOffButton->width = freezeBtnW;
		this->timeFreezeOffButton->height = 20;
		this->timeFreezeOffButton->posX = rightX + freezeBtnW + 6;
		this->timeFreezeOffButton->posY = freezeY + 12;
		this->timeFreezeOffButton->setYOffset(this->timeFreezeOffButton->height / 2 - 4);
	}

	int coordY = freezeY + 40;
	if (this->coordsLabel) {
		this->coordsLabel->posX = rightX;
		this->coordsLabel->posY = coordY;
	}
	int coordBoxW = (columnWidth - 10) / 3;
	if (this->xTextBox) {
		this->xTextBox->posX = rightX;
		this->xTextBox->posY = coordY + 12;
		this->xTextBox->width = coordBoxW;
		this->xTextBox->height = 20;
	}
	if (this->yTextBox) {
		this->yTextBox->posX = rightX + coordBoxW + 5;
		this->yTextBox->posY = coordY + 12;
		this->yTextBox->width = coordBoxW;
		this->yTextBox->height = 20;
	}
	if (this->zTextBox) {
		this->zTextBox->posX = rightX + (coordBoxW + 5) * 2;
		this->zTextBox->posY = coordY + 12;
		this->zTextBox->width = coordBoxW;
		this->zTextBox->height = 20;
	}

	this->makeInfiniteButton->width = columnWidth;
	this->makeInfiniteButton->height = 24;
	this->makeInfiniteButton->posX = leftX;
	this->makeInfiniteButton->posY = startY + 84;
	if (this->originalGeneratorVersion == 1) {
		this->makeInfiniteButton->active = false;
	}

	this->renameButton->width = 38;
	this->renameButton->height = 18;
	this->renameButton->posX = this->width - 42;
	this->renameButton->posY = 4;

	this->field_A4->setSize((float)this->width - 10.0, (float)(this->height - this->field_5C->height - 10));
	this->btnBgUnpressed->setSize((float)btnWidth, (float)this->survivalButton->height);
	this->btnBgPressed->setSize((float)btnWidth, (float)this->survivalButton->height);
}

bool_t RenameMPLevelScreen::handleBackEvent(bool_t a2) {
	if(!a2) {
		bool_t v4 = 1;
		for(auto&& e: this->elements) {
			if(e->suppressOtherGUI()) {
				v4 = 0;
				e->backPressed(this->minecraft, 0);
			}
		}
		if(v4) {
			this->closeScreen();
		}
	}
	return 1;
}

static char_t _D6723994[] = {0x2F, 0xA, 0xD, 9, 0, 0xC, 0x60, 0x3F, 0x2A, 0x5C, 0x3C, 0x3E, 0x7C, 0x22, 0x3A, 0x2F};

void RenameMPLevelScreen::buttonClicked(Button* a2) {
	if(a2 == this->closeScreenButton.get()) {
		this->closeScreen();
	} else if(a2 == this->survivalButton || a2 == this->creativeButton) {
		this->selectedGameMode = a2;
	} else if(a2 == this->timeFreezeOnButton || a2 == this->timeFreezeOffButton) {
		this->selectedTimeFreeze = a2;
	} else if(a2 == this->renameButton.get()) {
		std::string dest = this->worldNameTextBox->text;
		if(dest.size()) {
			int32_t v8 = 0;
			do {
				std::string v12(1, _D6723994[v8]);
				++v8;
				dest = *Util::stringReplace(dest, v12, "", -1);
			} while(v8 != 15);
			if(dest.size() == 0) {
				dest = "saved_world";
			}
			LevelStorageSource* ls = this->minecraft->getLevelSource();
			ls->renameLevel(this->name, dest);
			ExternalFileLevelStorageSource* els = (ExternalFileLevelStorageSource*)ls;
			std::string path = els->getFullPath(this->name);
			LevelData data;
			if(ExternalFileLevelStorage::readLevelData(path, data)) {
				int32_t gameType = (this->selectedGameMode == this->creativeButton) ? 1 : 0;
				data.setGameType(gameType);
				if (this->timeSlider) {
					int32_t selectedTime = (int32_t)(this->timeSlider->progress * 24000.0f);
					bool frozen = (this->selectedTimeFreeze == this->timeFreezeOnButton);
					if (frozen) {
						data.setStopTime(selectedTime);
						data.setTime(selectedTime);
					} else {
						data.setStopTime(-1);
						int32_t currentTicks = data.getTime();
						int32_t days = currentTicks / 24000;
						data.setTime(days * 24000 + selectedTime);
					}
				}
				data.setGeneratorVersion(this->originalGeneratorVersion);

				if (this->playerTagBackup) {
					ListTag* posList = this->playerTagBackup->getList("Pos");
					if (posList && posList->value.size() >= 3) {
						if (this->xTextBox->text.size()) ((FloatTag*)posList->value[0])->value = atof(this->xTextBox->text.c_str());
						if (this->yTextBox->text.size()) ((FloatTag*)posList->value[1])->value = atof(this->yTextBox->text.c_str());
						if (this->zTextBox->text.size()) ((FloatTag*)posList->value[2])->value = atof(this->zTextBox->text.c_str());
					}
					data.setPlayerTag(this->playerTagBackup);
				}

				ExternalFileLevelStorage::saveLevelData(path, data, nullptr);
			}

			this->closeScreen();
		}
	} else if (a2 == this->makeInfiniteButton.get()) {
		this->originalGeneratorVersion = 1;
		this->makeInfiniteButton->active = false;
	}
}
