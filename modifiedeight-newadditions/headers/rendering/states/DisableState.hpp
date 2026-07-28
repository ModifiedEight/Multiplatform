#pragma once
#include <_types.h>
#include <unigl.h>

struct DisableState{
	GLenum disabled;

	DisableState(GLenum s);
	~DisableState();
};
