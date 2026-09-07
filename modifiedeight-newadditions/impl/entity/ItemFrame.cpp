#include <entity/ItemFrame.hpp>
#include <level/Level.hpp>
#include <entity/Player.hpp>
#include <item/Item.hpp>
#include <inventory/Inventory.hpp>
#include <nbt/CompoundTag.hpp>
#include <tile/Tile.hpp>
#include <tile/SlabTile.hpp>
#include <tile/material/Material.hpp>

ItemFrame::ItemFrame(Level* level)
	: HangingEntity(level)
	, rotation(0)
	, dropChance(1.0f) {
	this->entityRenderId = (EntityRendererId)36;
	this->synchedEntityData.define(8, (int32_t)0);
	this->synchedEntityData.define(9, (int32_t)0);
	this->synchedEntityData.define(10, (int8_t)0);
}

ItemFrame::ItemFrame(Level* level, int32_t x, int32_t y, int32_t z, int32_t dir)
	: HangingEntity(level, x, y, z, dir)
	, rotation(0)
	, dropChance(1.0f) {
	this->entityRenderId = (EntityRendererId)36;
	this->setDir(dir);
	this->synchedEntityData.define(8, (int32_t)0);
	this->synchedEntityData.define(9, (int32_t)0);
	this->synchedEntityData.define(10, (int8_t)0);
}

ItemFrame::~ItemFrame() {
}

bool_t ItemFrame::isPickable() {
	return 1;
}

int32_t ItemFrame::getEntityTypeId() const {
	return 71;
}

int32_t ItemFrame::getWidth() {
	return 12;
}

int32_t ItemFrame::getHeight() {
	return 12;
}

void ItemFrame::setItem(const ItemInstance& newItem) {
	this->item = newItem;
	this->item.count = 1;
	this->rotation = 0;
	this->synchedEntityData.set(8, (int32_t)this->item.getId());
	this->synchedEntityData.set(9, (int32_t)this->item.getAuxValue());
	this->synchedEntityData.set(10, (int8_t)0);
	if (this->level) {
		this->level->playSound(this, "random.pop", 0.5f, 1.0f);
	}
}

void ItemFrame::removeFramedItem() {
	ItemInstance disp = this->getDisplayedItem();
	if (!disp.isNull() && disp.count > 0) {
		if (this->dropChance > 0.0f) {
			this->spawnAtLocation(disp, 0.0f);
		}
		this->item = ItemInstance();
		this->synchedEntityData.set(8, (int32_t)0);
		this->synchedEntityData.set(9, (int32_t)0);
		this->synchedEntityData.set(10, (int8_t)0);
		if (this->level) {
			this->level->playSound(this, "random.pop", 0.5f, 1.2f);
		}
	}
}

void ItemFrame::dropItem() {
	this->removeFramedItem();
	if (Item::itemFrame) {
		this->spawnAtLocation(Item::itemFrame->itemID, 1);
	}
}

bool_t ItemFrame::hurt(Entity* attacker, int32_t damage) {
	if (this->isDead) return 0;
	ItemInstance disp = this->getDisplayedItem();
	if (!disp.isNull() && disp.count > 0) {
		this->removeFramedItem();
		return 1;
	}
	this->dropItem();
	this->remove();
	return 1;
}

bool_t ItemFrame::interactWithPlayer(Player* player) {
	if (!player) return 0;
	ItemInstance disp = this->getDisplayedItem();
	if (disp.isNull() || disp.count <= 0) {
		ItemInstance* sel = player->inventory ? player->inventory->getSelected() : nullptr;
		if (sel && !sel->isNull() && sel->count > 0) {
			this->setItem(*sel);
			if (player->inventory && !player->abilities.instabuild) {
				sel->count--;
				if (sel->count <= 0) {
					player->inventory->clearSlot(player->inventory->selectedSlot);
				}
				player->inventory->setContainerChanged();
			}
			return 1;
		}
	} else {
		this->rotation = (this->getRotation() + 1) % 8;
		this->synchedEntityData.set(10, (int8_t)this->rotation);
		if (this->level) {
			this->level->playSound(this, "random.click", 0.3f, 0.6f + (float)this->rotation * 0.05f);
		}
		return 1;
	}
	return 0;
}

ItemInstance ItemFrame::getDisplayedItem() const {
	if (!this->item.isNull() && this->item.count > 0) {
		return this->item;
	}
	int32_t id = const_cast<SynchedEntityData&>(this->synchedEntityData).getInt(8);
	int32_t aux = const_cast<SynchedEntityData&>(this->synchedEntityData).getInt(9);
	if (id > 0) {
		return ItemInstance(id, 1, aux);
	}
	return ItemInstance();
}

int32_t ItemFrame::getRotation() const {
	return (int32_t)const_cast<SynchedEntityData&>(this->synchedEntityData).getByte(10);
}

void ItemFrame::readAdditionalSaveData(CompoundTag* tag) {
	HangingEntity::readAdditionalSaveData(tag);
	this->rotation = tag->getByte("ItemRotation");
	this->dropChance = tag->contains("ItemDropChance") ? tag->getFloat("ItemDropChance") : 1.0f;
	if (tag->contains("Item")) {
		CompoundTag* itTag = tag->getCompound("Item");
		if (itTag) {
			this->item.load(itTag);
		}
	}
}

void ItemFrame::addAdditonalSaveData(CompoundTag* tag) {
	HangingEntity::addAdditonalSaveData(tag);
	tag->putByte("ItemRotation", (uint8_t)this->rotation);
	tag->putFloat("ItemDropChance", this->dropChance);
	if (!this->item.isNull() && this->item.count > 0) {
		CompoundTag* itTag = new CompoundTag();
		this->item.save(itTag);
		tag->put("Item", itTag);
	}
}

void ItemFrame::setDir(int32_t dir) {
	this->direction = dir;
	float dist = 0.505f;
	float px = (float)this->tileX + 0.5f;
	float py = (float)this->tileY + 0.5f;
	float pz = (float)this->tileZ + 0.5f;

	if (dir == 0) {
		this->yaw = 0.0f;
		this->prevYaw = 0.0f;
		this->pitch = 0.0f;
		pz += dist;
		this->boundingBox = { px - 0.375f, py - 0.375f, pz - 0.0625f, px + 0.375f, py + 0.375f, pz + 0.0625f };
	} else if (dir == 1) {
		this->yaw = 90.0f;
		this->prevYaw = 90.0f;
		this->pitch = 0.0f;
		px -= dist;
		this->boundingBox = { px - 0.0625f, py - 0.375f, pz - 0.375f, px + 0.0625f, py + 0.375f, pz + 0.375f };
	} else if (dir == 2) {
		this->yaw = 180.0f;
		this->prevYaw = 180.0f;
		this->pitch = 0.0f;
		pz -= dist;
		this->boundingBox = { px - 0.375f, py - 0.375f, pz - 0.0625f, px + 0.375f, py + 0.375f, pz + 0.0625f };
	} else if (dir == 3) {
		this->yaw = 270.0f;
		this->prevYaw = 270.0f;
		this->pitch = 0.0f;
		px += dist;
		this->boundingBox = { px - 0.0625f, py - 0.375f, pz - 0.375f, px + 0.0625f, py + 0.375f, pz + 0.375f };
	} else if (dir == 4) {
		this->pitch = 90.0f;
		this->prevPitch = 90.0f;
		py -= dist;
		this->boundingBox = { px - 0.375f, py - 0.0625f, pz - 0.375f, px + 0.375f, py + 0.0625f, pz + 0.375f };
	} else if (dir == 5) {
		this->pitch = -90.0f;
		this->prevPitch = -90.0f;
		int tileId = this->level ? this->level->getTile(this->tileX, this->tileY, this->tileZ) : 0;
		if (tileId > 0 && Tile::tiles[tileId]) {
			Tile* t = Tile::tiles[tileId];
			if (t->getRenderShape() == 19) {
				SlabTile* st = (SlabTile*)t;
				if (st->isBottomSlab(this->level, this->tileX, this->tileY, this->tileZ)) {
					py = (float)this->tileY + 0.505f;
				} else {
					py += dist;
				}
			} else {
				py += dist;
			}
		} else {
			py += dist;
		}
		this->boundingBox = { px - 0.375f, py - 0.0625f, pz - 0.375f, px + 0.375f, py + 0.0625f, pz + 0.375f };
	}
	this->setPos(px, py, pz);
}

bool_t ItemFrame::survives() {
	if (!this->level) return 0;
	int tileId = this->level->getTile(this->tileX, this->tileY, this->tileZ);
	if (tileId <= 0 || !Tile::tiles[tileId]) return 0;
	Tile* t = Tile::tiles[tileId];
	const Material* mat = t->material;
	if (!mat || !mat->isSolid()) return 0;

	for (auto&& e : std::vector<Entity*>(*this->level->getEntities(this, this->boundingBox))) {
		if (e->isHangingEntity()) {
			return 0;
		}
	}
	return 1;
}
