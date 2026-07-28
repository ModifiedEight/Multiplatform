#include <level/gen/feature/LargeFeature.hpp>
#include <level/Level.hpp>

LargeFeature::~LargeFeature(){

}

LargeFeature::LargeFeature(void) {
	this->range = 8;
}

void LargeFeature::apply(ChunkSource* source, Level* level, int32_t cx, int32_t cz, uint8_t* blocks, int32_t a7) {
	int32_t range = this->range;
	uint64_t levelSeed = (uint64_t)level->getSeed();

	for (int32_t x = cx - range; x <= cx + range; ++x) {
		for (int32_t z = cz - range; z <= cz + range; ++z) {
			uint64_t chunkSeed = ((uint64_t)x * 341873128712ULL + (uint64_t)z * 132897987541ULL) ^ levelSeed;
			this->random.setSeed(chunkSeed);
			this->addFeature(level, x, z, cx, cz, blocks, a7);
		}
	}
}
