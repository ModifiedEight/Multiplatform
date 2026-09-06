#include <tile/entity/EnderChestTileEntity.hpp>
#include <entity/Player.hpp>
#include <level/Level.hpp>
#include <item/ItemInstance.hpp>
#include <nbt/CompoundTag.hpp>

EnderChestTileEntity::EnderChestTileEntity()
	: ChestTileEntity() {
	this->type = 6;
	this->items.resize(27);
	for (int i = 0; i < 27; ++i) {
		this->items[i] = nullptr;
	}
}

EnderChestTileEntity::~EnderChestTileEntity() {
	for (int i = 0; i < 27; ++i) {
		if (this->items[i]) {
			delete this->items[i];
			this->items[i] = nullptr;
		}
	}
}

bool_t EnderChestTileEntity::canPairWith(TileEntity*) {
	return 0;
}

bool_t EnderChestTileEntity::shouldSave() {
	return 0;
}

void EnderChestTileEntity::load(CompoundTag* tag) {
	TileEntity::load(tag);
}

bool_t EnderChestTileEntity::save(CompoundTag* tag) {
	return TileEntity::save(tag);
}

void EnderChestTileEntity::tick() {
	if (this->openedBy) {
		Player* p = (Player*)this->openedBy;
		for (int i = 0; i < 27; ++i) {
			p->enderChestItems[i] = (i < (int)this->items.size()) ? this->items[i] : nullptr;
		}
	}

	int32_t v3 = this->field_90 + 1;
	this->field_90 = v3;
	if (v3 > 79) {
		if (this->level) {
			this->level->tileEvent(this->posX, this->posY, this->posZ, 1, this->field_8C);
		}
		this->field_90 = 0;
	}

	if (this->openedBy) {
		int32_t v4 = this->field_A8 - 1;
		this->field_A8 = v4;
		if (!v4) {
			this->openedBy->openContainer(this);
			this->openedBy = 0;
			--this->field_8C;
		}
	}

	int32_t v5 = this->field_8C;
	float v6 = this->field_84;
	this->field_88 = v6;
	if (v5 > 0 && v6 == 0.0f) {
		if (this->level) {
			this->level->playSound((float)this->posX + 0.5f, (float)this->posY + 0.5f, (float)this->posZ + 0.5f, "random.enderchestopen", 0.5f, (float)(this->level->random.nextFloat() * 0.1f) + 0.9f);
		}
	}

	int32_t v12 = this->field_8C;
	if (!v12) {
		if (this->field_84 <= 0.0f) {
			return;
		}
	}
	if (v12 > 0 || this->field_84 > 0.0f) {
		float v13 = this->field_84;
		float v14 = (v12 <= 0) ? (v13 - 0.1f) : (v13 + 0.1f);
		this->field_84 = v14;
		if (this->field_84 > 1.0f) {
			this->field_84 = 1.0f;
		}
		if (this->field_84 < 0.5f && v13 >= 0.5f) {
			if (this->level) {
				this->level->playSound((float)this->posX + 0.5f, (float)this->posY + 0.5f, (float)this->posZ + 0.5f, "random.enderchestclosed", 0.5f, (float)(this->level->random.nextFloat() * 0.1f) + 0.9f);
			}
		}
		if (this->field_84 < 0.0f) {
			this->field_84 = 0.0f;
		}
	}
}

void EnderChestTileEntity::openBy(Player* player) {
	if (this->openedBy && this->openedBy != player) {
		Player* prev = (Player*)this->openedBy;
		for (int i = 0; i < 27; ++i) {
			prev->enderChestItems[i] = (i < (int)this->items.size()) ? this->items[i] : nullptr;
		}
	}
	this->openedBy = player;
	if (player) {
		for (int i = 0; i < 27; ++i) {
			if (i < (int)this->items.size()) {
				this->items[i] = player->enderChestItems[i];
			}
		}
	}
	if (this->field_A8 <= 0) {
		this->field_A8 = 6;
		this->startOpen();
	}
	if (player) {
		player->openContainer(this);
	}
}
