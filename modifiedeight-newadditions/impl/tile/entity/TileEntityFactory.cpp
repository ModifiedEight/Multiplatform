#include <tile/entity/TileEntityFactory.hpp>
#include <tile/entity/FurnaceTileEntity.hpp>
#include <tile/entity/ChestTileEntity.hpp>
#include <tile/entity/NetherReactorTileEntity.hpp>
#include <tile/entity/SignTileEntity.hpp>
#include <tile/entity/MixedSlabTileEntity.hpp>
#include <tile/entity/EnderChestTileEntity.hpp>

TileEntity* TileEntityFactory::createTileEntity(int32_t type) {
	switch(type) {
		case 1:
			return new FurnaceTileEntity();
		case 2:
			return new ChestTileEntity();
		case 3:
			return new NetherReactorTileEntity();
		case 4:
			return new SignTileEntity();
		case 5:
			return new MixedSlabTileEntity();
		case 6:
			return new EnderChestTileEntity();
		default:
			return 0;
	}
}
