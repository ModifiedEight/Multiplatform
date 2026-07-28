#pragma once
#include <_types.h>
#include <AppContext.hpp>

struct ENGINE{
#ifdef ANDROID
	struct android_app* state;
#else
	void* state;
#endif

	int32_t field_4;
#if defined(USEGLES) && !defined(MCPE_IOS) && !defined(__APPLE__)
	EGLDisplay field_8;
	EGLSurface surface;
	EGLContext field_10;
#else
	void* field_8;
	void* surface;
	void* field_10;
#endif
	int32_t width, height;
	int32_t field_1C;
	struct Minecraft* minecraft;
	int8_t field_24;
	AppContext appCtx;
};
#ifdef ANDROID
void engine_handle_cmd(android_app*, int);
void engine_term_display(ENGINE*);
int engine_handle_input(struct android_app* app, struct AInputEvent* event); //XXX no symbol name
#endif
