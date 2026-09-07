#include <entity/Slime.hpp>
#include <entity/Player.hpp>
#include <level/Level.hpp>
#include <level/biome/Biome.hpp>
#include <tile/Tile.hpp>
#include <item/Item.hpp>
#include <math/Mth.hpp>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Slime::Slime(Level* level)
	: Monster(level) {
	this->entityRenderId = SLIME;
	int32_t r = this->random.genrand_int32() % 3;
	this->slimeSize = (r == 0) ? 1 : ((r == 1) ? 2 : 4);
	this->setSlimeSize(this->slimeSize);
	this->squish = 0.0f;
	this->oSquish = 0.0f;
	this->targetSquish = 0.0f;
	this->jumpDelay = this->random.genrand_int32() % 20 + 10;
}

Slime::~Slime() {
}

void Slime::setSlimeSize(int32_t size) {
	this->slimeSize = size;
	this->setSize(0.6f * (float)size, 0.6f * (float)size);
	this->health = size * size;
	this->attackDamage = (size > 1) ? size : 0;
}

void Slime::playerTouch(Player* player) {
	if (this->slimeSize > 1 && player && !player->abilities.invulnerable && !player->abilities.instabuild) {
		float reach = 0.6f * (float)this->slimeSize + 0.3f;
		float dx = player->posX - this->posX;
		float dz = player->posZ - this->posZ;
		float dy = (player->posY + player->entityHeight * 0.5f) - (this->posY + this->entityHeight * 0.5f);
		if (dx * dx + dz * dz < reach * reach && fabsf(dy) < (this->entityHeight * 0.5f + player->entityHeight * 0.5f + 0.3f)) {
			if (this->attackTime <= 0) {
				this->attackTime = 20;
				if (player->hurt(this, this->slimeSize)) {
					this->level->playSound(this, "mob.slime.big", 1.0f, ((this->random.nextFloat() - this->random.nextFloat()) * 0.2f + 1.0f) * 0.8f);
				}
			}
		}
	}
}

void Slime::checkHurtTarget(Entity* a2, float a3) {
	if (this->slimeSize > 1 && a2) {
		float reach = 0.6f * (float)this->slimeSize + 0.3f;
		if (this->attackTime <= 0 && a3 < reach && a2->boundingBox.maxY > this->boundingBox.minY && a2->boundingBox.minY < this->boundingBox.maxY) {
			this->attackTime = this->getAttackTime();
			this->doHurtTarget(a2);
		}
	}
}

int32_t Slime::getEntityTypeId() const {
	return 37;
}

static std::string _slimeTex = "mob/slime.png";
std::string* Slime::getTexture() {
	return &_slimeTex;
}

int32_t Slime::getMaxHealth() {
	return this->slimeSize * this->slimeSize;
}

float Slime::getBaseSpeed() {
	return 0.3f;
}

void Slime::aiStep() {
	Monster::aiStep();

	if (this->slimeSize > 1 && this->level) {
		AABB hitBox{.minX = this->boundingBox.minX - 0.3f, .minY = this->boundingBox.minY - 0.3f, .minZ = this->boundingBox.minZ - 0.3f, .maxX = this->boundingBox.maxX + 0.3f, .maxY = this->boundingBox.maxY + 0.3f, .maxZ = this->boundingBox.maxZ + 0.3f};
		std::vector<Entity*>* nearby = this->level->getEntities(this, hitBox);
		if (nearby) {
			for (size_t i = 0; i < nearby->size(); ++i) {
				Entity* ent = nearby->at(i);
				if (ent && ent->isPlayer()) {
					this->playerTouch((Player*)ent);
				}
			}
		}
	}

	this->oSquish = this->squish;
	this->squish += (this->targetSquish - this->squish) * 0.5f;

	if (this->onGround) {
		if (this->targetSquish != 0.0f) {
			this->targetSquish = 0.0f;
		}

		if (--this->jumpDelay <= 0) {
			this->jumpDelay = this->random.genrand_int32() % 20 + 10;
			this->targetSquish = -0.5f;

			Player* target = this->level->getNearestPlayer(this->posX, this->posY, this->posZ, 16.0f);
			if (target && !target->abilities.instabuild && !target->abilities.invulnerable) {
				float dx = target->posX - this->posX;
				float dz = target->posZ - this->posZ;
				this->yaw = (float)(atan2f(dz, dx) * 180.0 / M_PI) - 90.0f;
			} else {
				this->yaw += (this->random.nextFloat() - 0.5f) * 90.0f;
			}

			float rad = this->yaw * (float)M_PI / 180.0f;
			float jumpSpeed = 0.2f + 0.1f * this->slimeSize;
			this->motionX = -Mth::sin(rad) * jumpSpeed;
			this->motionZ = Mth::cos(rad) * jumpSpeed;
			this->motionY = 0.42f;
			const char* sname = this->slimeSize > 1 ? "mob.slime.big" : "mob.slime.small";
			this->level->playSound(this, sname, 1.0f, ((this->random.nextFloat() - this->random.nextFloat()) * 0.2f + 1.0f) * 0.8f);
		} else {
			this->motionX = 0.0f;
			this->motionZ = 0.0f;
		}
	} else {
		this->targetSquish = 1.0f;
	}
}

std::string Slime::getHurtSound() {
	return this->slimeSize > 1 ? "mob.slime.big" : "mob.slime.small";
}

std::string Slime::getDeathSound() {
	return this->slimeSize > 1 ? "mob.slime.big" : "mob.slime.small";
}

bool_t Slime::canSpawn() {
	if (!this->level) return 0;
	int32_t x = (int32_t)floorf(this->posX);
	int32_t y = (int32_t)floorf(this->posY);
	int32_t z = (int32_t)floorf(this->posZ);
	if (y <= 1 || y >= 127) return 0;
	Biome* b = this->level->getBiome(x, z);
	if (b == Biome::swampland) {
		int32_t tileBelow = this->level->getTile(x, y - 1, z);
		int32_t tileAt = this->level->getTile(x, y, z);
		if (tileAt != 0 && tileAt != Tile::waterLily->blockID && tileAt != Tile::tallgrass->blockID && tileAt != Tile::water->blockID && tileAt != Tile::calmWater->blockID && Tile::tiles[tileAt] && Tile::tiles[tileAt]->isSolidRender()) return 0;
		return (y >= 45 && y <= 75 && (tileBelow == Tile::grass->blockID || tileBelow == Tile::dirt->blockID || tileBelow == Tile::sand->blockID || tileBelow == Tile::leaves->blockID || tileBelow == Tile::waterLily->blockID || tileBelow == Tile::water->blockID || tileBelow == Tile::calmWater->blockID || tileBelow == Tile::clay->blockID || tileBelow == Tile::gravel->blockID || this->level->isSolidBlockingTile(x, y - 1, z)));
	}
	return this->level->isSolidBlockingTile(x, y - 1, z) && !this->level->isSolidBlockingTile(x, y, z);
}

int32_t Slime::getMaxSpawnClusterSize() {
	return 8;
}

void Slime::die(Entity* cause) {
	if (this->slimeSize > 1 && this->level) {
		int newSize = this->slimeSize / 2;
		int count = 2 + (this->random.genrand_int32() % 3);
		for (int i = 0; i < count; ++i) {
			float ox = ((float)(i % 2) - 0.5f) * (float)newSize * 0.5f;
			float oz = ((float)(i / 2) - 0.5f) * (float)newSize * 0.5f;
			Slime* child = new Slime(this->level);
			child->setSlimeSize(newSize);
			child->moveTo(this->posX + ox, this->posY + 0.5f, this->posZ + oz, this->random.nextFloat() * 360.0f, 0.0f);
			this->level->addEntity(child);
		}
	}
	Monster::die(cause);
}

void Slime::dropDeathLoot() {
	if (this->slimeSize <= 1 && Item::slimeBall) {
		int count = (this->random.genrand_int32() % 3);
		if (count > 0) {
			this->spawnAtLocation(Item::slimeBall->itemID, count, 0);
		}
	}
}
