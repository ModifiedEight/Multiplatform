#pragma once
#include <tile/entity/ChestTileEntity.hpp>
#include <vector>

struct EnderChestTileEntity: ChestTileEntity {
	static std::vector<ItemInstance*> s_enderInventory;

	EnderChestTileEntity();
	virtual ~EnderChestTileEntity();
	bool_t canPairWith(TileEntity*);
	virtual bool_t shouldSave() override;
	virtual void load(CompoundTag*) override;
	virtual bool_t save(CompoundTag*) override;
	virtual void tick() override;
	virtual std::string getName() override { return "Ender Chest"; }
	void openBy(Player*);
};
