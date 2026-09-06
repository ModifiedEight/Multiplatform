#pragma once
#include <_types.h>
#include <input/ControllerLayout.hpp>

struct Minecraft;
struct IMoveInput;
struct Player;

struct ControllerHandler {
	static bool_t captureMode;
	static bool_t captureWantsAxis;
	static bool_t hasCaptured;
	static Binding capturedBinding;
	static float cursorX, cursorY;
	static int32_t scrollRepeat;
	static int32_t lastPadCursorMs;

	static void hideCursor();
	static bool_t cursorVisible();
	static float moveX, moveY;
	static float frameDt;
	static double lastFrameS;
	static void clampVec(float&, float&);
	static int32_t lastPerspectiveMs;
	static bool_t jumpHeld, sneakHeld, sprintHeld;
	static bool_t attackHeld, useHeld, confirmHeld;

	static void beginCapture(bool_t);
	static void endCapture();
	static bool_t takeCaptured(Binding&);

	static void tick(Minecraft*);
	static void tickGame(Minecraft*);
	static void tickMenu(Minecraft*);
	static void applyMove(IMoveInput*, Player*);
	static void releaseAll();
	static bool_t isEnabled(Minecraft*);
};
