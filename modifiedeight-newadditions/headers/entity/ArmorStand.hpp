#pragma once
#include <entity/Mob.hpp>
#include <item/ItemInstance.hpp>

struct ArmorStand : Mob {
	ItemInstance armorItems[4];
	ItemInstance handItem;
	bool_t showArms;
	bool_t noBasePlate;

	ArmorStand(Level*);
	ArmorStand(Level*, float x, float y, float z);

	virtual ~ArmorStand();
	virtual bool_t isPickable();
	virtual bool_t isPushable();
	virtual int32_t getEntityTypeId() const;
	virtual int32_t getMaxHealth();
	virtual float getBaseSpeed() { return 0.0f; }
	virtual bool_t hurt(Entity*, int32_t);
	virtual void die(Entity*);
	virtual bool_t interactWithPlayer(Player*);
	virtual bool_t interactPreventDefault() { return 1; }
	virtual void readAdditionalSaveData(CompoundTag*);
	virtual void addAdditonalSaveData(CompoundTag*);
	virtual void tick();
	virtual void aiStep() {}
	virtual void lookAt(Entity*, float, float) {}
	void dropEquipment();
};
