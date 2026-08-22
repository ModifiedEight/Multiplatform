#pragma once
#include <_types.h>

/*
 * Diagnostics for the Java Edition session.
 *
 * javaLog is for the handful of milestones (and failures) a player or a bug
 * report needs to see; it always prints.  javaDebug is per packet noise and only
 * prints when M8_JAVA_DEBUG is set to something other than 0 in the
 * environment, so a normal session stays quiet.
 */
/*
 * Let the compiler check the varargs.  Android builds five ABIs, three of them
 * 32 bit, where handing a 64 bit or size_t value to %d prints rubbish.
 */
#if defined(__GNUC__) || defined(__clang__)
#define JAVA_LOG_FORMAT __attribute__((format(printf, 1, 2)))
#else
#define JAVA_LOG_FORMAT
#endif

void javaLog(const char_t* fmt, ...) JAVA_LOG_FORMAT;
void javaDebug(const char_t* fmt, ...) JAVA_LOG_FORMAT;
bool_t javaDebugEnabled();
// M8_JAVA_DEBUG=2: also name every packet in both directions.
bool_t javaTraceEnabled();
