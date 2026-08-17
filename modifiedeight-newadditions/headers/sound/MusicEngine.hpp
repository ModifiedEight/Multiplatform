#pragma once
#include <_types.h>
#include <sound/MusicData.hpp>
#include <sound/helix/aacdec.h>
#include <vector>

#if (defined(__linux__) && !defined(ANDROID)) || defined(MCPE_IOS) || defined(__APPLE__)
#if defined(MCPE_IOS) || defined(__APPLE__)
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif
#define HAS_OPENAL 1
#else
typedef unsigned int ALuint;
#define HAS_OPENAL 0
#endif

struct Minecraft;

enum MusicMode {
	MUSIC_MODE_NONE = 0,
	MUSIC_MODE_MENU = 1,
	MUSIC_MODE_GAME = 2
};

struct MusicEngine {
	static MusicEngine* instance;

	ALuint musicSource;
	ALuint musicBuffers[4];
	bool isPlaying;
	bool isDecoderOpen;
	bool isInitialized;
	int32_t currentMode;
	int32_t lastPlayedMenuIndex;
	int32_t lastPlayedGameIndex;
	float pauseSeconds;
	double lastUpdateTime;
	float currentVolume;

	HAACDecoder aacDec;
	AACFrameInfo aacInfo;
	const unsigned char* streamData;
	int streamBytesLeft;
	std::vector<short> pcmBuffer;

	MusicEngine();
	~MusicEngine();

	void init();
	void destroy();
	void stop();
	void setVolume(float volume);
	bool_t isTrackPlaying();
	void playTrack(const MusicTrack& track);
	void startRandomTrack(int32_t mode);
	void streamBuffers();
	void update(Minecraft* mc);
};
