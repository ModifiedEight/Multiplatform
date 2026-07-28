#include <_types.h>

struct LevelSettings
{
	int32_t seed, gameType;
	int32_t generatorType; // 0 = Old (finite 256x256), 1 = Infinite
};
