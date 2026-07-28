#pragma once
#include <_types.h>
#include <unigl.h>

struct EnableClientState{
	GLenum enabled;

	EnableClientState();
	EnableClientState(GLenum s);
	~EnableClientState();
};
