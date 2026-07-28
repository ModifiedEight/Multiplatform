#pragma once
#if defined(MCPE_IOS) || defined(__APPLE__)
#include_next <_types.h>
#endif
#define UNK
#include <stdint.h>
typedef char char_t;
typedef	unsigned char bool_t;
#ifdef DEBUG

#ifdef ANDROID
#include <android/log.h>
inline void _android_debug_thingy(const char* fmt, ...){
	va_list args;
	va_start(args, fmt);
	__android_log_vprint(ANDROID_LOG_DEBUG, "MCPE081DECOMP", fmt, args);
	va_end(args);
}
#define DEBUGMSG _android_debug_thingy
#else
#include <stdio.h>
#define DEBUGMSG printf
#endif

#else
#define DEBUGMSG(...)
#endif
