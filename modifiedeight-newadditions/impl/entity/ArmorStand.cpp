#include <entity/ArmorStand.hpp>
#include <level/Level.hpp>
#include <entity/Player.hpp>
#include <item/Item.hpp>
#include <item/ArmorItem.hpp>
#include <inventory/Inventory.hpp>
#include <nbt/CompoundTag.hpp>
#include <nbt/ListTag.hpp>

ArmorStand::ArmorStand(Level* level)
	: Mob(level)
	, showArms(0)
	, noBasePlate(0) {
	this->entityRenderId = (EntityRendererId)37;
	this->setSize(0.5f, 1.975f);
}

ArmorStand::ArmorStand(Level* level, float x, float y, float z)
	: Mob(level)
	, showArms(0)
	, noBasePlate(0) {
	this->entityRenderId = (EntityRendererId)37;
	this->setSize(0.5f, 1.975f);
	this->setPos(x, y, z);
}

ArmorStand::~ArmorStand() {
}

bool_t ArmorStand::isPickable() {
	return 1;
}

bool_t ArmorStand::isPushable() {
	return 0;
}

int32_t ArmorStand::getEntityTypeId() const {
	return 78;
}

int32_t ArmorStand::getMaxHealth() {
	return 6;
}

void ArmorStand::tick() {
	Entity::tick();
	this->motionX = 0.0f;
	this->motionZ = 0.0f;
	if (this->motionY > 0.0f) this->motionY = 0.0f;
	this->headYaw = this->yaw;
	this->prevHeadYaw = this->yaw;
	this->field_128 = this->yaw;
	this->field_124 = this->yaw;
}

void ArmorStand::dropEquipment() {
	for (int i = 0; i < 4; ++i) {
		if (!this->armorItems[i].isNull() && this->armorItems[i].count > 0) {
			this->spawnAtLocation(this->armorItems[i], 0.0f);
			this->armorItems[i] = ItemInstance();
		}
	}
	if (!this->handItem.isNull() && this->handItem.count > 0) {
		this->spawnAtLocation(this->handItem, 0.0f);
		this->handItem = ItemInstance();
	}
	if (Item::armorStand) {
		this->spawnAtLocation(Item::armorStand->itemID, 1);
	}
}

void ArmorStand::die(Entity* source) {
	Mob::die(source);
	this->dropEquipment();
}

bool_t ArmorStand::hurt(Entity* attacker, int32_t damage) {
	if (this->isDead) return 0;
	if (attacker && attacker->isPlayer()) {
		Player* p = (Player*)attacker;
		if (p->inventory && p->inventory->field_20 != 0) {
			this->remove();
			return 1;
		}
	}
	this->dropEquipment();
	this->remove();
	if (this->level) {
		this->level->playSound(this, "dig.wood", 1.0f, 1.0f);
	}
	return 1;
}

bool_t ArmorStand::interactWithPlayer(Player* player) {
	if (!player || !player->inventory) return 0;

	ItemInstance* held = player->inventory->getSelected();
	if (held && !held->isNull() && held->count > 0) {
		Item* it = Item::items[held->getId()];
		if (it && it->isArmor()) {
			ArmorItem* armor = (ArmorItem*)it;
			int slot = armor->field_48; // 0=helmet, 1=chestplate, 2=leggings, 3=boots
			if (slot >= 0 && slot < 4) {
				if (this->armorItems[slot].isNull() || this->armorItems[slot].count <= 0) {
					this->armorItems[slot] = *held;
					this->armorItems[slot].count = 1;
					if (player->inventory->field_20 == 0) {
						held->count--;
						if (held->count <= 0) {
							player->inventory->clearSlot(player->inventory->selectedSlot);
						}
					}
					player->inventory->setContainerChanged();
					if (this->level) this->level->playSound(this, "mob.armorstand.place", 1.0f, 1.0f);
					return 1;
				}
			}
		}
	}

	for (int i = 0; i < 4; ++i) {
		if (!this->armorItems[i].isNull() && this->armorItems[i].count > 0) {
			player->inventory->add(&this->armorItems[i]);
			this->armorItems[i] = ItemInstance();
			player->inventory->setContainerChanged();
			if (this->level) this->level->playSound(this, "mob.armorstand.place", 1.0f, 1.0f);
			return 1;
		}
	}

	return 0;
}

void ArmorStand::readAdditionalSaveData(CompoundTag* tag) {
	Mob::readAdditionalSaveData(tag);
	this->showArms = tag->getByte("ShowArms") != 0;
	this->noBasePlate = tag->getByte("NoBasePlate") != 0;
	if (tag->contains("ArmorItems")) {
		ListTag* list = tag->getList("ArmorItems");
		if (list) {
			for (size_t i = 0; i < 4 && i < list->value.size(); ++i) {
				CompoundTag* itTag = (CompoundTag*)list->value[i];
				if (itTag) this->armorItems[i].load(itTag);
			}
		}
	}
}

void ArmorStand::addAdditonalSaveData(CompoundTag* tag) {
	Mob::addAdditonalSaveData(tag);
	tag->putByte("ShowArms", this->showArms ? 1 : 0);
	tag->putByte("NoBasePlate", this->noBasePlate ? 1 : 0);
	ListTag* list = new ListTag();
	for (int i = 0; i < 4; ++i) {
		CompoundTag* itTag = new CompoundTag();
		if (!this->armorItems[i].isNull() && this->armorItems[i].count > 0) {
			this->armorItems[i].save(itTag);
		}
		list->add(itTag);
	}
	tag->put("ArmorItems", list);
}
