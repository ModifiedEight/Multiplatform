#pragma once
#include <gui/Screen.hpp>
#include <cstdint>
#include <string>

struct Player;

struct MusicPlayerScreen : Screen {
	int32_t blockX, blockY, blockZ;
	int32_t scrollOffset;
	int32_t dialogX, dialogY, dialogW, dialogH;

	MusicPlayerScreen(int32_t x, int32_t y, int32_t z);
	virtual ~MusicPlayerScreen();
	virtual void init();
	virtual void render(int32_t mx, int32_t my, float a4);
	virtual void tick();
	virtual bool_t renderGameBehind();
	virtual bool_t isInGameScreen();
	virtual void buttonClicked(Button*);
	virtual void mouseClicked(int32_t mx, int32_t my, int32_t btn);
	virtual void keyPressed(int32_t key);
};
