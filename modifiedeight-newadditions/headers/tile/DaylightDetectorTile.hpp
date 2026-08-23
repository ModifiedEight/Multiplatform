#pragma once
#include <tile/Tile.hpp>
#include <tile/material/Material.hpp>
#include <entity/Player.hpp>
#include <level/Level.hpp>
#include <item/Item.hpp>
#include <item/ItemInstance.hpp>
#include <cmath>
#include <algorithm>

class DaylightDetectorTile : public Tile {
public:
	bool_t inverted;

	DaylightDetectorTile(int32_t id, bool_t inverted)
		: Tile(id, inverted ? "daylight_detector_inverted_top" : "daylight_detector_top", Material::wood), inverted(inverted) {
		this->setShape(0.0f, 0.0f, 0.0f, 1.0f, 0.375f, 1.0f);
	}

	bool_t isSolidRender() override {
		return 0;
	}

	bool_t isCubeShaped() override {
		return 0;
	}

	bool_t isSignalSource() override {
		return 1;
	}

	int32_t getDirectSignal(Level* level, int32_t x, int32_t y, int32_t z, int32_t side) override {
		return side == 1 ? level->getData(x, y, z) : 0;
	}

	int32_t getSignal(LevelSource* level, int32_t x, int32_t y, int32_t z, int32_t side) override {
		return level->getData(x, y, z);
	}

	void onPlace(Level* level, int32_t x, int32_t y, int32_t z) override {
		updateSignal(level, x, y, z);
		level->addToTickNextTick(x, y, z, blockID, 1);
	}

	void neighborChanged(Level* level, int32_t x, int32_t y, int32_t z, int32_t a5, int32_t a6, int32_t a7, int32_t a8) override {
		updateSignal(level, x, y, z);
	}

	void tick(Level* level, int32_t x, int32_t y, int32_t z, Random* rand) override {
		updateSignal(level, x, y, z);
		level->addToTickNextTick(x, y, z, blockID, 10);
	}

	void updateSignal(Level* level, int32_t x, int32_t y, int32_t z) {
		if (!level) return;
		float time = level->getTimeOfDay(1.0f);
		float daylightFactor = cosf(time * 3.14159265f * 2.0f);
		int power = 0;

		if (!inverted) {
			if (daylightFactor > 0.0f) {
				power = (int)roundf(daylightFactor * 15.0f);
				if (power > 15) power = 15;
				if (power < 0) power = 0;
			}
		} else {
			float nightFactor = -daylightFactor;
			if (nightFactor > 0.0f) {
				power = (int)roundf(nightFactor * 15.0f);
				if (power > 15) power = 15;
				if (power < 0) power = 0;
			}
		}

		if (level->getData(x, y, z) != power) {
			level->setData(x, y, z, power, 3);
			level->updateNeighborsAt(x, y, z, blockID);
			level->updateNeighborsAt(x, y - 1, z, blockID);
			level->updateNeighborsAt(x, y + 1, z, blockID);
			level->updateNeighborsAt(x - 1, y, z, blockID);
			level->updateNeighborsAt(x + 1, y, z, blockID);
			level->updateNeighborsAt(x, y, z - 1, blockID);
			level->updateNeighborsAt(x, y, z + 1, blockID);
		}
	}

	bool_t use(Level* level, int32_t x, int32_t y, int32_t z, Player* player) override {
		if (!level) return 0;
		int nextId = inverted ? (Tile::daylightDetector ? Tile::daylightDetector->blockID : 151)
		                      : (Tile::daylightDetectorInverted ? Tile::daylightDetectorInverted->blockID : 178);
		int currentData = level->getData(x, y, z);
		level->setTileAndData(x, y, z, nextId, currentData, 3);
		level->playSound((float)x + 0.5f, (float)y + 0.5f, (float)z + 0.5f, "random.click", 0.3f, inverted ? 0.5f : 0.6f);
		Tile* nextTile = Tile::tiles[nextId];
		if (nextTile) {
			((DaylightDetectorTile*)nextTile)->updateSignal(level, x, y, z);
		}
		return 1;
	}

	TextureUVCoordinateSet* getCarriedTexture(int32_t side, int32_t data) override {
		return getTexture(side, data);
	}

	TextureUVCoordinateSet* getTexture(int32_t side, int32_t data) override {
		if (side == 1) {
			static TextureUVCoordinateSet uv1_day, uv1_night;
			if (inverted) {
				uv1_night = Tile::getTextureUVCoordinateSet("daylight_detector_inverted_top", 0);
				return &uv1_night;
			} else {
				uv1_day = Tile::getTextureUVCoordinateSet("daylight_detector_top", 0);
				return &uv1_day;
			}
		}
		if (side == 0) {
			static TextureUVCoordinateSet uv0;
			uv0 = Tile::getTextureUVCoordinateSet("planks", 0);
			return &uv0;
		}
		static TextureUVCoordinateSet uvs;
		uvs = Tile::getTextureUVCoordinateSet("daylight_detector_side", 0);
		return &uvs;
	}

	TextureUVCoordinateSet* getTexture(int32_t side) override {
		return getTexture(side, 0);
	}

	TextureUVCoordinateSet* getTexture(LevelSource* level, int32_t x, int32_t y, int32_t z, int32_t side) override {
		return getTexture(side, level ? level->getData(x, y, z) : 0);
	}

	int32_t getResource(int32_t data, Random* rand) override {
		return Tile::daylightDetector ? Tile::daylightDetector->blockID : 151;
	}
};
