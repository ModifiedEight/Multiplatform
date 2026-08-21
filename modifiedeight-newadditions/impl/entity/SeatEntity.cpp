#include <entity/SeatEntity.hpp>
#include <level/Level.hpp>
#include <util/CushionManager.hpp>

SeatEntity::SeatEntity(Level* level, float x, float y, float z, int bx, int by, int bz)
	: Entity(level), blockX(bx), blockY(by), blockZ(bz) {
	this->setSize(0.1f, 0.1f);
	this->setPos(x, y, z);
	this->ridingHeight = 0.0f;
	this->noclip = true;
	this->entityRenderId = (EntityRendererId)0;
}

SeatEntity::~SeatEntity() {
}

void SeatEntity::positionRider(bool_t isDead) {
	if (this->rider) {
		float rideH = this->rider->getRidingHeight();
		this->rider->setPos(this->posX, this->posY + rideH, this->posZ);
		this->rider->motionY = 0.0f;
		this->rider->fallDistance = 0.0f;
		this->rider->onGround = true;
	}
}

void SeatEntity::tick() {
	Entity::tick();
	if (!this->level || this->level->isClientMaybe) return;
	if (!this->rider || this->rider->isDead || this->rider->ridingAt != this) {
		this->remove();
		return;
	}
	this->rider->motionY = 0.0f;
	this->rider->onGround = true;
	if (!CushionManager::hasCushion(this->level, this->blockX, this->blockY, this->blockZ)) {
		this->rider->setPos(this->posX, (float)this->blockY + 1.0f + this->rider->ridingHeight, this->posZ);
		this->rider->fallDistance = 0.0f;
		this->rider->motionY = 0.0f;
		this->rider->ride(0);
		this->remove();
	}
}

bool_t SeatEntity::isPickable() { return 0; }
bool_t SeatEntity::isPushable() { return 0; }
bool_t SeatEntity::isShootable() { return 0; }
bool_t SeatEntity::isShadow() { return 0; }
int32_t SeatEntity::getEntityTypeId() const { return 0; }
void SeatEntity::readAdditionalSaveData(CompoundTag*) {}
void SeatEntity::addAdditonalSaveData(CompoundTag*) {}
