#pragma once
#ifdef MCPE_IOS
#include <AppPlatform.hpp>
#include <string>
#include <vector>

// iOS platform implementation. Mirrors the responsibilities of
// AppPlatform_sdl / AppPlatform_android but for UIKit + EAGL. The actual
// GL surface and the run loop are owned by the Objective-C(++) shell
// (see mcpe_ios_main.mm / EAGLView.mm); this class only fulfils the
// AppPlatform pure-virtual contract the core game expects.
struct AppPlatform_iOS : public AppPlatform {
	int32_t screenWidth;
	int32_t screenHeight;
	float pixelsPerMM;

	AppPlatform_iOS();
	virtual ~AppPlatform_iOS();

	// Asset / image access (bundle-relative).
	virtual std::string getImagePath(const std::string&, bool_t);
	virtual void loadPNG(ImageData&, const std::string&, bool_t);
	virtual AssetFile readAssetFile(const std::string& path);

	// Screen metrics.
	virtual int32_t getScreenWidth(void);
	virtual int32_t getScreenHeight(void);
	virtual float getPixelsPerMillimeter(void);

	// Capabilities.
	virtual bool supportsTouchscreen(void);
	virtual LoginInformation getLoginInformation(void);

	// On-screen keyboard (chat / sign text). Driven by the shell's text view.
	virtual void showKeyboard(std::string*, int32_t, bool_t);
	virtual void hideKeyboard(void);
	virtual void updateTextBoxText(const std::string&);

	void setScreenSize(int32_t w, int32_t h, float scale);

	// Absolute path to the .app bundle resources (set by the shell at launch).
	static std::string bundlePath;
};
#endif
