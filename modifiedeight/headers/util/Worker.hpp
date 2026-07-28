#pragma once
#include <_types.h>

struct ThreadCollection;
struct Worker
{
	ThreadCollection* threadCollection;

	Worker(ThreadCollection&);
	void operator()(void);
};
