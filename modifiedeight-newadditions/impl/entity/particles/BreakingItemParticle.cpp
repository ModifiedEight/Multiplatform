#include <entity/particles/BreakingItemParticle.hpp>
#include <item/Item.hpp>
#include <tile/Tile.hpp>
#include <rendering/Tesselator.hpp>

BreakingItemParticle::BreakingItemParticle(Level* level, ParticleType pt, const std::string& a2, Item* it)
	: Particle(level, pt, a2) {
	this->item = it;
}

BreakingItemParticle::~BreakingItemParticle() {
}

void BreakingItemParticle::init(float a2, float a3, float a4, float a5, float a6, float a7, int32_t a8) {
	if (a8 > 0 && a8 < 512 && Item::items[a8]) {
		this->item = Item::items[a8];
	} else if (!this->item) {
		this->item = Item::snowBall;
	}
	if (this->item) {
		TextureUVCoordinateSet* set = this->item->getIcon(0, 0, 0);
		if (set) this->texture = *set;
	}
	float v17 = this->_scale * 0.5f;
	this->bColMul = 1.0f;
	this->gColMul = 1.0f;
	this->rColMul = 1.0f;
	this->_scale = v17;
	this->gravity = Tile::snow ? Tile::snow->field_3C : 0.0f;
}

void BreakingItemParticle::render(Tesselator& a2, float a3, float a4, float a5, float a6, float a7, float a8) {
	float maxX = this->texture.maxX;
	float maxY = this->texture.maxY;
	float v13 = this->texture.minX + (float)((float)(this->field_138 * 0.25f) * (float)(maxX - this->texture.minX));
	float v14 = this->texture.minY + (float)((float)(this->field_13C * 0.25f) * (float)(maxY - this->texture.minY));
	float v15 = this->_scale * 0.1f;
	float v16 = (float)(this->prevX + (float)((float)(this->posX - this->prevX) * a3)) - Particle::xOff;
	float v17 = (float)(this->prevY + (float)((float)(this->posY - this->prevY) * a3)) - Particle::yOff;
	float v18 = (float)(this->prevZ + (float)((float)(this->posZ - this->prevZ) * a3)) - Particle::zOff;
	float v19 = this->getBrightness(a3);
	a2.color(v19 * this->rColMul, v19 * this->gColMul, v19 * this->bColMul);
	float v20 = a4 * v15;
	float v21 = a7 * v15;
	float v22 = a6 * v15;
	float v23 = a5 * v15;
	float v24 = a8 * v15;
	float v25 = v17 - v23;
	a2.vertexUV((float)(v16 - v20) - v21, v17 - v23, (float)(v18 - v22) - v24, v13, maxY);
	float v26 = v17 + v23;
	a2.vertexUV((float)(v16 - v20) + v21, v26, (float)(v18 - v22) + v24, v13, v14);
	float v27 = v16 + v20;
	float v28 = v18 + v22;
	a2.vertexUV(v27 + v21, v26, v28 + v24, maxX, v14);
	a2.vertexUV(v27 - v21, v25, v28 - v24, maxX, maxY);
}
