#pragma once
#include <gui/GuiElementContainer.hpp>
#include <memory>

struct Button;
struct ControllerLayoutOption: GuiElementContainer
{
	std::shared_ptr<Button> customizeButton;
	Button* pressedButton;
	bool_t isInWorld;

	ControllerLayoutOption(Minecraft*, bool_t);

	virtual ~ControllerLayoutOption();
	virtual void render(Minecraft*, int32_t, int32_t);
	virtual void setupPositions();
	virtual void mouseClicked(Minecraft*, int32_t, int32_t, int32_t);
	virtual void mouseReleased(Minecraft*, int32_t, int32_t, int32_t);
};
