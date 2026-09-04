#include <tile/entity/EnderChestTileEntity.hpp>
#include <entity/Player.hpp>
#include <level/Level.hpp>
#include <nbt/CompoundTag.hpp>
#include <nbt/ListTag.hpp>
#include <tile/Tile.hpp>

std::vector<ItemInstance*> EnderChestTileEntity::s_enderInventory(27, nullptr);

EnderChestTileEntity::EnderChestTileEntity()
	: ChestTileEntity() {
	this->type = 6;
	this->isUnpaired = 1;
	this->pair = nullptr;
	for (int i = 0; i < 27; ++i) {
		this->items[i] = s_enderInventory[i];
	}
}

EnderChestTileEntity::~EnderChestTileEntity() {
	for (int i = 0; i < 27; ++i) {
		s_enderInventory[i] = this->items[i];
		this->items[i] = nullptr;
	}
}

bool_t EnderChestTileEntity::canPairWith(TileEntity*) {
	return 0;
}

bool_t EnderChestTileEntity::shouldSave() {
	return 1;
}

void EnderChestTileEntity::load(CompoundTag* tag) {
	TileEntity::load(tag);
	if (tag->contains("EnderItems")) {
		ListTag* list = tag->getList("EnderItems");
		for (size_t i = 0; i < list->value.size(); ++i) {
			Tag* tg = (Tag*)list->value[i];
			if (tg->getId() == 10) {
				CompoundTag* itemTag = (CompoundTag*)tg;
				int32_t slot = (uint8_t)itemTag->getByte("Slot");
				if (slot < 27) {
					if (!s_enderInventory[slot]) {
						s_enderInventory[slot] = new ItemInstance();
					}
					s_enderInventory[slot]->load(itemTag);
					this->items[slot] = s_enderInventory[slot];
				}
			}
		}
	}
}

bool_t EnderChestTileEntity::save(CompoundTag* tag) {
	if (TileEntity::save(tag)) {
		for (int i = 0; i < 27; ++i) {
			s_enderInventory[i] = this->items[i];
		}
		ListTag* list = new ListTag();
		for (int32_t i = 0; i < 27; ++i) {
			ItemInstance* it = s_enderInventory[i];
			if (it && !it->isNull()) {
				CompoundTag* itemTag = new CompoundTag();
				itemTag->putByte("Slot", (int8_t)i);
				it->save(itemTag);
				list->add(itemTag);
			}
		}
		tag->put("EnderItems", list);
		return 1;
	}
	return 0;
}

void EnderChestTileEntity::tick() {
	for (int i = 0; i < 27; ++i) {
		s_enderInventory[i] = this->items[i];
	}
	ChestTileEntity::tick();
}

void EnderChestTileEntity::openBy(Player* player) {
	for (int i = 0; i < 27; ++i) {
		this->items[i] = s_enderInventory[i];
	}
	if (!this->openedBy) {
		this->field_A8 = 6;
		this->openedBy = player;
		this->startOpen();
	}
	if (player) {
		player->openContainer(this);
	}
}
