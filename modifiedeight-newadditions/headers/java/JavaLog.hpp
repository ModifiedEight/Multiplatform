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
void javaLog(const char_t* fmt, ...);
void javaDebug(const char_t* fmt, ...);
bool_t javaDebugEnabled();
// M8_JAVA_DEBUG=2: also name every packet in both directions.
bool_t javaTraceEnabled();
