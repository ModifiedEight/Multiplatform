#pragma once
#include <entity/Entity.hpp>

struct CompoundTag;

struct SeatEntity : Entity {
	int blockX, blockY, blockZ;

	SeatEntity(Level* level, float x, float y, float z, int bx, int by, int bz);
	virtual ~SeatEntity();

	virtual void tick();
	virtual void positionRider(bool_t isDead);
	virtual bool_t isPickable();
	virtual bool_t isPushable();
	virtual bool_t isShootable();
	virtual bool_t isShadow();
	virtual int32_t getEntityTypeId() const;
	virtual void readAdditionalSaveData(CompoundTag*);
	virtual void addAdditonalSaveData(CompoundTag*);
};
