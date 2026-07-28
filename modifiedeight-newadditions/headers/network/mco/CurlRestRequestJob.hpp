#pragma once
#if defined(__linux__) && !defined(ANDROID)
#include <network/mco/RestRequestJob.hpp>
#include <android/AppPlatform_android.hpp>
#include <mutex>
#include <condition_variable>

struct CurlRestRequestJob: RestRequestJob
{
	int field_54, field_58, field_5C;
	std::condition_variable field_60;
	std::mutex field_64;
	std::mutex mutex;
	int field_6C;
	int httpStatusOrNegativeError;
	std::string content;
	bool started;
	char _align[3];

	CurlRestRequestJob();
	bool isRunning();
	void onRequestComplete(int, int, const std::string&);

	virtual ~CurlRestRequestJob();
	virtual void stop();
	virtual void run();
	virtual void finish();
};
#endif
