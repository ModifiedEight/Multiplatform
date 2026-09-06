#include <entity/Boat.hpp>
#include <entity/Player.hpp>
#include <level/Level.hpp>
#include <tile/material/Material.hpp>
#include <item/Item.hpp>
#include <item/ItemInstance.hpp>
#include <nbt/CompoundTag.hpp>
#include <cmath>

Boat::Boat(Level* level)
	: Entity(level) {
	this->entityRenderId = BOAT;
	this->setSize(1.375f, 0.5625f);
	this->damageTime = 0;
	this->damageDir = 1;
	this->damageTaken = 0.0f;
	this->rowingTime[0] = 0.0f;
	this->rowingTime[1] = 0.0f;
	this->deltaRotation = 0.0f;
	this->ridingHeight = 0.0f;

	this->synchedEntityData.define<int32_t>(17, 0);
	this->synchedEntityData.define<int32_t>(18, 1);
	this->synchedEntityData.define<float>(19, 0.0f);
	this->synchedEntityData.define<int32_t>(20, 0);
}

Boat::Boat(Level* level, float x, float y, float z)
	: Boat(level) {
	this->setPos(x, y + 0.3f, z);
	this->motionX = 0.0f;
	this->motionY = 0.0f;
	this->motionZ = 0.0f;
	this->prevX = x;
	this->prevY = y + 0.3f;
	this->prevZ = z;
}

Boat::~Boat() {
}

bool_t Boat::isPickable() {
	return !this->isDead;
}

bool_t Boat::isPushable() {
	return 1;
}

int32_t Boat::getEntityTypeId() const {
	return 40;
}

int32_t Boat::getBoatType() const {
	return const_cast<SynchedEntityData&>(this->synchedEntityData).getInt(20);
}

void Boat::setBoatType(int32_t type) {
	this->synchedEntityData.set(20, type);
}

float Boat::getRowingTime(int32_t paddle, float partialTicks) const {
	return this->rowingTime[paddle];
}

bool_t Boat::hurt(Entity* attacker, int32_t damage) {
	if (this->isDead) return 0;
	this->damageDir = -this->damageDir;
	this->damageTime = 10;
	this->damageTaken += (float)damage * 10.0f;
	this->synchedEntityData.set(17, this->damageTime);
	this->synchedEntityData.set(18, this->damageDir);
	this->synchedEntityData.set(19, this->damageTaken);

	if (attacker && attacker->isPlayer()) {
		Player* p = (Player*)attacker;
		if (p->abilities.instabuild) {
			this->remove();
			return 1;
		}
	}

	if (this->damageTaken > 40.0f) {
		if (this->rider) {
			this->rider->ride(0);
		}
		if (Item::boat) {
			this->spawnAtLocation(Item::boat->itemID, 1, this->getBoatType());
		}
		this->remove();
	}
	return 1;
}

bool_t Boat::interactWithPlayer(Player* player) {
	if (!player) return 0;
	if (this->rider && this->rider != player) {
		return 0;
	}
	if (!this->level->isClientMaybe) {
		if (this->rider == player) {
			player->ride(0);
		} else {
			player->ride(this);
		}
	}
	return 1;
}

float Boat::getRideHeight() {
	return 1.05f;
}

void Boat::positionRider(bool_t isDead) {
	if (!this->rider) return;
	float ry = this->posY + this->getRideHeight();
	this->rider->setPos(this->posX, ry, this->posZ);
}

void Boat::tick() {
	this->prevX = this->posX;
	this->prevY = this->posY;
	this->prevZ = this->posZ;
	this->prevYaw = this->yaw;
	this->prevPitch = this->pitch;

	if (this->damageTime > 0) {
		this->damageTime--;
		this->synchedEntityData.set(17, this->damageTime);
	}
	if (this->damageTaken > 0.0f) {
		this->damageTaken -= 0.1f;
		if (this->damageTaken < 0.0f) this->damageTaken = 0.0f;
		this->synchedEntityData.set(19, this->damageTaken);
	}

	bool_t inWater = 0;
	AABB checkBB = {this->boundingBox.minX, this->boundingBox.minY - 0.2f, this->boundingBox.minZ, this->boundingBox.maxX, this->boundingBox.maxY, this->boundingBox.maxZ};
	if (this->level->containsLiquid(checkBB, Material::water)) {
		inWater = 1;
	}

	if (inWater) {
		if (this->motionY < 0.05f) {
			this->motionY += 0.04f;
		}
		this->motionX *= 0.92f;
		this->motionZ *= 0.92f;
		this->motionY *= 0.8f;
	} else {
		this->motionY -= 0.04f;
		this->motionX *= 0.98f;
		this->motionZ *= 0.98f;
		this->motionY *= 0.98f;
	}

	if (this->rider && this->rider->isPlayer()) {
		Player* p = (Player*)this->rider;
		if (p->isSneaking()) {
			p->ride(0);
		} else {
			if (p->moveStrafe > 0.0f) this->yaw -= 3.0f;
			else if (p->moveStrafe < 0.0f) this->yaw += 3.0f;
			float forward = p->moveForward;
			if (forward != 0.0f) {
				float speed = inWater ? 0.04f : 0.02f;
				float rad = (this->yaw * 3.14159265f) / 180.0f;
				this->motionX += -std::sin(rad) * speed * forward;
				this->motionZ += std::cos(rad) * speed * forward;
				this->rowingTime[0] += 0.25f;
				this->rowingTime[1] += 0.25f;
			} else if (p->moveStrafe != 0.0f) {
				this->rowingTime[0] += 0.15f;
				this->rowingTime[1] += 0.15f;
			}
		}
	}

	float speedH = std::sqrt(this->motionX * this->motionX + this->motionZ * this->motionZ);
	if (speedH > 0.35f) {
		this->motionX = (this->motionX / speedH) * 0.35f;
		this->motionZ = (this->motionZ / speedH) * 0.35f;
	}

	this->move(this->motionX, this->motionY, this->motionZ);
}

void Boat::readAdditionalSaveData(CompoundTag* tag) {
	if (tag->contains("BoatType")) {
		this->setBoatType(tag->getInt("BoatType"));
	}
}

void Boat::addAdditonalSaveData(CompoundTag* tag) {
	tag->putInt("BoatType", this->getBoatType());
}
