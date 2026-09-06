#pragma once
#include <_types.h>
#include <gui/Screen.hpp>
#include <input/ControllerLayout.hpp>

namespace Touch {
	struct TButton;
}
struct NinePatchLayer;

struct ControllerLayoutScreen: Screen
{
	Touch::TButton* buttonSave;
	Touch::TButton* buttonReset;
	Touch::TButton* buttonCancel;
	NinePatchLayer* optBtn;
	NinePatchLayer* playPressed;
	NinePatchLayer* listBg;
	NinePatchLayer* panelBg;

	Binding working[CA_COUNT];
	int32_t capturingAction;
	float scrollOffset;
	float scrollTarget;
	int32_t contentHeight;
	int32_t listTop, listBottom;
	int32_t actionsColumnWidth, bindingsColumnWidth;
	int32_t contentLeft, contentRight, scrollBarX;
	int32_t dragStartY;
	float dragStartOffset;
	float dragVel;
	int32_t lastDragMs;
	bool_t isDragging;
	bool_t draggingBar;
	int32_t barGrabDY;
	int32_t lastAnimMs;
	bool_t isInWorld;

	ControllerLayoutScreen(bool_t);

	void closeScreen();
	void clampScroll();
	void calcBar(int32_t&, int32_t&);
	int32_t rowIndexAt(int32_t, int32_t);

	virtual ~ControllerLayoutScreen();
	virtual void init();
	virtual void setupPositions();
	virtual void render(int32_t, int32_t, float);
	virtual void tick();
	virtual bool_t handleBackEvent(bool_t);
	virtual bool_t renderGameBehind();
	virtual void buttonClicked(Button*);
	virtual void mouseClicked(int32_t, int32_t, int32_t);
	virtual void mouseReleased(int32_t, int32_t, int32_t);
	virtual void keyPressed(int32_t);
};
