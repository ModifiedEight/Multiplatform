#pragma once
#include <entity/Entity.hpp>

struct Boat : Entity {
	int32_t damageTime;
	int32_t damageDir;
	float damageTaken;
	float rowingTime[2];
	float deltaRotation;

	Boat(Level*);
	Boat(Level*, float x, float y, float z);
	virtual ~Boat();

	virtual void tick();
	virtual bool_t isPickable();
	virtual bool_t isPushable();
	virtual bool_t hurt(Entity*, int32_t);
	virtual bool_t interactWithPlayer(Player*);
	virtual void positionRider(bool_t isDead = false) override;
	virtual float getRideHeight() override;
	virtual int32_t getEntityTypeId() const;
	virtual void readAdditionalSaveData(CompoundTag*);
	virtual void addAdditonalSaveData(CompoundTag*);

	int32_t getBoatType() const;
	void setBoatType(int32_t);
	float getRowingTime(int32_t paddle, float partialTicks) const;
};
