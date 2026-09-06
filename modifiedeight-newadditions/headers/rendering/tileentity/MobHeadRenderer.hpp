#pragma once
#include <rendering/tileentity/TileEntityRenderer.hpp>
#include <rendering/model/ModelPart.hpp>

struct MobHeadRenderer: TileEntityRenderer {
	static MobHeadRenderer* instance;
	ModelPart headParts[18];

	MobHeadRenderer();
	virtual ~MobHeadRenderer();

	virtual void render(TileEntity*, float, float, float, float, bool_t);
	void renderHead(int32_t headType, float x, float y, float z, float rotAngle, float scale = 0.0625f);
	static const char* getTexturePath(int32_t headType);
};
