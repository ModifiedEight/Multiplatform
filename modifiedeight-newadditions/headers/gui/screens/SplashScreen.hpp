#pragma once
#include <gui/Screen.hpp>

struct SplashScreen : Screen {
	float timer;
	float duration;
	float fadeDuration;

	SplashScreen();
	virtual ~SplashScreen();
	virtual void render(int32_t mx, int32_t my, float delta);
	virtual void tick();
	virtual bool_t isPauseScreen();
	virtual bool_t renderGameBehind();
	virtual bool_t handleBackEvent(bool_t);
};
