#include <rendering/textures/FoliageTexture.hpp>
#include <NinecraftApp.hpp>
#include <rendering/EntityRenderDispatcher.hpp>
#include <rendering/entity/EntityRenderer.hpp>
#include <Options.hpp>
#include <string.h>
#include <math.h>

extern uint8_t* g_terrainAtlasPixels;
extern int g_terrainAtlasWidth;
extern int g_terrainAtlasHeight;

FoliageTexture::FoliageTexture(const TextureUVCoordinateSet& uvCoord, float speed, float phase, float amplitude, int fixedRows, bool isVine, bool isLeaf, bool isReeds, bool isWaterlily, bool isDoublePlantTop, bool isDoublePlantBottom)
	: DynamicTexture(uvCoord, 1)
	, speed(speed)
	, phase(phase)
	, amplitude(amplitude)
	, fixedRows(fixedRows)
	, isVine(isVine)
	, isLeaf(isLeaf)
	, isReeds(isReeds)
	, isWaterlily(isWaterlily)
	, isDoublePlantTop(isDoublePlantTop)
	, isDoublePlantBottom(isDoublePlantBottom) {
	this->ticks = 0;
	this->initialized = false;
	memset(this->originalPixels, 0, 1024);
}

FoliageTexture::~FoliageTexture() {
}

void FoliageTexture::tick() {
	if (!this->initialized) {
		if (g_terrainAtlasPixels && this->uv.minX >= 0.0f && this->uv.minY >= 0.0f) {
			int startX = (int)floorf(this->uv.minX * (float)g_terrainAtlasWidth);
			int startY = (int)floorf(this->uv.minY * (float)g_terrainAtlasHeight);
			if (startX >= 0 && startX + 16 <= g_terrainAtlasWidth &&
			    startY >= 0 && startY + 16 <= g_terrainAtlasHeight) {
				for (int y = 0; y < 16; ++y) {
					for (int x = 0; x < 16; ++x) {
						int atlasIdx = ((startY + y) * g_terrainAtlasWidth + (startX + x)) * 4;
						int dstIdx = (y * 16 + x) * 4;
						this->originalPixels[dstIdx + 0] = g_terrainAtlasPixels[atlasIdx + 0];
						this->originalPixels[dstIdx + 1] = g_terrainAtlasPixels[atlasIdx + 1];
						this->originalPixels[dstIdx + 2] = g_terrainAtlasPixels[atlasIdx + 2];
						this->originalPixels[dstIdx + 3] = g_terrainAtlasPixels[atlasIdx + 3];
					}
				}
				memcpy(this->data, this->originalPixels, 1024);
				this->initialized = true;
			} else {
				return;
			}
		} else {
			return;
		}
	}

	bool animate = true;
	if (Options::instance) {
		animate = Options::instance->animateTextures;
	}
	if (NinecraftApp::instance && NinecraftApp::instance->currentScreen != nullptr) {
		animate = false;
	}

	if (!animate) {
		memcpy(this->data, this->originalPixels, 1024);
		return;
	}

	++this->ticks;
	float time = (float)this->ticks * this->speed + this->phase;
	memset(this->data, 0, 1024);

	auto sampleRow = [this](int y, float shift) {
		for (int x = 0; x < 16; ++x) {
			float srcX = (float)x - shift;
			int dstIdx = (y * 16 + x) * 4;
			if (srcX >= 0.0f && srcX <= 15.0f) {
				int x0 = (int)floorf(srcX);
				int x1 = (x0 < 15) ? x0 + 1 : 15;
				float t = srcX - (float)x0;
				int idx0 = (y * 16 + x0) * 4;
				int idx1 = (y * 16 + x1) * 4;
				float a0 = (float)this->originalPixels[idx0 + 3];
				float a1 = (float)this->originalPixels[idx1 + 3];
				float a = (1.0f - t) * a0 + t * a1;
				if (a > 10.0f) {
					float invA = 255.0f / a;
					float r = ((1.0f - t) * (float)this->originalPixels[idx0 + 0] * (a0 / 255.0f) + t * (float)this->originalPixels[idx1 + 0] * (a1 / 255.0f)) * invA;
					float g = ((1.0f - t) * (float)this->originalPixels[idx0 + 1] * (a0 / 255.0f) + t * (float)this->originalPixels[idx1 + 1] * (a1 / 255.0f)) * invA;
					float b = ((1.0f - t) * (float)this->originalPixels[idx0 + 2] * (a0 / 255.0f) + t * (float)this->originalPixels[idx1 + 2] * (a1 / 255.0f)) * invA;
					this->data[dstIdx + 0] = (uint8_t)fminf(255.0f, fmaxf(0.0f, r));
					this->data[dstIdx + 1] = (uint8_t)fminf(255.0f, fmaxf(0.0f, g));
					this->data[dstIdx + 2] = (uint8_t)fminf(255.0f, fmaxf(0.0f, b));
					this->data[dstIdx + 3] = (uint8_t)fminf(255.0f, fmaxf(0.0f, a));
				} else {
					this->data[dstIdx + 0] = 0;
					this->data[dstIdx + 1] = 0;
					this->data[dstIdx + 2] = 0;
					this->data[dstIdx + 3] = 0;
				}
			} else {
				this->data[dstIdx + 0] = 0;
				this->data[dstIdx + 1] = 0;
				this->data[dstIdx + 2] = 0;
				this->data[dstIdx + 3] = 0;
			}
		}
	};

	if (this->isLeaf) {
		float windX = sinf(time * 0.9f) * 0.4f;
		for (int y = 0; y < 16; ++y) {
			float rowSway = sinf(time * 0.85f + (float)y * 0.35f) * this->amplitude * 0.3f;
			sampleRow(y, windX + rowSway);
		}
	} else if (this->isReeds) {
		for (int y = 0; y < 16; ++y) {
			float sway = sinf(time * 0.9f + (float)(15 - y) * 0.18f) * this->amplitude * ((float)(15 - y) / 15.0f);
			sampleRow(y, sway);
		}
	} else if (this->isWaterlily) {
		float shiftX = sinf(time * 0.7f) * 0.6f;
		for (int y = 0; y < 16; ++y) {
			sampleRow(y, shiftX);
		}
	} else if (this->isVine) {
		for (int y = 0; y < 16; ++y) {
			float swayFactor = 0.2f + 0.8f * ((float)y / 15.0f);
			float shift = sinf(time * 1.1f + (float)y * 0.25f) * this->amplitude * swayFactor;
			sampleRow(y, shift);
		}
	} else if (this->isDoublePlantBottom) {
		for (int y = 0; y < 16; ++y) {
			float gh = (float)(15 - y);
			float swayFactor = gh / 30.0f;
			float shift = sinf(time + (30.0f - gh) * 0.12f) * this->amplitude * swayFactor;
			sampleRow(y, shift);
		}
	} else if (this->isDoublePlantTop) {
		for (int y = 0; y < 16; ++y) {
			float gh = 15.0f + (float)(15 - y);
			float swayFactor = gh / 30.0f;
			float shift = sinf(time + (30.0f - gh) * 0.12f) * this->amplitude * swayFactor;
			sampleRow(y, shift);
		}
	} else {
		for (int y = 0; y < 16; ++y) {
			float swayFactor = 0.0f;
			if (y < 16 - this->fixedRows) {
				swayFactor = (float)(15 - this->fixedRows - y) / (float)(15 - this->fixedRows);
			}
			float shift = sinf(time + (float)y * 0.2f) * this->amplitude * swayFactor;
			sampleRow(y, shift);
		}
	}
}
