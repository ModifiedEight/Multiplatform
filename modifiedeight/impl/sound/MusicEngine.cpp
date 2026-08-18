#include <sound/MusicEngine.hpp>
#include <Minecraft.hpp>
#include <gui/Screen.hpp>
#include <stdlib.h>
#include <sys/time.h>

static double getMusicTime() {
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	return (double)tv.tv_sec + (double)tv.tv_usec * 1e-6;
}

MusicEngine* MusicEngine::instance = nullptr;

MusicEngine::MusicEngine()
	: musicSource(0)
	, isPlaying(false)
	, isDecoderOpen(false)
	, isInitialized(false)
	, currentMode(MUSIC_MODE_NONE)
	, lastPlayedMenuIndex(-1)
	, lastPlayedGameIndex(-1)
	, pauseSeconds(0.0f)
	, lastUpdateTime(0.0)
	, currentVolume(1.0f)
	, aacDec(nullptr)
	, streamData(nullptr)
	, streamBytesLeft(0)
	, pcmBuffer(8192) {
	for (int i = 0; i < 4; ++i) {
		this->musicBuffers[i] = 0;
	}
	MusicEngine::instance = this;
}

MusicEngine::~MusicEngine() {
	this->destroy();
	if (MusicEngine::instance == this) {
		MusicEngine::instance = nullptr;
	}
}

void MusicEngine::init() {
#if HAS_OPENAL
	if (this->isInitialized) return;
	ALCcontext* ctx = alcGetCurrentContext();
	if (!ctx) return;
	alGenSources(1, &this->musicSource);
	alGenBuffers(4, this->musicBuffers);
	this->isInitialized = true;
#endif
}

void MusicEngine::destroy() {
#if HAS_OPENAL
	this->stop();
	if (this->isInitialized) {
		if (this->musicSource != 0) {
			alDeleteSources(1, &this->musicSource);
			this->musicSource = 0;
		}
		alDeleteBuffers(4, this->musicBuffers);
		for (int i = 0; i < 4; ++i) {
			this->musicBuffers[i] = 0;
		}
		this->isInitialized = false;
	}
#endif
}

void MusicEngine::stop() {
#if HAS_OPENAL
	if (!this->isInitialized) return;
	if (this->musicSource != 0) {
		alSourceStop(this->musicSource);
		alSourcei(this->musicSource, AL_BUFFER, 0);
	}
	if (this->isDecoderOpen) {
		if (this->aacDec) {
			AACFreeDecoder(this->aacDec);
			this->aacDec = nullptr;
		}
		this->isDecoderOpen = false;
	}
	this->streamData = nullptr;
	this->streamBytesLeft = 0;
	this->isPlaying = false;
#endif
}

void MusicEngine::setVolume(float volume) {
	this->currentVolume = (volume < 0.0f) ? 0.0f : ((volume > 1.0f) ? 1.0f : volume);
#if HAS_OPENAL
	if (this->isInitialized && this->musicSource != 0) {
		alSourcef(this->musicSource, AL_GAIN, this->currentVolume);
	}
#endif
}

bool_t MusicEngine::isTrackPlaying() {
#if HAS_OPENAL
	if (!this->isPlaying || !this->isInitialized || this->musicSource == 0) {
		return false;
	}
	ALint state = 0;
	alGetSourcei(this->musicSource, AL_SOURCE_STATE, &state);
	return (state == AL_PLAYING);
#else
	return false;
#endif
}

void MusicEngine::playTrack(const MusicTrack& track) {
#if HAS_OPENAL
	if (!this->isInitialized) {
		this->init();
	}
	if (!this->isInitialized || this->musicSource == 0) {
		return;
	}
	this->stop();
	if (!track.data || track.size == 0) {
		return;
	}

	this->aacDec = AACInitDecoder();
	if (!this->aacDec) {
		return;
	}

	this->streamData = track.data;
	this->streamBytesLeft = (int)track.size;
	this->isDecoderOpen = true;

	alSourcei(this->musicSource, AL_SOURCE_RELATIVE, AL_TRUE);
	alSource3f(this->musicSource, AL_POSITION, 0.0f, 0.0f, 0.0f);
	alSource3f(this->musicSource, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
	alSourcef(this->musicSource, AL_ROLLOFF_FACTOR, 0.0f);
	alSourcef(this->musicSource, AL_GAIN, this->currentVolume);
	alSourcei(this->musicSource, AL_LOOPING, AL_FALSE);

	int queuedBuffers = 0;
	for (int i = 0; i < 4; ++i) {
		if (this->streamBytesLeft <= 0) break;
		int syncOffset = AACFindSyncWord((unsigned char*)this->streamData, this->streamBytesLeft);
		if (syncOffset < 0) break;
		this->streamData += syncOffset;
		this->streamBytesLeft -= syncOffset;

		unsigned char* readPtr = (unsigned char*)this->streamData;
		int beforeBytes = this->streamBytesLeft;
		int err = AACDecode(this->aacDec, &readPtr, &this->streamBytesLeft, this->pcmBuffer.data());
		if (err == 0) {
			AACGetLastFrameInfo(this->aacDec, &this->aacInfo);
			this->streamData = readPtr;
			ALenum format = (this->aacInfo.nChans == 2) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
			ALsizei freq = this->aacInfo.sampRateCore;
			alBufferData(this->musicBuffers[i], format, this->pcmBuffer.data(), this->aacInfo.outputSamps * sizeof(short), freq);
			alSourceQueueBuffers(this->musicSource, 1, &this->musicBuffers[i]);
			queuedBuffers++;
		} else {
			if (this->streamBytesLeft == beforeBytes) {
				this->streamData++;
				this->streamBytesLeft--;
			}
		}
	}

	if (queuedBuffers > 0) {
		alSourcePlay(this->musicSource);
		this->isPlaying = true;
	} else {
		this->stop();
	}
#endif
}

void MusicEngine::startRandomTrack(int32_t mode) {
	if (mode == MUSIC_MODE_MENU) {
		int count = getMenuTrackCount();
		if (count <= 0) return;
		int idx = rand() % count;
		if (count > 1 && idx == this->lastPlayedMenuIndex) {
			idx = (idx + 1) % count;
		}
		this->lastPlayedMenuIndex = idx;
		this->currentMode = MUSIC_MODE_MENU;
		this->playTrack(getMenuTrack(idx));
	} else if (mode == MUSIC_MODE_GAME) {
		int count = getGameTrackCount();
		if (count <= 0) return;
		int idx = rand() % count;
		if (count > 1 && idx == this->lastPlayedGameIndex) {
			idx = (idx + 1) % count;
		}
		this->lastPlayedGameIndex = idx;
		this->currentMode = MUSIC_MODE_GAME;
		this->playTrack(getGameTrack(idx));
	}
}

void MusicEngine::streamBuffers() {
#if HAS_OPENAL
	if (!this->isPlaying || !this->isDecoderOpen || !this->isInitialized || this->musicSource == 0) {
		return;
	}
	ALint processed = 0;
	alGetSourcei(this->musicSource, AL_BUFFERS_PROCESSED, &processed);
	while (processed > 0) {
		ALuint buffer = 0;
		alSourceUnqueueBuffers(this->musicSource, 1, &buffer);
		if (this->streamBytesLeft > 0) {
			int syncOffset = AACFindSyncWord((unsigned char*)this->streamData, this->streamBytesLeft);
			if (syncOffset >= 0) {
				this->streamData += syncOffset;
				this->streamBytesLeft -= syncOffset;
				unsigned char* readPtr = (unsigned char*)this->streamData;
				int beforeBytes = this->streamBytesLeft;
				int err = AACDecode(this->aacDec, &readPtr, &this->streamBytesLeft, this->pcmBuffer.data());
				if (err == 0) {
					AACGetLastFrameInfo(this->aacDec, &this->aacInfo);
					this->streamData = readPtr;
					ALenum format = (this->aacInfo.nChans == 2) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
					ALsizei freq = this->aacInfo.sampRateCore;
					alBufferData(buffer, format, this->pcmBuffer.data(), this->aacInfo.outputSamps * sizeof(short), freq);
					alSourceQueueBuffers(this->musicSource, 1, &buffer);
				} else {
					if (this->streamBytesLeft == beforeBytes) {
						this->streamData++;
						this->streamBytesLeft--;
					}
				}
			}
		}
		processed--;
	}

	ALint state = 0;
	alGetSourcei(this->musicSource, AL_SOURCE_STATE, &state);
	if (state != AL_PLAYING && this->isPlaying) {
		ALint queued = 0;
		alGetSourcei(this->musicSource, AL_BUFFERS_QUEUED, &queued);
		if (queued > 0) {
			alSourcePlay(this->musicSource);
		} else {
			this->stop();
		}
	}
#endif
}

void MusicEngine::update(Minecraft* mc) {
	if (!mc) return;
	double now = getMusicTime();
	if (this->lastUpdateTime == 0.0) {
		this->lastUpdateTime = now;
		return;
	}
	float dt = (float)(now - this->lastUpdateTime);
	this->lastUpdateTime = now;
	if (dt < 0.0f || dt > 1.0f) dt = 0.05f;

	float musicOpt = mc->options.musicVolume;
	this->setVolume(musicOpt);
	if (musicOpt <= 0.001f) {
		if (this->isPlaying) {
			this->stop();
		}
		return;
	}

	if (!this->isInitialized) {
		this->init();
	}

	bool inGame = (mc->level != nullptr);
	int32_t neededMode = inGame ? MUSIC_MODE_GAME : MUSIC_MODE_MENU;

	if (this->isPlaying && this->currentMode != neededMode) {
		this->stop();
	}

	if (this->isPlaying) {
		this->streamBuffers();
	} else {
		this->pauseSeconds -= dt;
		if (this->pauseSeconds <= 0.0f) {
			this->startRandomTrack(neededMode);
			this->pauseSeconds = 20.0f + (float)(rand() % 40);
		}
	}
}
