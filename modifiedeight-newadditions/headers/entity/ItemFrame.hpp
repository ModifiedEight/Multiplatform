#pragma once
#include <entity/HangingEntity.hpp>
#include <item/ItemInstance.hpp>

struct ItemFrame : HangingEntity {
	ItemInstance item;
	int32_t rotation;
	float dropChance;

	ItemFrame(Level*);
	ItemFrame(Level*, int32_t x, int32_t y, int32_t z, int32_t dir);

	virtual ~ItemFrame();
	virtual bool_t isPickable();
	virtual int32_t getEntityTypeId() const;
	virtual void readAdditionalSaveData(CompoundTag*);
	virtual void addAdditonalSaveData(CompoundTag*);
	virtual int32_t getWidth();
	virtual int32_t getHeight();
	virtual void dropItem();
	virtual bool_t hurt(Entity*, int32_t);
	virtual bool_t interactWithPlayer(Player*);
	void setItem(const ItemInstance&);
	void removeFramedItem();
	ItemInstance getDisplayedItem() const;
	int32_t getRotation() const;
};
