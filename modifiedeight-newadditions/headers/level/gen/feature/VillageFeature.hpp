#pragma once

#include <level/gen/feature/Feature.hpp>
#include <level/biome/Biome.hpp>

enum class VillageType {
	PLAINS,
	DESERT,
	TAIGA,
	SNOWY
};

struct VillageMaterials {
	int32_t woodId;
	int32_t woodMeta;
	int32_t plankId;
	int32_t plankMeta;
	int32_t stoneId;
	int32_t stoneMeta;
	int32_t stairId;
	int32_t slabId;
	int32_t slabMeta;
	int32_t fenceId;
	int32_t pathId;
	int32_t pathMeta;
	bool_t hasSnow;
};

class VillageFeature : public Feature {
public:
	VillageFeature(bool_t update = 0);
	virtual ~VillageFeature();

	virtual bool_t place(Level* level, Random* random, int32_t x, int32_t y, int32_t z);

	static VillageMaterials getMaterials(VillageType type);
	static VillageType getVillageType(Biome* biome);

private:
	void placeWell(Level* level, Random* random, int32_t x, int32_t y, int32_t z, const VillageMaterials& mat);
	void placeSmallHouse(Level* level, Random* random, int32_t x, int32_t y, int32_t z, int32_t rotation, int32_t variant, const VillageMaterials& mat);
	void placeLargeHouse(Level* level, Random* random, int32_t x, int32_t y, int32_t z, int32_t rotation, const VillageMaterials& mat);
	void placeFarm(Level* level, Random* random, int32_t x, int32_t y, int32_t z, int32_t rotation, const VillageMaterials& mat);
	void placeRoad(Level* level, int32_t x1, int32_t z1, int32_t x2, int32_t z2, const VillageMaterials& mat);
	void placeLamp(Level* level, int32_t x, int32_t y, int32_t z, const VillageMaterials& mat);

	void fillFoundation(Level* level, int32_t minX, int32_t maxX, int32_t minZ, int32_t maxZ, int32_t baseY, const VillageMaterials& mat);
	void placeRotated(Level* level, int32_t originX, int32_t originY, int32_t originZ, int32_t lx, int32_t ly, int32_t lz, int32_t sizeX, int32_t sizeZ, int32_t blockId, int32_t data, int32_t rotation);
};
