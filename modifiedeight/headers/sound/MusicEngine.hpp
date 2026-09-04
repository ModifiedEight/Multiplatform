#pragma once
#include <_types.h>
#include <sound/MusicData.hpp>
#include <sound/helix/aacdec.h>
#include <vector>

#if defined(_WIN32) || defined(WIN32)
#define HAS_OPENAL 0
#define HAS_WINMM 1
#define HAS_OPENSLES 0
#include <winsock2.h>
#include <windows.h>
#include <mmsystem.h>
typedef unsigned int ALuint;

#elif defined(ANDROID)
#define HAS_OPENAL 0
#define HAS_WINMM 0
#define HAS_OPENSLES 1
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
typedef unsigned int ALuint;

#elif defined(__linux__) || defined(MCPE_IOS) || defined(__APPLE__)
#define HAS_OPENAL 1
#define HAS_WINMM 0
#define HAS_OPENSLES 0
#if defined(MCPE_IOS) || defined(__APPLE__)
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

#else
#define HAS_OPENAL 0
#define HAS_WINMM 0
#define HAS_OPENSLES 0
typedef unsigned int ALuint;
#endif

struct Minecraft;

enum MusicMode {
	MUSIC_MODE_NONE = 0,
	MUSIC_MODE_MENU = 1,
	MUSIC_MODE_GAME = 2
};

struct MusicEngine {
	static MusicEngine* instance;

#if HAS_OPENAL
	ALuint musicSource;
	ALuint musicBuffers[4];
#elif HAS_WINMM
	HWAVEOUT hWaveOut;
	WAVEHDR waveHeaders[4];
	std::vector<short> winPcmBuffers[4];
	bool winBufferInUse[4];
#elif HAS_OPENSLES
	SLObjectItf slPlayerObj;
	SLPlayItf slPlayItf;
	SLVolumeItf slVolumeItf;
	SLAndroidSimpleBufferQueueItf slBufferQueueItf;
	SLObjectItf slOutputMix;
	std::vector<short> slPcmBuffers[4];
	int slBufferIndex;
#endif

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

