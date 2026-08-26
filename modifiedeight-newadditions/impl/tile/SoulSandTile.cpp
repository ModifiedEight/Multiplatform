#include <tile/SoulSandTile.hpp>
#include <tile/material/Material.hpp>
#include <entity/Entity.hpp>
#include <level/Level.hpp>

SoulSandTile::SoulSandTile(int32_t id, const std::string& name)
	: Tile(id, name, Material::dirt) {
}

SoulSandTile::~SoulSandTile() {
}

AABB* SoulSandTile::getAABB(Level* level, int32_t x, int32_t y, int32_t z) {
	this->aabb.minX = (float)x;
	this->aabb.minY = (float)y;
	this->aabb.minZ = (float)z;
	this->aabb.maxX = (float)x + 1.0f;
	this->aabb.maxY = (float)y + 0.875f;
	this->aabb.maxZ = (float)z + 1.0f;
	return &this->aabb;
}

void SoulSandTile::entityInside(Level* level, int32_t x, int32_t y, int32_t z, Entity* entity) {
	if (entity) {
		entity->motionX *= 0.4f;
		entity->motionZ *= 0.4f;
	}
}
