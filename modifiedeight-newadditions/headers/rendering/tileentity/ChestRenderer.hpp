#pragma once
#include <_types.h>
#include <rendering/tileentity/TileEntityRenderer.hpp>
#include <rendering/model/ChestModel.hpp>

struct Textures;

struct ChestRenderer: TileEntityRenderer
{
	ChestModel field_8, field_1A8;

	ChestRenderer();

	virtual ~ChestRenderer();
	virtual void render(struct TileEntity*, float, float, float, float, bool_t);
	static void renderGuiChest(Textures* textures, bool isEnder, float x, float y, float scale, float alpha = 1.0f);
};
