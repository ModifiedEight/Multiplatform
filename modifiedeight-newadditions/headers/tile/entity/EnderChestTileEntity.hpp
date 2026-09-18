#pragma once
#include <tile/entity/ChestTileEntity.hpp>

struct EnderChestTileEntity: ChestTileEntity {
	Player* viewingPlayer;

	EnderChestTileEntity();
	virtual ~EnderChestTileEntity();
	bool_t canPairWith(TileEntity*);
	virtual bool_t shouldSave() override;
	virtual void load(CompoundTag*) override;
	virtual bool_t save(CompoundTag*) override;
	virtual void tick() override;
	virtual std::string getName() override { return "Ender Chest"; }
	virtual ItemInstance* getItem(int32_t) override;
	virtual void setItem(int32_t, ItemInstance*) override;
	virtual void stopOpen() override;
	void openBy(Player*);
};
