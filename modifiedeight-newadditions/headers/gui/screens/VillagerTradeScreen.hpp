#pragma once
#include <gui/Screen.hpp>
#include <entity/Villager.hpp>

struct Player;

struct VillagerTradeScreen : Screen {
	Player* player;
	Villager* villager;
	int32_t selectedTrade;
	int32_t scrollOffset;
	int32_t guiX, guiY;
	ItemInstance placedInputItem;
	int32_t lastSourceSlot;

	VillagerTradeScreen(Player* player, Villager* villager);

	virtual ~VillagerTradeScreen();
	virtual void init();
	virtual void render(int32_t, int32_t, float);
	virtual void tick();
	virtual void removed();
	virtual bool_t renderGameBehind();
	virtual bool_t isInGameScreen();
	virtual void buttonClicked(Button*);
	virtual void mouseClicked(int32_t, int32_t, int32_t);
	virtual void keyPressed(int32_t);
};
