#pragma once
#include <string>
#include <vector>
#include <cstdint>

struct Minecraft;

struct CustomMusicTrack {
	std::string name;
	std::string file;
	const unsigned char* data;
	size_t size;
};

enum PlaybackMode {
	PLAYBACK_SEQUENTIAL = 0,
	PLAYBACK_SHUFFLE = 1,
	PLAYBACK_REPEAT_ONE = 2,
	PLAYBACK_SINGLE = 3
};

struct PlayingJukebox {
	int32_t trackIndex;
	int32_t blockX, blockY, blockZ;
	float volumeFade;
	bool isFadingOut;
	int32_t nextTrackIndex;
};

struct MusicPlayerManager {
	static MusicPlayerManager instance;

	std::vector<CustomMusicTrack> tracks;
	std::vector<PlayingJukebox> activePlayers;
	bool isPlaying;
	int32_t currentTrackIndex;
	int32_t blockX, blockY, blockZ;
	float volumeFade;
	bool isFadingOut;
	int32_t nextTrackIndex;
	int32_t nextX, nextY, nextZ;
	int32_t playbackMode;
	std::vector<uint8_t> loadedFileBytes;

	MusicPlayerManager();
	void loadTracks();
	void playTrack(int32_t trackIndex, int32_t x, int32_t y, int32_t z);
	void stop();
	void stopAt(int32_t x, int32_t y, int32_t z);
	void stopImmediate();
	void onTrackFinished();
	void playNextTrack();
	void playPrevTrack();
	void cyclePlaybackMode();
	std::string getPlaybackModeName() const;
	void update(Minecraft* mc);
	bool isPlayingAt(int32_t x, int32_t y, int32_t z) const;
	std::string getCurrentTrackName() const;
	std::string getTrackNameAt(int32_t x, int32_t y, int32_t z) const;
};
