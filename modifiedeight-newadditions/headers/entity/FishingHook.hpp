#pragma once
#include <entity/Entity.hpp>
#include <util/Random.hpp>

struct Player;
struct ItemInstance;
struct CompoundTag;

struct FishingHook: Entity {
	Player* owner;
	int32_t flightTime;
	int32_t nibble;
	int32_t timeUntilLured;
	int32_t timeUntilHooked;
	int32_t life;
	bool_t inGround;
	float fishAngle;
	Random random;

	FishingHook(Level*, Player*);
	FishingHook(Level*, Player*, float, float, float);
	virtual ~FishingHook();

	virtual int32_t getEntityTypeId() const;
	virtual void readAdditionalSaveData(CompoundTag*);
	virtual void addAdditonalSaveData(CompoundTag*);
	virtual void tick();
	virtual bool_t shouldRenderAtSqrDistance(float);
	void catchingFishLogic();
	int32_t retrieve(ItemInstance*);
};
