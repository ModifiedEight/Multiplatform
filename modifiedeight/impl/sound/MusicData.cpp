#include <sound/MusicData.hpp>
static const MusicTrack s_menuTracks[] = {
	{"Moog City 2", music_moog_city_2_start, (size_t)(music_moog_city_2_end - music_moog_city_2_start)},
	{"Door", music_door_start, (size_t)(music_door_end - music_door_start)},
	{"Equinoxe", music_equinoxe_start, (size_t)(music_equinoxe_end - music_equinoxe_start)},
	{"Key", music_key_start, (size_t)(music_key_end - music_key_start)},
	{"Oxygene", music_oxygene_start, (size_t)(music_oxygene_end - music_oxygene_start)}
};
static const MusicTrack s_gameTracks[] = {
	{"Minecraft", music_minecraft_start, (size_t)(music_minecraft_end - music_minecraft_start)},
	{"Subwoofer Lullaby", music_subwoofer_lullaby_start, (size_t)(music_subwoofer_lullaby_end - music_subwoofer_lullaby_start)},
	{"Living Mice", music_living_mice_start, (size_t)(music_living_mice_end - music_living_mice_start)},
	{"Haggstrom", music_haggstrom_start, (size_t)(music_haggstrom_end - music_haggstrom_start)},
	{"Danny", music_danny_start, (size_t)(music_danny_end - music_danny_start)},
	{"Mice on Venus", music_mice_on_venus_start, (size_t)(music_mice_on_venus_end - music_mice_on_venus_start)},
	{"Key", music_key_start, (size_t)(music_key_end - music_key_start)},
	{"Oxygene", music_oxygene_start, (size_t)(music_oxygene_end - music_oxygene_start)},
	{"Equinoxe", music_equinoxe_start, (size_t)(music_equinoxe_end - music_equinoxe_start)}
};
const MusicTrack& getMenuTrack(int index) {
	if (index < 0 || index >= (int)(sizeof(s_menuTracks) / sizeof(s_menuTracks[0]))) {
		return s_menuTracks[0];
	}
	return s_menuTracks[index];
}
int getMenuTrackCount() {
	return (int)(sizeof(s_menuTracks) / sizeof(s_menuTracks[0]));
}
const MusicTrack& getGameTrack(int index) {
	if (index < 0 || index >= (int)(sizeof(s_gameTracks) / sizeof(s_gameTracks[0]))) {
		return s_gameTracks[0];
	}
	return s_gameTracks[index];
}
int getGameTrackCount() {
	return (int)(sizeof(s_gameTracks) / sizeof(s_gameTracks[0]));
}
