#ifdef MCPE_IOS
#include "AppPlatform_iOS.hpp"
#include <_AssetFile.hpp>
#include <ImageData.hpp>
#include <network/mco/LoginInformation.hpp>
#include <stb_image.h>

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

std::string AppPlatform_iOS::bundlePath;

// Keyboard bridge implemented in main.mm (talks to the view controller).
extern void MCPE_iOS_ShowKeyboard(const std::string& current);
extern void MCPE_iOS_HideKeyboard(void);
extern void MCPE_iOS_UpdateTextBoxText(const std::string& current);

AppPlatform_iOS::AppPlatform_iOS()
	: screenWidth(0), screenHeight(0), pixelsPerMM(10.0f) {
	if (bundlePath.empty()) {
		@autoreleasepool {
			NSString* res = [[NSBundle mainBundle] resourcePath];
			if (res) bundlePath = std::string([res UTF8String]) + "/";
		}
	}
}

AppPlatform_iOS::~AppPlatform_iOS() {}

void AppPlatform_iOS::setScreenSize(int32_t w, int32_t h, float scale) {
	this->screenWidth = w;
	this->screenHeight = h;
	// ~163 dpi baseline (non-retina), scaled by the backing factor.
	this->pixelsPerMM = (163.0f / 25.4f) * (scale > 0 ? scale : 1.0f);
}

// assets/ is copied into the .app bundle (see build-ipa.sh). Resolve every
// asset path against the bundle's resource directory.
std::string AppPlatform_iOS::getImagePath(const std::string& name, bool_t) {
	return bundlePath + "assets/images/" + name;
}

AssetFile AppPlatform_iOS::readAssetFile(const std::string& path) {
	AssetFile packFile = AppPlatform::readAssetFile(path);
	if (packFile.bytes && packFile.length > 0) {
		return packFile;
	}
	return AppPlatform::readAssetFile(bundlePath + "assets/" + path);
}

void AppPlatform_iOS::loadPNG(ImageData& data, const std::string& path, bool_t) {
	int32_t channels;
	AssetFile file = this->readAssetFile(path);
	if (file.bytes && file.length > 0) {
		uint8_t* pixels = stbi_load_from_memory(file.bytes, file.length, &data.width, &data.height, &channels, STBI_rgb_alpha);
		delete[] file.bytes;
		if (pixels) {
			data.field_C = 0;
			data.pixels = pixels;
			return;
		}
	}
	uint8_t* pixels = stbi_load(path.c_str(), &data.width, &data.height,
	                            &channels, STBI_rgb_alpha);
	if (!pixels) {
		NSLog(@"[MCPE] failed to load %s", path.c_str());
		return;
	}
	data.field_C = 0;
	data.pixels = pixels;
}

int32_t AppPlatform_iOS::getScreenWidth(void) { return this->screenWidth; }
int32_t AppPlatform_iOS::getScreenHeight(void) { return this->screenHeight; }
float AppPlatform_iOS::getPixelsPerMillimeter(void) { return this->pixelsPerMM; }

bool AppPlatform_iOS::supportsTouchscreen(void) { return true; }

LoginInformation AppPlatform_iOS::getLoginInformation(void) {
	return LoginInformation();
}

void AppPlatform_iOS::showKeyboard(std::string* text, int32_t, bool_t) {
	this->keyboardShown = 1;
	MCPE_iOS_ShowKeyboard(text ? *text : std::string());
}

void AppPlatform_iOS::hideKeyboard(void) {
	this->keyboardShown = 0;
	MCPE_iOS_HideKeyboard();
}

void AppPlatform_iOS::updateTextBoxText(const std::string& text) {
	MCPE_iOS_UpdateTextBoxText(text);
}
#endif
