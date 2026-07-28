#pragma once
#include <_types.h>
#include <set>
#include <deque>
#include <unigl.h>

extern struct GLBufferPool glBufferPool;

struct GLBufferPool{
	std::set<uint32_t> usedBuffers;
	std::deque<uint32_t> unusedBuffers;
	int reserveCnt;

	GLBufferPool(unsigned int reserveCnt); //inlined by compiler, type: unsigned int according to x86 version
	GLuint get(void);
	void release(uint32_t);
	bool_t trim(void);
	~GLBufferPool();
};
