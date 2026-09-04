#include <rendering/EntityTileRenderer.hpp>
#include <tile/entity/ChestTileEntity.hpp>
#include <rendering/TileEntityRenderDispatcher.hpp>
#include <tile/Tile.hpp>

EntityTileRenderer* EntityTileRenderer::instance = new ChestTileEntity();
void EntityTileRenderer::render(Tile* a2, int32_t a3, float a4) {
	if (a2 && Tile::enderChest && a2 == Tile::enderChest) {
		((ChestTileEntity*)this)->type = 6;
	} else {
		((ChestTileEntity*)this)->type = 1;
	}
	TileEntityRenderDispatcher::getInstance()->render((TileEntity*) this, 0, 0, 0, 0);
}
