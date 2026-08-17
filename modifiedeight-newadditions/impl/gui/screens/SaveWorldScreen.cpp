#include <gui/screens/SaveWorldScreen.hpp>
#include <Minecraft.hpp>
#include <level/Level.hpp>
#include <level/gen/ChunkSource.hpp>
#include <level/storage/LevelData.hpp>
#include <level/storage/LevelStorage.hpp>
#include <nbt/CompoundTag.hpp>
#include <entity/LocalPlayer.hpp>
#include <rendering/Font.hpp>
#include <rendering/Tesselator.hpp>
#include <rendering/Textures.hpp>
#include <utils.h>

SaveWorldScreen::SaveWorldScreen(bool_t copyMap) {
	this->step = 0;
	this->copyMap = copyMap;
}

SaveWorldScreen::~SaveWorldScreen() {
}

void SaveWorldScreen::init() {
}

void SaveWorldScreen::setupPositions() {
}

bool_t SaveWorldScreen::handleBackEvent(bool_t a2) {
	return 1;
}

bool_t SaveWorldScreen::isInGameScreen() {
	return 0;
}

void SaveWorldScreen::render(int32_t a2, int32_t a3, float a4) {
	this->renderBackground(0);
	this->minecraft->texturesPtr->loadAndBindTexture("gui/background.png");
	Tesselator::instance.begin(4);
	Tesselator::instance.color(4210752);
	Tesselator::instance.vertexUV(0.0, (float)this->height, 0.0, 0.0, (float)this->height * 0.03125);
	Tesselator::instance.vertexUV((float)this->width, (float)this->height, 0.0, (float)this->width * 0.03125, (float)this->height * 0.03125);
	Tesselator::instance.vertexUV((float)this->width, 0.0, 0.0, (float)this->width * 0.03125, 0.0);
	Tesselator::instance.vertexUV(0.0, 0.0, 0.0, 0.0, 0.0);
	Tesselator::instance.draw(1);

	int32_t centerX = this->width / 2;
	int32_t centerY = this->height / 2;

	Font* font = this->minecraft->font;
	std::string title = "Saving world...";
	int32_t tw = font->width(title);
	font->drawShadow(title, (float)(centerX - tw / 2), (float)(centerY - 4), 0xFFFFFF);

	Screen::render(a2, a3, a4);
}

void SaveWorldScreen::tick() {
	if (this->step == 0) {
		this->step = 1;
	} else if (this->step == 1) {
		if (this->minecraft->player && this->minecraft->level && this->minecraft->level->getLevelData()) {
			CompoundTag playerTag;
			this->minecraft->player->saveWithoutId(&playerTag);
			this->minecraft->level->getLevelData()->setPlayerTag(&playerTag);
		}
		if (this->minecraft->level && this->minecraft->level->getLevelStorage() && this->minecraft->player) {
			this->minecraft->level->getLevelStorage()->save(this->minecraft->player);
		}
		this->step = 2;
	} else if (this->step == 2) {
		if (this->minecraft->level && this->minecraft->level->getChunkSource()) {
			this->minecraft->level->getChunkSource()->saveAll(0);
		}
		this->step = 3;
	} else if (this->step == 3) {
		if (this->minecraft->level) {
			this->minecraft->level->saveGame();
			this->minecraft->level->savePlayers();
			this->minecraft->level->saveLevelData();
		}
		this->step = 4;
	} else if (this->step == 4) {
		this->minecraft->leaveGame(this->copyMap, 1);
	}
}
