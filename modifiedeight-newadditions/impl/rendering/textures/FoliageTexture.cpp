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

	if (this->isLeaf) {
		float windX = sinf(time * 0.9f) * 0.7f;
		for (int y = 0; y < 16; ++y) {
			float rowSway = sinf(time * 0.85f + (float)y * 0.35f) * this->amplitude * 0.6f;
			int shiftX = (int)roundf(windX + rowSway);
			for (int x = 0; x < 16; ++x) {
				int srcX = x - shiftX;
				int dstIdx = (y * 16 + x) * 4;
				if (srcX >= 0 && srcX < 16) {
					int srcIdx = (y * 16 + srcX) * 4;
					this->data[dstIdx + 0] = this->originalPixels[srcIdx + 0];
					this->data[dstIdx + 1] = this->originalPixels[srcIdx + 1];
					this->data[dstIdx + 2] = this->originalPixels[srcIdx + 2];
					this->data[dstIdx + 3] = this->originalPixels[srcIdx + 3];
				} else {
					int clampedX = (srcX < 0) ? 0 : 15;
					int srcIdx = (y * 16 + clampedX) * 4;
					this->data[dstIdx + 0] = this->originalPixels[srcIdx + 0];
					this->data[dstIdx + 1] = this->originalPixels[srcIdx + 1];
					this->data[dstIdx + 2] = this->originalPixels[srcIdx + 2];
					this->data[dstIdx + 3] = this->originalPixels[srcIdx + 3];
				}
			}
		}
	} else if (this->isReeds) {
		for (int y = 0; y < 16; ++y) {
			float sway = sinf(time * 0.9f + (float)(15 - y) * 0.18f) * this->amplitude * ((float)(15 - y) / 15.0f);
			int shift = (int)roundf(sway);
			for (int x = 0; x < 16; ++x) {
				int srcX = x - shift;
				int dstIdx = (y * 16 + x) * 4;
				if (srcX >= 0 && srcX < 16) {
					int srcIdx = (y * 16 + srcX) * 4;
					this->data[dstIdx + 0] = this->originalPixels[srcIdx + 0];
					this->data[dstIdx + 1] = this->originalPixels[srcIdx + 1];
					this->data[dstIdx + 2] = this->originalPixels[srcIdx + 2];
					this->data[dstIdx + 3] = this->originalPixels[srcIdx + 3];
				} else {
					this->data[dstIdx + 0] = 0;
					this->data[dstIdx + 1] = 0;
					this->data[dstIdx + 2] = 0;
					this->data[dstIdx + 3] = 0;
				}
			}
		}
	} else if (this->isWaterlily) {
		int shiftX = (int)roundf(sinf(time * 0.7f) * 1.0f);
		int shiftY = (int)roundf(cosf(time * 0.5f) * 0.8f);
		for (int y = 0; y < 16; ++y) {
			for (int x = 0; x < 16; ++x) {
				int srcX = (x - shiftX) & 15;
				int srcY = (y - shiftY) & 15;
				int srcIdx = (srcY * 16 + srcX) * 4;
				int dstIdx = (y * 16 + x) * 4;
				this->data[dstIdx + 0] = this->originalPixels[srcIdx + 0];
				this->data[dstIdx + 1] = this->originalPixels[srcIdx + 1];
				this->data[dstIdx + 2] = this->originalPixels[srcIdx + 2];
				this->data[dstIdx + 3] = this->originalPixels[srcIdx + 3];
			}
		}
	} else if (this->isVine) {
		for (int y = 0; y < 16; ++y) {
			float swayFactor = 0.2f + 0.8f * ((float)y / 15.0f);
			int shift = (int)roundf(sinf(time * 1.1f + (float)y * 0.25f) * this->amplitude * swayFactor);
			for (int x = 0; x < 16; ++x) {
				int srcX = x - shift;
				int dstIdx = (y * 16 + x) * 4;
				if (srcX >= 0 && srcX < 16) {
					int srcIdx = (y * 16 + srcX) * 4;
					this->data[dstIdx + 0] = this->originalPixels[srcIdx + 0];
					this->data[dstIdx + 1] = this->originalPixels[srcIdx + 1];
					this->data[dstIdx + 2] = this->originalPixels[srcIdx + 2];
					this->data[dstIdx + 3] = this->originalPixels[srcIdx + 3];
				} else {
					this->data[dstIdx + 0] = 0;
					this->data[dstIdx + 1] = 0;
					this->data[dstIdx + 2] = 0;
					this->data[dstIdx + 3] = 0;
				}
			}
		}
	} else if (this->isDoublePlantBottom) {
		for (int y = 0; y < 16; ++y) {
			float gh = (float)(15 - y);
			float swayFactor = gh / 30.0f;
			int shift = (int)roundf(sinf(time + (30.0f - gh) * 0.12f) * this->amplitude * swayFactor);
			for (int x = 0; x < 16; ++x) {
				int srcX = x - shift;
				if (srcX >= 0 && srcX < 16) {
					int srcIdx = (y * 16 + srcX) * 4;
					int dstIdx = (y * 16 + x) * 4;
					this->data[dstIdx + 0] = this->originalPixels[srcIdx + 0];
					this->data[dstIdx + 1] = this->originalPixels[srcIdx + 1];
					this->data[dstIdx + 2] = this->originalPixels[srcIdx + 2];
					this->data[dstIdx + 3] = this->originalPixels[srcIdx + 3];
				}
			}
		}
	} else if (this->isDoublePlantTop) {
		for (int y = 0; y < 16; ++y) {
			float gh = 15.0f + (float)(15 - y);
			float swayFactor = gh / 30.0f;
			int shift = (int)roundf(sinf(time + (30.0f - gh) * 0.12f) * this->amplitude * swayFactor);
			for (int x = 0; x < 16; ++x) {
				int srcX = x - shift;
				if (srcX >= 0 && srcX < 16) {
					int srcIdx = (y * 16 + srcX) * 4;
					int dstIdx = (y * 16 + x) * 4;
					this->data[dstIdx + 0] = this->originalPixels[srcIdx + 0];
					this->data[dstIdx + 1] = this->originalPixels[srcIdx + 1];
					this->data[dstIdx + 2] = this->originalPixels[srcIdx + 2];
					this->data[dstIdx + 3] = this->originalPixels[srcIdx + 3];
				}
			}
		}
	} else {
		for (int y = 0; y < 16; ++y) {
			float swayFactor = 0.0f;
			if (y < 16 - this->fixedRows) {
				swayFactor = (float)(15 - this->fixedRows - y) / (float)(15 - this->fixedRows);
			}
			int shift = (int)roundf(sinf(time + (float)y * 0.2f) * this->amplitude * swayFactor);

			for (int x = 0; x < 16; ++x) {
				int srcX = x - shift;
				if (srcX >= 0 && srcX < 16) {
					int srcIdx = (y * 16 + srcX) * 4;
					int dstIdx = (y * 16 + x) * 4;
					this->data[dstIdx + 0] = this->originalPixels[srcIdx + 0];
					this->data[dstIdx + 1] = this->originalPixels[srcIdx + 1];
					this->data[dstIdx + 2] = this->originalPixels[srcIdx + 2];
					this->data[dstIdx + 3] = this->originalPixels[srcIdx + 3];
				}
			}
		}
	}
}
