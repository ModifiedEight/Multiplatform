#pragma once
#if !defined(ANDROID) && !defined(MCPE_IOS) && !defined(__APPLE__)
#include <AppPlatform.hpp>
#if defined(__has_include)
#if __has_include(<SDL/SDL.h>)
#include <SDL/SDL.h>
#elif __has_include(<SDL.h>)
#include <SDL.h>
#else
#include <SDL/SDL.h>
#endif
#else
#include <SDL/SDL.h>
#endif

struct AppPlatform_sdl : AppPlatform{

	int32_t screenWidth, screenHeight;
	bool_t hasContext;
	SDL_Surface* sdl_surface;
	SDL_Event sdl_event;

	virtual std::string getImagePath(const std::string&, bool_t);
	virtual void loadPNG(ImageData&, const std::string&, bool_t);
	virtual AssetFile readAssetFile(const std::string& path);
	virtual bool supportsTouchscreen();
	virtual LoginInformation getLoginInformation(void);
	virtual void showKeyboard(std::string*, int32_t, bool_t) override;
	virtual void hideKeyboard(void) override;

	AppPlatform_sdl();
	~AppPlatform_sdl();
	SDL_Surface* setSDLVideoMode();
	bool_t sdlCtxInit();
	void sdlCtxDestroy();
	void init();
	void onKeyPressed(struct Minecraft*, SDLKey key, uint8_t scancode, bool pressed);
};
#endif
