#pragma once
#include <gui/Screen.hpp>
#include <string>

struct SaveWorldScreen : Screen
{
	int32_t step;
	bool_t copyMap;

	SaveWorldScreen(bool_t copyMap = 0);
	virtual ~SaveWorldScreen();

	virtual void render(int32_t, int32_t, float);
	virtual void init();
	virtual void setupPositions();
	virtual bool_t handleBackEvent(bool_t);
	virtual void tick();
	virtual bool_t isInGameScreen();
};
