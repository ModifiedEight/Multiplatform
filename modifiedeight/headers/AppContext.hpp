#pragma once
#include <_types.h>
#if defined(USEGLES) && !defined(MCPE_IOS) && !defined(__APPLE__)
#include <unigl.h>
#endif
struct AppContext{
#if defined(USEGLES) && !defined(MCPE_IOS) && !defined(__APPLE__)
	EGLDisplay  field_0;
	EGLContext field_4;
	EGLSurface field_8;
#else
	void* field_0;
	void* field_4;
	void* field_8;
#endif
	struct AppPlatform* platform;

	int8_t field_10, field_11, field_12, field_13;
};
