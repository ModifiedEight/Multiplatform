#pragma once
#include <_types.h>
#if defined(ANDROID)
#include <android/AppPlatform_android.hpp>
extern bool contextWasLost;
extern jobject mainActivity_ref;
extern struct NinecraftApp* ninecraftApp;
extern AppPlatform_android appPlatform;
extern pthread_mutex_t _D6E04480;
#elif defined(MCPE_IOS) || defined(__APPLE__)
#include <AppPlatform_iOS.hpp>
extern AppPlatform_iOS appPlatform;
#else
#include <AppPlatform_sdl.hpp>
extern AppPlatform_sdl appPlatform;
#endif
