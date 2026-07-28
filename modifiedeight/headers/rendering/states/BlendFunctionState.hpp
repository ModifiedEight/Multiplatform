#pragma once
#include <_types.h>
#include <unigl.h>

struct BlendFunctionState{
	BlendFunctionState(GLenum sfactor, GLenum dfactor);
	~BlendFunctionState();
};
