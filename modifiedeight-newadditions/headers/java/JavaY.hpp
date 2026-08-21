#pragma once
#include <_types.h>
#include <level/LevelHeight.hpp>

/*
 * JavaY - the single place that knows how a Java Y relates to an m8 Y.
 *
 * Minecraft Java Edition 1.8 worlds are 256 blocks tall and m8's are 128, so a
 * Java session asks for a 256 tall level (LevelHeight::set(JAVA_HEIGHT) before
 * the level is built) and the two agree block for block: BASE is 0, so toM8 and
 * toJava are the identity and nothing is clipped.
 *
 * The translation is kept anyway rather than deleted, for two reasons.  It is
 * the thing to change if the window ever has to move (a shorter level, or the
 * top half of a world instead of the bottom), and it is the safety net: every
 * range check below asks LevelHeight what the level actually is, so if a session
 * ever ends up running against a 128 tall level the world is clipped at 128
 * instead of writing past the end of a column buffer.
 */
struct JavaY
{
	// The Java Y that lands on m8 Y 0.
	static const int32_t BASE = 0;
	// What a Java server believes the world height to be, always 256.
	static const int32_t HEIGHT = JAVA_HEIGHT;

	static inline int32_t toM8(int32_t javaY) { return javaY - BASE; }
	static inline int32_t toJava(int32_t m8Y) { return m8Y + BASE; }
	static inline double toM8d(double javaY) { return javaY - (double)BASE; }
	static inline double toJavad(double m8Y) { return m8Y + (double)BASE; }

	// Does this Java Y have somewhere to live in the level as it stands?
	static inline bool_t inRange(int32_t javaY) { return LevelHeight::inRange(javaY - BASE); }
	// Section index (0..15) entirely outside the level, so it can be skipped wholesale.
	static inline bool_t sectionVisible(int32_t section)
	{
		int32_t lo = section * 16 - BASE;
		return lo + 15 >= 0 && lo < LevelHeight::height;
	}
	static inline int32_t clampM8(int32_t m8Y)
	{
		if(m8Y < 0) return 0;
		if(m8Y > LevelHeight::maxY()) return LevelHeight::maxY();
		return m8Y;
	}
};
