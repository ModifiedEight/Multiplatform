#pragma once
#include <_types.h>
#include <unigl.h>

struct EnableState{
	GLenum enabled;

	EnableState(GLenum s);
	~EnableState();
};
