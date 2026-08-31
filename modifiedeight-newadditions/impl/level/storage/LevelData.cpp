#include <level/storage/LevelData.hpp>
#include <BitStream.h>
#include <entity/Player.hpp>
#include <level/LevelSettings.hpp>
#include <nbt/CompoundTag.hpp>
#include <utils.h>

LevelData::LevelData(struct CompoundTag* a2) {
	this->sizeOnDisk = 0; //TODO doesnt seem to exist in mcpe
	this->playerTag = 0;
	this->getTagData(a2);
}
LevelData::LevelData(const LevelData& a2) {
	this->sizeOnDisk = 0; //TODO doesnt seem to exist in mcpe
	this->playerData = a2.playerData;
	this->field_2C = a2.field_2C;
	this->field_30 = a2.field_30;
	this->field_34 = a2.field_34;
	this->field_38 = a2.field_38;
	this->field_3C = a2.field_3C;
	this->field_40 = a2.field_40;
	this->field_44 = a2.field_44;
	this->field_48 = a2.field_48;
	this->field_4C = a2.field_4C;
	this->field_50 = a2.field_50;
	this->levelName = a2.levelName;
	this->seed = a2.seed;
	this->xSpawn = a2.xSpawn;
	this->ySpawn = a2.ySpawn;
	this->zSpawn = a2.zSpawn;
	this->time = a2.time;
	this->lastPlayed = a2.lastPlayed;
	this->playerTag = 0;
	this->dimensionID = a2.dimensionID;
	this->gameType = a2.gameType;
	this->storageVersion = a2.storageVersion;
	this->spawnMobs = a2.spawnMobs;
	this->generatorVersion = a2.generatorVersion;
	this->setPlayerTag(a2.playerTag);
}
LevelData::LevelData(const struct LevelSettings& settings, const std::string& a3, int32_t a4) {
	this->sizeOnDisk = 0; //TODO doesnt seem to exist in mcpe
	int32_t v9;	 // r3
	int32_t v10; // r3

	this->field_50 = -1;
	this->levelName = a3;
	this->ySpawn = 64;
	this->seed = settings.seed;
	this->xSpawn = 128;
	this->zSpawn = 128;
	this->time = 0;
	this->lastPlayed = 0;
	this->playerTag = 0;
	this->dimensionID = 0;
	// Use generatorType from LevelSettings if provided (non-zero), otherwise fall back to a4
	if(settings.generatorType != 0) {
		this->generatorVersion = settings.generatorType;
	} else {
		this->generatorVersion = a4 & ~(a4 >> 31);
	}
	this->gameType = settings.gameType;
	this->generateCaves = settings.generateCaves;
	this->spawnMonsters = settings.spawnMonsters;
	this->spawnAnimals = settings.spawnAnimals;
	this->spawnMobs = settings.spawnMonsters || settings.spawnAnimals;
	if(settings.timeFreeze) {
		this->stopTime = 5000;
	} else {
		this->stopTime = -1;
	}
	this->field_2C = 0;
	this->field_30 = 24000 + (rand() % 48000);
	this->field_34 = 0;
}
LevelData::LevelData(void) {
	this->sizeOnDisk = 0; //TODO doesnt seem to exist in mcpe
	this->playerData.x = 0.0;
	this->playerData.y = 0.0;
	this->playerData.z = 0.0;
	this->playerData.motX = 0.0;
	this->playerData.motY = 0.0;
	this->playerData.motZ = 0.0;
	this->field_50 = -1;
	this->ySpawn = 64;
	this->xSpawn = 128;
	this->levelName = "";
	this->zSpawn = 128;
	this->seed = 0;
	this->time = 0;
	this->lastPlayed = 0;
	this->playerTag = 0;
	this->dimensionID = 0;
	this->storageVersion = 0;
	this->generatorVersion = 0;
	this->generateCaves = 1;
	this->spawnMonsters = 1;
	this->spawnAnimals = 1;
	this->spawnMobs = 1;
	this->gameType = 1;
	this->stopTime = -1;
}
struct CompoundTag* LevelData::createTag(const std::vector<struct Player*>& a2) {
	CompoundTag* v4 = new CompoundTag();
	CompoundTag* v7 = 0;
	if(!a2.empty() && a2[0]) {
		v7 = new CompoundTag();
		a2[0]->saveWithoutId(v7);
	} else if (this->playerTag) {
		v7 = (CompoundTag*)this->playerTag->copy();
	}
	this->setTagData(v4, v7);
	return v4;
}

struct CompoundTag* LevelData::createTag() {
	CompoundTag* v2 = new CompoundTag();
	if(this->playerTag) this->setTagData(v2, (CompoundTag*)this->playerTag->copy());
	else this->setTagData(v2, 0);

	return v2;
}

int32_t LevelData::getDimension() {
	return this->dimensionID;
}
int32_t LevelData::getGameType() {
	return this->gameType;
}
int32_t LevelData::getGeneratorVersion() {
	return this->generatorVersion;
}
int32_t LevelData::getLastPlayed() {
	return this->lastPlayed;
}
std::string LevelData::getLevelName() {
	return this->levelName;
}
struct CompoundTag* LevelData::getLoadedPlayerTag() {
	return this->playerTag;
}

int32_t LevelData::getSeed() {
	return this->seed;
}
int32_t LevelData::getSizeOnDisk() {
	return this->sizeOnDisk;
}
bool_t LevelData::getSpawnMobs() {
	return this->spawnMobs;
}
int32_t LevelData::getStorageVersion() {
	return this->storageVersion;
}
void LevelData::getTagData(const CompoundTag* a2) {
	if(a2) {
		this->seed = a2->getLong("RandomSeed");
		this->gameType = a2->getInt("GameType");
		this->xSpawn = a2->getInt("SpawnX");
		this->ySpawn = a2->getInt("SpawnY");
		this->zSpawn = a2->getInt("SpawnZ");
		this->time = a2->getLong("Time");
		this->lastPlayed = a2->getLong("LastPlayed");
		this->sizeOnDisk = a2->getLong("SizeOnDisk");
		this->levelName = a2->getString("LevelName");
		this->storageVersion = a2->getInt("StorageVersion");
		this->generatorVersion = a2->getInt("generatorVersion");
		if(a2->contains("dayCycleStopTime")) {
			this->stopTime = a2->getLong("dayCycleStopTime");
		} else {
			this->stopTime = -1;
		}
		if(a2->contains("spawnMobs", 1)) {
			this->spawnMobs = a2->getByte("spawnMobs") != 0;
		} else {
			this->spawnMobs = 1;
		}
		if(a2->contains("generateCaves", 1)) {
			this->generateCaves = a2->getByte("generateCaves") != 0;
		} else {
			this->generateCaves = 1;
		}
		if(a2->contains("spawnMonsters", 1)) {
			this->spawnMonsters = a2->getByte("spawnMonsters") != 0;
		} else {
			this->spawnMonsters = 1;
		}
		if(a2->contains("spawnAnimals", 1)) {
			this->spawnAnimals = a2->getByte("spawnAnimals") != 0;
		} else {
			this->spawnAnimals = 1;
		}
		if(a2->contains("weatherType")) {
			this->field_2C = a2->getInt("weatherType");
		} else {
			this->field_2C = 0;
		}
		if(a2->contains("weatherTicks")) {
			this->field_30 = a2->getInt("weatherTicks");
		} else {
			this->field_30 = 12000;
		}
		if(a2->contains("rainLevel")) {
			this->field_34 = a2->getInt("rainLevel");
		} else {
			this->field_34 = 0;
		}
FINALS:
		if(a2->contains("Player", 10)) {
			this->setPlayerTag(a2->getCompound("Player"));
		}
	}
}
int32_t LevelData::getTime() {
	return this->time;
}
int32_t LevelData::getXSpawn() {
	return this->xSpawn;
}
int32_t LevelData::getYSpawn() {
	return this->ySpawn;
}
int32_t LevelData::getZSpawn() {
	return this->zSpawn;
}
LevelData& LevelData::operator=(const LevelData& a2) {
	if(this != &a2) {
		this->seed = a2.seed;
		this->gameType = a2.gameType;
		this->levelName = a2.levelName;
		this->xSpawn = a2.xSpawn;
		this->ySpawn = a2.ySpawn;
		this->zSpawn = a2.zSpawn;
		this->lastPlayed = a2.lastPlayed;
		this->time = a2.time;
		this->dimensionID = a2.dimensionID;
		this->spawnMobs = a2.spawnMobs;
		this->generateCaves = a2.generateCaves;
		this->spawnMonsters = a2.spawnMonsters;
		this->spawnAnimals = a2.spawnAnimals;
		this->playerData = a2.playerData;
		this->field_2C = a2.field_2C;
		this->field_30 = a2.field_30;
		this->field_34 = a2.field_34;
		this->field_38 = a2.field_38;
		this->field_3C = a2.field_3C;
		this->field_40 = a2.field_40;
		this->field_44 = a2.field_44;
		this->field_48 = a2.field_48;
		this->field_4C = a2.field_4C;
		this->stopTime = a2.stopTime;
		this->field_50 = a2.field_50;
		this->generatorVersion = a2.generatorVersion;
		this->storageVersion = a2.storageVersion;
		this->setPlayerTag(a2.playerTag);
	}
	return *this;
}
void LevelData::setDimension(int32_t a2) {
	this->dimensionID = a2;
}
void LevelData::setGameType(int32_t a2) {
	this->gameType = a2;
}
void LevelData::setGeneratorVersion(int32_t a2) {
	this->generatorVersion = a2;
}
void LevelData::setLevelName(const std::string& a2) {
	this->levelName = a2;
}
void LevelData::setLoadedPlayerTag(struct CompoundTag* a2) {
	if(this->playerTag) {
		this->playerTag->deleteChildren();
		if(this->playerTag) delete this->playerTag;
		this->playerTag = 0;
	}
	this->playerTag = a2;
}
void LevelData::setLoadedPlayerTo(struct Player* a2) {
	if(this->field_50 == 1) {
		this->playerData.loadPlayer(a2);
	}
}
void LevelData::setPlayerTag(struct CompoundTag* a2) {
	if(this->playerTag) {
		this->playerTag->deleteChildren();
		if(this->playerTag) delete this->playerTag;
		this->playerTag = 0;
	}
	if(a2) {
		this->playerTag = (CompoundTag*)a2->copy();
	}
}
void LevelData::setSeed(int32_t a2) { //long
	this->seed = a2;
}
void LevelData::setSizeOnDisk(int32_t a2) { //long
	this->sizeOnDisk = a2;
}
void LevelData::setSpawn(int32_t x, int32_t y, int32_t z) {
	this->xSpawn = x;
	this->ySpawn = y;
	this->zSpawn = z;
}
void LevelData::setSpawnMobs(bool_t a2) {
	this->spawnMobs = a2;
}
void LevelData::setStopTime(int32_t a2) { //long
	this->stopTime = a2;
}
void LevelData::setStorageVersion(int32_t a2) {
	this->storageVersion = a2;
}
void LevelData::setTagData(struct CompoundTag* a2, struct CompoundTag* a3) {
	a2->putLong("RandomSeed", this->seed);
	a2->putInt("GameType", this->gameType);
	a2->putInt("SpawnX", this->xSpawn);
	a2->putInt("SpawnY", this->ySpawn);
	a2->putInt("SpawnZ", this->zSpawn);
	a2->putLong("Time", this->time);
	a2->putLong("SizeOnDisk", this->sizeOnDisk);
	a2->putLong("LastPlayed", getEpochTimeS());
	a2->putString("LevelName", this->levelName);
	a2->putInt("StorageVersion", this->storageVersion);
	a2->putInt("generatorVersion", this->generatorVersion);
	a2->putInt("Platform", 2);
	a2->putInt("dayCycleStopTime", this->stopTime);
	a2->putBoolean("spawnMobs", this->spawnMobs);
	a2->putBoolean("generateCaves", this->generateCaves);
	a2->putBoolean("spawnMonsters", this->spawnMonsters);
	a2->putBoolean("spawnAnimals", this->spawnAnimals);
	a2->putInt("weatherType", this->field_2C);
	a2->putInt("weatherTicks", this->field_30);
	a2->putInt("rainLevel", this->field_34);
	if(a3) {
		a2->put("Player", a3);
	}
}
void LevelData::setTime(int32_t a2) { //long
	this->time = a2;
}
void LevelData::setXSpawn(int32_t x) {
	this->xSpawn = x;
}
void LevelData::setYSpawn(int32_t y) {
	this->ySpawn = y;
}
void LevelData::setZSpawn(int32_t z) {
	this->zSpawn = z;
}
void LevelData::v1_read(RakNet::BitStream& a2, int32_t a3) {
	this->storageVersion = a3;
	a2.Read<int32_t>(this->seed); //long
	a2.Read<int32_t>(this->xSpawn);
	a2.Read<int32_t>(this->ySpawn);
	a2.Read<int32_t>(this->zSpawn);
	a2.Read<int32_t>(this->time);		//long
	a2.Read<int32_t>(this->sizeOnDisk); //long
	a2.Read<int32_t>(this->lastPlayed);
	{
		RakNet::RakString s;
		a2.Read<RakNet::RakString>(s);
		this->levelName = s.C_String();
	}
}
void LevelData::v1_write(RakNet::BitStream& a2) {
	a2.Write<int32_t>(this->seed); //long
	a2.Write<int32_t>(this->xSpawn);
	a2.Write<int32_t>(this->ySpawn);
	a2.Write<int32_t>(this->zSpawn);
	a2.Write<int32_t>(this->time);		 //long
	a2.Write<int32_t>(this->sizeOnDisk); //long
	a2.Write<int32_t>(getEpochTimeS());
	a2.Write(this->levelName.c_str());
}
LevelData::~LevelData() {
	this->setPlayerTag(0);
}
