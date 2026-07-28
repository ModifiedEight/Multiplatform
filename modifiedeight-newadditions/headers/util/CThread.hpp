#pragma once
#include <_types.h>
#include <pthread.h>

struct CThread
{
#ifdef EVILOREOBROKEPTHREADS
	pthread_mutex_t lock;
#endif
	pthread_t field_8;
	void* (*function)(void*);
	pthread_attr_t field_C;

	CThread(void* (*)(void*), void*);
	int32_t join();
	static int32_t sleep(uint32_t);

	virtual ~CThread();
};
