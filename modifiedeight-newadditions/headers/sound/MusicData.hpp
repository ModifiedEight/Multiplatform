#pragma once
#include <_types.h>
#include <stddef.h>
struct MusicTrack {
	const char* name;
	const unsigned char* data;
	size_t size;
};
extern "C" {
	extern const unsigned char music_danny_start[];
	extern const unsigned char music_danny_end[];
	extern const unsigned char music_door_start[];
	extern const unsigned char music_door_end[];
	extern const unsigned char music_haggstrom_start[];
	extern const unsigned char music_haggstrom_end[];
	extern const unsigned char music_key_start[];
	extern const unsigned char music_key_end[];
	extern const unsigned char music_living_mice_start[];
	extern const unsigned char music_living_mice_end[];
	extern const unsigned char music_mice_on_venus_start[];
	extern const unsigned char music_mice_on_venus_end[];
	extern const unsigned char music_minecraft_start[];
	extern const unsigned char music_minecraft_end[];
	extern const unsigned char music_moog_city_2_start[];
	extern const unsigned char music_moog_city_2_end[];
	extern const unsigned char music_oxygene_start[];
	extern const unsigned char music_oxygene_end[];
	extern const unsigned char music_subwoofer_lullaby_start[];
	extern const unsigned char music_subwoofer_lullaby_end[];
	extern const unsigned char music_equinoxe_start[];
	extern const unsigned char music_equinoxe_end[];
}
const MusicTrack& getMenuTrack(int index);
int getMenuTrackCount();
const MusicTrack& getGameTrack(int index);
int getGameTrackCount();
