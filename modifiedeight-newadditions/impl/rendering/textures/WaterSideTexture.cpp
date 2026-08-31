#include <rendering/textures/WaterSideTexture.hpp>
#include <math/Mth.hpp>
#include <tile/Tile.hpp>
#include <Options.hpp>
#include <math.h>

bool_t WaterSideTexture::isEnabled() {
	return Options::instance ? (Options::instance->animateTextures && Options::instance->animateWater) : 1;
}

WaterSideTexture::WaterSideTexture()
	: DynamicTexture(*Tile::water->getTexture(3), 2) {
	this->field_28 = 0;
	this->field_2C = 0;
	this->field_30 = 0;
	this->field_34 = new float[256];
	this->field_38 = new float[256];
	this->field_3C = new float[256];
	this->field_40 = new float[256];
	for(int32_t i = 0; i != 256; ++i) {
		this->field_34[i] = 0;
		this->field_38[i] = 0;
		this->field_3C[i] = 0;
		this->field_40[i] = 0;
	}
}

WaterSideTexture::~WaterSideTexture() {
	if(this->field_34) delete[] this->field_34;
	if(this->field_38) delete[] this->field_38;
	if(this->field_3C) delete[] this->field_3C;
	if(this->field_40) delete[] this->field_40;
}
void WaterSideTexture::tick() {
	++this->field_30;
	for(int32_t i = 0; i != 16; ++i) {
		for(int32_t j = 0; j != 16; ++j) {
			int32_t v3 = i - 2;
			float v4 = 0.0f;
			while(v3 != i + 1) {
				int32_t v5 = v3++ & 0xF;
				v4 = v4 + this->field_34[16 * v5 + j];
			}
			int32_t v6 = j + 16 * i;
			this->field_38[v6] = (float)(v4 / 3.2f) + (float)(this->field_3C[v6] * 0.8f);
		}
	}
	for(int32_t k = 0; k != 256; k += 16) {
		for(int32_t m = 0; m != 16; ++m) {
			int32_t v8 = m * 4 + k * 4;
			this->field_3C[m + k] = this->field_3C[m + k] + (float)(this->field_40[m + k] * 0.05f);
			if(this->field_3C[m + k] < 0.0f) {
				this->field_3C[m + k] = 0.0f;
			}
			this->field_40[v8 / 4] = this->field_40[v8 / 4] - 0.3f;
			if(Mth::fastRandom() <= 0x33333332) {
				this->field_40[v8 / 4] = 0.5f;
			}
		}
	}
	float* v12 = this->field_34;
	this->field_34 = this->field_38;
	this->field_38 = v12;
	for(int32_t idx = 0; idx < 256; ++idx) {
		float v15 = this->field_34[(uint8_t)(idx - 16 * this->field_30)];
		if(v15 > 1.0f) v15 = 1.0f;
		else if(v15 < 0.0f) v15 = 0.0f;
		float v16 = v15 * v15;
		int32_t r = (int32_t)(155.0f + v16 * 60.0f);
		int32_t g = (int32_t)(190.0f + v16 * 50.0f);
		int32_t b = (int32_t)(230.0f + v16 * 25.0f);
		int32_t a = (int32_t)(140.0f + v16 * 60.0f);
		this->data[idx * 4 + 0] = (uint8_t)r;
		this->data[idx * 4 + 1] = (uint8_t)g;
		this->data[idx * 4 + 2] = (uint8_t)b;
		this->data[idx * 4 + 3] = (uint8_t)a;
	}
}
