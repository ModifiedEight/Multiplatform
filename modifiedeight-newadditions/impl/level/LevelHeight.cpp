#include <level/LevelHeight.hpp>

int32_t LevelHeight::height = MCPE_HEIGHT;
int32_t LevelHeight::xShift = MCPE_X_SHIFT;
int32_t LevelHeight::zShift = MCPE_Z_SHIFT;
int32_t LevelHeight::tiles = MCPE_TILES;

/*
 * Call this immediately before building a Level and never while one is alive:
 * every chunk of a level is allocated and indexed for the height that was set
 * when the level was created.
 */
void LevelHeight::set(int32_t h) {
	if(h > MCPE_HEIGHT) {
		LevelHeight::height = JAVA_HEIGHT;
		LevelHeight::xShift = JAVA_X_SHIFT;
		LevelHeight::zShift = JAVA_Z_SHIFT;
		LevelHeight::tiles = JAVA_TILES;
	} else {
		LevelHeight::height = MCPE_HEIGHT;
		LevelHeight::xShift = MCPE_X_SHIFT;
		LevelHeight::zShift = MCPE_Z_SHIFT;
		LevelHeight::tiles = MCPE_TILES;
	}
}
