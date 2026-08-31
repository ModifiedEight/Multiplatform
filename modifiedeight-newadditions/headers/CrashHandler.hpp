#pragma once
#include <_types.h>
#include <string>

struct CrashHandler {
	static void install();
	static std::string getLastCrashReport();
	static void triggerReport(const std::string& customError);
};
