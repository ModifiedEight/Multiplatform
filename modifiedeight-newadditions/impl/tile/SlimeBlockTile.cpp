#include <tile/SlimeBlockTile.hpp>
#include <tile/material/Material.hpp>
#include <entity/Entity.hpp>
#include <level/Level.hpp>
#include <cmath>

SlimeBlockTile::SlimeBlockTile(int32_t id, const std::string& texName)
	: Tile(id, Material::cloth) {
	this->tex = *this->getTextureItem(texName);
}

SlimeBlockTile::~SlimeBlockTile() {
}

TextureUVCoordinateSet* SlimeBlockTile::getTexture(int32_t side, int32_t data) {
	TextureAtlasTextureItem* ti = this->getTextureItem("slime");
	if (ti) return ti->getUV(0);
	ti = this->getTextureItem("slime_block");
	if (ti) return ti->getUV(0);
	return this->tex.getUV(0);
}

TextureUVCoordinateSet* SlimeBlockTile::getCarriedTexture(int32_t side, int32_t data) {
	return this->getTexture(side, data);
}

void SlimeBlockTile::fallOn(Level* level, int32_t x, int32_t y, int32_t z, Entity* entity, float damage) {
	if (entity) {
		entity->fallDistance = 0.0f;
	}
}

void SlimeBlockTile::stepOn(Level* level, int32_t x, int32_t y, int32_t z, Entity* entity) {
	if (entity) {
		if (entity->motionY < -0.1f) {
			entity->motionY = -entity->motionY * 0.8f;
		}
		entity->motionX *= 0.25f;
		entity->motionZ *= 0.25f;
	}
}

int32_t SlimeBlockTile::getRenderLayer() {
	return 1;
}

bool_t SlimeBlockTile::isSolidRender() {
	return 0;
}
