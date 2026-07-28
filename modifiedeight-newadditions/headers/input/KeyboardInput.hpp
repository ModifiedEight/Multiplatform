#pragma once
#include <input/IMoveInput.hpp>

struct Options;
struct Player;

struct KeyboardInput: IMoveInput
{
	int8_t inputs[10];
	Options* options;
	int forwardTapTimer; // ticks remaining to detect double-tap
	bool forwardDoubleTapDetected;

	KeyboardInput(Options*);

	virtual ~KeyboardInput();
	virtual void tick(Player*);
	virtual void setKey(int32_t, bool_t);
	virtual void releaseAllKeys();
};
