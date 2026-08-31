#include <rendering/textures/WaterTexture.hpp>
#include <tile/Tile.hpp>
#include <math/Mth.hpp>
#include <Options.hpp>
#include <math.h>

bool_t WaterTexture::isEnabled() {
	return Options::instance ? (Options::instance->animateTextures && Options::instance->animateWater) : 1;
}


WaterTexture::WaterTexture(const TextureUVCoordinateSet& a2)
	: DynamicTexture(Tile::water->textureUV, 1) {
	this->field_28 = 0;
	this->field_2C = 0;
	this->field_30 = new float[256];
	this->field_34 = new float[256];
	this->field_38 = new float[256];
	this->field_3C = new float[256];
	for(int32_t i = 0; i != 256; ++i) {
		this->field_30[i] = 0;
		this->field_34[i] = 0;
		this->field_38[i] = 0;
		this->field_3C[i] = 0;
	}
}

WaterTexture::~WaterTexture() {
	if(this->field_30) delete[] this->field_30;
	if(this->field_34) delete[] this->field_34;
	if(this->field_38) delete[] this->field_38;
	if(this->field_3C) delete[] this->field_3C;
}
void WaterTexture::tick() {
	for(int32_t i = 0; i != 256; i += 16) {
		for(int32_t j = 0; j != 16; ++j) {
			int32_t v3 = j - 1;
			float v4 = 0.0f;
			while(v3 != j + 2) {
				int32_t v5 = (v3++ & 0xF) + i;
				v4 = v4 + this->field_30[v5];
			}
			int32_t v6 = j + i;
			this->field_34[v6] = (float)(v4 / 3.3f) + (float)(this->field_38[v6] * 0.8f);
		}
	}
	for(int32_t k = 0; k != 256; k += 16) {
		for(int32_t m = 0; m != 16; ++m) {
			int32_t v8 = m * 4 + k * 4;
			this->field_38[m + k] = this->field_38[m + k] + (float)(this->field_3C[m + k] * 0.05f);
			if(this->field_38[m + k] < 0.0f) {
				this->field_38[m + k] = 0.0f;
			}
			this->field_3C[v8 / 4] = this->field_3C[v8 / 4] - 0.1f;
			if(Mth::fastRandom() <= 0xCCCCCCC) {
				this->field_3C[v8 / 4] = 0.5f;
			}
		}
	}
	float* v12 = this->field_34;
	this->field_34 = this->field_30;
	this->field_30 = v12;
	for(int32_t idx = 0; idx < 256; ++idx) {
		float v14 = this->field_30[idx];
		if(v14 > 1.0f) v14 = 1.0f;
		else if(v14 < 0.0f) v14 = 0.0f;
		float v15 = v14 * v14;
		int32_t r = (int32_t)(160.0f + v15 * 60.0f);
		int32_t g = (int32_t)(195.0f + v15 * 50.0f);
		int32_t b = (int32_t)(235.0f + v15 * 20.0f);
		int32_t a = (int32_t)(140.0f + v15 * 60.0f);
		this->data[idx * 4 + 0] = (uint8_t)r;
		this->data[idx * 4 + 1] = (uint8_t)g;
		this->data[idx * 4 + 2] = (uint8_t)b;
		this->data[idx * 4 + 3] = (uint8_t)a;
	}
}
