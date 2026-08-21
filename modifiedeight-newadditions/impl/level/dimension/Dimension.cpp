#include <level/dimension/Dimension.hpp>
#include <level/BiomeSource.hpp>
#include <level/Level.hpp>
#include <level/dimension/NormalDayCycleDimension.hpp>
#include <level/gen/RandomLevelSource.hpp>
#include <level/gen/NewRandomLevelSource.hpp>
#include <math/Mth.hpp>
#include <tile/Tile.hpp>
#include <Options.hpp>

Dimension::Dimension() {
	this->biomeSourcePtr = 0;
	this->field_C = 0;
	this->isOverworldMaybe = 0;
	this->hasNoSkyMaybe = 0;
	this->id = 0;
}
int32_t Dimension::getMoonPhase(int32_t a2) {
	return a2 / 19200 % 8;
}
Dimension* Dimension::getNew(int32_t id) {
	if(id == 10) {
		return new NormalDayCycleDimension();
	} else if(id == 0) {
		return new Dimension();
	}
	return 0;
}

Dimension::~Dimension() {
	delete this->biomeSourcePtr;
}
void Dimension::init(struct Level* level) {
	this->levelPtr = level;
	this->init();
	this->updateLightRamp();
}
struct ChunkSource* Dimension::createRandomLevelSource(void) {
	int32_t seed;		  // r7
	LevelData* levelData; // r0
	int32_t genVer;		  // r6
	Level* level;		  // r0
	bool_t spawnMobs;	  // r5

	seed = this->levelPtr->getSeed();
	levelData = this->levelPtr->getLevelData();
	genVer = levelData->getGeneratorVersion();
	level = this->levelPtr;
	if(level->isClientMaybe) {
		spawnMobs = 0;
	} else {
		spawnMobs = level->getLevelData()->getSpawnMobs();
	}

	if (genVer >= 1) {
		return new NewRandomLevelSource(this->levelPtr, seed, genVer, spawnMobs);
	}
	return new RandomLevelSource(this->levelPtr, seed, genVer, spawnMobs);
}

bool_t Dimension::isValidSpawn(int32_t x, int32_t z) {
	int32_t topTile = this->levelPtr->getTopTile(x, z);
	if (topTile <= 0 || topTile >= 256 || !Tile::tiles[topTile]) {
		return 0;
	}
	if (topTile == Tile::invisible_bedrock->blockID) {
		return 0;
	} else {
		return Tile::tiles[topTile]->isSolidRender();
	}
}
bool_t Dimension::isNaturalDimension() {
	return 0;
}
float Dimension::getTimeOfDay(int32_t, float) { //long, float
	return 1.0;
}
Color4 Dimension::getSunriseColor(float a3, float a4) {
	float v4; // s15
	float v5; // s15
	float v6; // s13
	float v7; // s10
	float v8; // s15

	v4 = Mth::cos((float)((float)(a3 * 3.1416) + (float)(a3 * 3.1416)));
	if(v4 >= -0.4 && v4 <= 0.4) {
		v5 = (float)((float)((float)(v4 + 0.0) / 0.4) * 0.5) + 0.5;
		v6 = 1.0 - (float)((float)(1.0 - Mth::sin((float)(v5 * 3.1416))) * 0.99);
		v7 = (float)(v5 * 0.3) + 0.7;
		v8 = v5 * v5;
		return Color4(v7 * 0.85f, ((float)(v8 * 0.7) + 0.2) * 0.85f, ((float)(v8 * 0.0) + 0.2) * 0.85f, (v6 * v6) * 0.75f);
	}
	return Color4(0, 0, 0, 0);
}
Color4 Dimension::getFogColor(float a3, float a4) {
	float v4; // r3
	float v5; // s15
	float v6; // s14

	v4 = Mth::cos((float)((float)(a3 * 3.1416) + (float)(a3 * 3.1416)));
	v5 = (float)(v4 + v4) + 0.5;
	if(v5 > 1.0) {
		v5 = 1.0;
	} else if(v5 <= 0.0) {
		v5 = 0.0;
	}
	v6 = (float)(v5 * 0.94) + 0.06;
	return Color4(v6 * 0.50196, v6 * 0.8549, v6, 1);
}
float Dimension::getCloudHeight() {
	return 192.0f;
}
bool_t Dimension::mayRespawn(void) {
	return 1;
}
void Dimension::updateLightRamp(void) {
	float bright = 0.5f;
	if(Options::instance) bright = Options::instance->brightness;
	for(int32_t i = 0; i != 16; ++i) {
		float f1 = 1.0f - (float)i / 15.0f;
		float base = (1.0f - f1) / (f1 * 3.0f + 1.0f) * 0.95f + 0.05f;
		this->lightRamp[i] = base + (1.0f - base) * bright * 0.85f;
	}
}
void Dimension::init() {
	this->biomeSourcePtr = new BiomeSource(this->levelPtr);
}
