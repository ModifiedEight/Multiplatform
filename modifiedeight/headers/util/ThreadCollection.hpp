#pragma once
#include <_types.h>
#include <deque>
#include <vector>
#include <mutex>
#include <thread>
#include <memory>
#include <condition_variable>

struct Job;
struct ThreadCollection
{
	std::vector<std::thread> threads;
	std::deque<std::shared_ptr<Job>> field_C;
	std::deque<std::shared_ptr<Job>> field_34; //maybe endedJobs
	std::mutex mutex;
	std::mutex field_60; //maybe endedJobsMutex?
	std::condition_variable field_64;
	bool isStopped;

	ThreadCollection(uint32_t maxthreads);
	void enqueue(std::shared_ptr<Job>);
	void processUIThread();
	~ThreadCollection();
};
