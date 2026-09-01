#include <sound/MusicEngine.hpp>
#include <sound/MusicPlayerManager.hpp>
#include <Minecraft.hpp>
#include <NinecraftApp.hpp>
#include <entity/Player.hpp>
#include <entity/LocalPlayer.hpp>
#include <tile/material/Material.hpp>
#include <gui/Screen.hpp>
#include <stdlib.h>
#include <string.h>

#if HAS_OPENSLES
#include <sound/SoundSystemSL.hpp>
#endif

#if defined(_WIN32) || defined(WIN32)
static double getMusicTime() {
	LARGE_INTEGER freq, cnt;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&cnt);
	return (double)cnt.QuadPart / (double)freq.QuadPart;
}
#else
#include <sys/time.h>
static double getMusicTime() {
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	return (double)tv.tv_sec + (double)tv.tv_usec * 1e-6;
}
#endif

MusicEngine* MusicEngine::instance = nullptr;

MusicEngine::MusicEngine()
#if HAS_OPENAL
	: musicSource(0)
#elif HAS_WINMM
	: hWaveOut(NULL)
#elif HAS_OPENSLES
	: slPlayerObj(nullptr)
	, slPlayItf(nullptr)
	, slVolumeItf(nullptr)
	, slBufferQueueItf(nullptr)
	, slOutputMix(nullptr)
	, slBufferIndex(0)
#endif
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
#if HAS_OPENAL
	for (int i = 0; i < 4; ++i) {
		this->musicBuffers[i] = 0;
	}
#elif HAS_WINMM
	for (int i = 0; i < 4; ++i) {
		memset(&this->waveHeaders[i], 0, sizeof(WAVEHDR));
		this->winBufferInUse[i] = false;
	}
#endif
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
#elif HAS_WINMM || HAS_OPENSLES
	this->isInitialized = true;
#endif
}

void MusicEngine::destroy() {
	this->stop();
#if HAS_OPENAL
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
#elif HAS_OPENSLES
	if (this->slOutputMix) {
		(*this->slOutputMix)->Destroy(this->slOutputMix);
		this->slOutputMix = nullptr;
	}
	this->isInitialized = false;
#elif HAS_WINMM
	this->isInitialized = false;
#endif
}

void MusicEngine::stop() {
#if HAS_OPENAL
	if (!this->isInitialized) return;
	if (this->musicSource != 0) {
		alSourceStop(this->musicSource);
		alSourcei(this->musicSource, AL_BUFFER, 0);
	}
#elif HAS_WINMM
	if (this->hWaveOut) {
		waveOutReset(this->hWaveOut);
		for (int i = 0; i < 4; ++i) {
			if (this->winBufferInUse[i]) {
				waveOutUnprepareHeader(this->hWaveOut, &this->waveHeaders[i], sizeof(WAVEHDR));
				this->winBufferInUse[i] = false;
			}
		}
		waveOutClose(this->hWaveOut);
		this->hWaveOut = NULL;
	}
#elif HAS_OPENSLES
	if (this->slPlayItf) {
		(*this->slPlayItf)->SetPlayState(this->slPlayItf, SL_PLAYSTATE_STOPPED);
		this->slPlayItf = nullptr;
	}
	if (this->slBufferQueueItf) {
		(*this->slBufferQueueItf)->Clear(this->slBufferQueueItf);
		this->slBufferQueueItf = nullptr;
	}
	this->slVolumeItf = nullptr;
	if (this->slPlayerObj) {
		(*this->slPlayerObj)->Destroy(this->slPlayerObj);
		this->slPlayerObj = nullptr;
	}
#endif
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
}

void MusicEngine::pauseStream() {
#if HAS_OPENAL
	if (this->isInitialized && this->musicSource != 0) {
		alSourcePause(this->musicSource);
	}
#elif HAS_WINMM
	if (this->hWaveOut) {
		waveOutPause(this->hWaveOut);
	}
#elif HAS_OPENSLES
	if (this->slPlayItf) {
		(*this->slPlayItf)->SetPlayState(this->slPlayItf, SL_PLAYSTATE_PAUSED);
	}
#endif
}

void MusicEngine::resumeStream() {
#if HAS_OPENAL
	if (this->isInitialized && this->musicSource != 0) {
		alSourcePlay(this->musicSource);
	}
#elif HAS_WINMM
	if (this->hWaveOut) {
		waveOutRestart(this->hWaveOut);
	}
#elif HAS_OPENSLES
	if (this->slPlayItf) {
		(*this->slPlayItf)->SetPlayState(this->slPlayItf, SL_PLAYSTATE_PLAYING);
	}
#endif
}

void MusicEngine::setVolume(float volume) {
	this->currentVolume = (volume < 0.0f) ? 0.0f : ((volume > 1.0f) ? 1.0f : volume);
#if HAS_OPENAL
	if (this->isInitialized && this->musicSource != 0) {
		alSourcef(this->musicSource, AL_GAIN, this->currentVolume);
	}
#elif HAS_WINMM
	if (this->hWaveOut) {
		DWORD vol = (DWORD)(this->currentVolume * 0xFFFF);
		DWORD lrVol = (vol & 0xFFFF) | (vol << 16);
		waveOutSetVolume(this->hWaveOut, lrVol);
	}
#elif HAS_OPENSLES
	if (this->slVolumeItf) {
		SLmillibel mb = (this->currentVolume <= 0.0001f) ? -9600 : (SLmillibel)((1.0f - this->currentVolume) * -2000.0f);
		(*this->slVolumeItf)->SetVolumeLevel(this->slVolumeItf, mb);
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
#elif HAS_WINMM
	return this->isPlaying && (this->hWaveOut != NULL);
#elif HAS_OPENSLES
	if (!this->isPlaying || !this->slPlayItf) {
		return false;
	}
	SLuint32 state = 0;
	(*this->slPlayItf)->GetPlayState(this->slPlayItf, &state);
	return (state == SL_PLAYSTATE_PLAYING);
#else
	return false;
#endif
}

void MusicEngine::playTrack(const MusicTrack& track) {
	if (!this->isInitialized) {
		this->init();
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

#if HAS_OPENAL
	if (!this->isInitialized || this->musicSource == 0) {
		this->stop();
		return;
	}

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

#elif HAS_WINMM
	int syncOffset = AACFindSyncWord((unsigned char*)this->streamData, this->streamBytesLeft);
	if (syncOffset < 0) { this->stop(); return; }
	this->streamData += syncOffset;
	this->streamBytesLeft -= syncOffset;

	unsigned char* readPtr = (unsigned char*)this->streamData;
	int err = AACDecode(this->aacDec, &readPtr, &this->streamBytesLeft, this->pcmBuffer.data());
	if (err != 0) { this->stop(); return; }
	AACGetLastFrameInfo(this->aacDec, &this->aacInfo);
	this->streamData = readPtr;

	WAVEFORMATEX wfx;
	memset(&wfx, 0, sizeof(wfx));
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = (WORD)this->aacInfo.nChans;
	wfx.nSamplesPerSec = (DWORD)this->aacInfo.sampRateCore;
	wfx.wBitsPerSample = 16;
	wfx.nBlockAlign = (wfx.nChannels * wfx.wBitsPerSample) / 8;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

	MMRESULT res = waveOutOpen(&this->hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
	if (res != MMSYSERR_NOERROR) {
		this->stop();
		return;
	}
	this->setVolume(this->currentVolume);

	int numSamps = this->aacInfo.outputSamps;
	this->winPcmBuffers[0].assign(this->pcmBuffer.begin(), this->pcmBuffer.begin() + numSamps);
	memset(&this->waveHeaders[0], 0, sizeof(WAVEHDR));
	this->waveHeaders[0].lpData = (LPSTR)this->winPcmBuffers[0].data();
	this->waveHeaders[0].dwBufferLength = numSamps * sizeof(short);
	waveOutPrepareHeader(this->hWaveOut, &this->waveHeaders[0], sizeof(WAVEHDR));
	waveOutWrite(this->hWaveOut, &this->waveHeaders[0], sizeof(WAVEHDR));
	this->winBufferInUse[0] = true;

	for (int i = 1; i < 4; ++i) {
		if (this->streamBytesLeft <= 0) break;
		syncOffset = AACFindSyncWord((unsigned char*)this->streamData, this->streamBytesLeft);
		if (syncOffset < 0) break;
		this->streamData += syncOffset;
		this->streamBytesLeft -= syncOffset;
		readPtr = (unsigned char*)this->streamData;
		int beforeBytes = this->streamBytesLeft;
		err = AACDecode(this->aacDec, &readPtr, &this->streamBytesLeft, this->pcmBuffer.data());
		if (err == 0) {
			AACGetLastFrameInfo(this->aacDec, &this->aacInfo);
			this->streamData = readPtr;
			numSamps = this->aacInfo.outputSamps;
			this->winPcmBuffers[i].assign(this->pcmBuffer.begin(), this->pcmBuffer.begin() + numSamps);
			memset(&this->waveHeaders[i], 0, sizeof(WAVEHDR));
			this->waveHeaders[i].lpData = (LPSTR)this->winPcmBuffers[i].data();
			this->waveHeaders[i].dwBufferLength = numSamps * sizeof(short);
			waveOutPrepareHeader(this->hWaveOut, &this->waveHeaders[i], sizeof(WAVEHDR));
			waveOutWrite(this->hWaveOut, &this->waveHeaders[i], sizeof(WAVEHDR));
			this->winBufferInUse[i] = true;
		} else {
			if (this->streamBytesLeft == beforeBytes) {
				this->streamData++;
				this->streamBytesLeft--;
			}
		}
	}
	this->isPlaying = true;

#elif HAS_OPENSLES
	if (!SoundSystemSL::objEngine) {
		this->stop();
		return;
	}

	int syncOffset = AACFindSyncWord((unsigned char*)this->streamData, this->streamBytesLeft);
	if (syncOffset < 0) { this->stop(); return; }
	this->streamData += syncOffset;
	this->streamBytesLeft -= syncOffset;

	unsigned char* readPtr = (unsigned char*)this->streamData;
	int err = AACDecode(this->aacDec, &readPtr, &this->streamBytesLeft, this->pcmBuffer.data());
	if (err != 0) { this->stop(); return; }
	AACGetLastFrameInfo(this->aacDec, &this->aacInfo);
	this->streamData = readPtr;

	SLEngineItf engineItf;
	(*SoundSystemSL::objEngine)->GetInterface(SoundSystemSL::objEngine, SL_IID_ENGINE, &engineItf);

	if (!this->slOutputMix) {
		const SLInterfaceID mixIds[1] = {SL_IID_ENVIRONMENTALREVERB};
		const SLboolean mixReq[1] = {SL_BOOLEAN_FALSE};
		(*engineItf)->CreateOutputMix(engineItf, &this->slOutputMix, 0, mixIds, mixReq);
		if (this->slOutputMix) {
			(*this->slOutputMix)->Realize(this->slOutputMix, SL_BOOLEAN_FALSE);
		}
	}

	SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 4};
	SLDataFormat_PCM format_pcm = {
		SL_DATAFORMAT_PCM,
		(SLuint32)this->aacInfo.nChans,
		(SLuint32)(this->aacInfo.sampRateCore * 1000),
		SL_PCMSAMPLEFORMAT_FIXED_16,
		SL_PCMSAMPLEFORMAT_FIXED_16,
		(SLuint32)(this->aacInfo.nChans == 1 ? SL_SPEAKER_FRONT_CENTER : (SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT)),
		SL_BYTEORDER_LITTLEENDIAN
	};
	SLDataSource audioSrc = {&loc_bufq, &format_pcm};
	SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, this->slOutputMix};
	SLDataSink audioSnk = {&loc_outmix, NULL};

	const SLInterfaceID ids[2] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME};
	const SLboolean req[2] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
	SLresult sres = (*engineItf)->CreateAudioPlayer(engineItf, &this->slPlayerObj, &audioSrc, &audioSnk, 2, ids, req);
	if (sres != SL_RESULT_SUCCESS || !this->slPlayerObj) {
		this->stop();
		return;
	}
	(*this->slPlayerObj)->Realize(this->slPlayerObj, SL_BOOLEAN_FALSE);
	(*this->slPlayerObj)->GetInterface(this->slPlayerObj, SL_IID_PLAY, &this->slPlayItf);
	(*this->slPlayerObj)->GetInterface(this->slPlayerObj, SL_IID_VOLUME, &this->slVolumeItf);
	(*this->slPlayerObj)->GetInterface(this->slPlayerObj, SL_IID_BUFFERQUEUE, &this->slBufferQueueItf);

	this->setVolume(this->currentVolume);

	int numSamps = this->aacInfo.outputSamps;
	this->slPcmBuffers[0].assign(this->pcmBuffer.begin(), this->pcmBuffer.begin() + numSamps);
	(*this->slBufferQueueItf)->Enqueue(this->slBufferQueueItf, this->slPcmBuffers[0].data(), numSamps * sizeof(short));
	this->slBufferIndex = 1;

	for (int i = 1; i < 4; ++i) {
		if (this->streamBytesLeft <= 0) break;
		syncOffset = AACFindSyncWord((unsigned char*)this->streamData, this->streamBytesLeft);
		if (syncOffset < 0) break;
		this->streamData += syncOffset;
		this->streamBytesLeft -= syncOffset;
		readPtr = (unsigned char*)this->streamData;
		int beforeBytes = this->streamBytesLeft;
		err = AACDecode(this->aacDec, &readPtr, &this->streamBytesLeft, this->pcmBuffer.data());
		if (err == 0) {
			AACGetLastFrameInfo(this->aacDec, &this->aacInfo);
			this->streamData = readPtr;
			numSamps = this->aacInfo.outputSamps;
			this->slPcmBuffers[i].assign(this->pcmBuffer.begin(), this->pcmBuffer.begin() + numSamps);
			(*this->slBufferQueueItf)->Enqueue(this->slBufferQueueItf, this->slPcmBuffers[i].data(), numSamps * sizeof(short));
			this->slBufferIndex = (i + 1) % 4;
		} else {
			if (this->streamBytesLeft == beforeBytes) {
				this->streamData++;
				this->streamBytesLeft--;
			}
		}
	}
	(*this->slPlayItf)->SetPlayState(this->slPlayItf, SL_PLAYSTATE_PLAYING);
	this->isPlaying = true;
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

static void applyUnderwaterFilter(short* pcm, int numSamps, int numChans) {
	if (NinecraftApp::instance && NinecraftApp::instance->player) {
		bool isUnderwater = NinecraftApp::instance->player->isUnderLiquid(Material::water);
		static float s_underwaterMix = 0.0f;
		static float s_lpLeft = 0.0f, s_lpRight = 0.0f;
		float targetMix = isUnderwater ? 1.0f : 0.0f;
		s_underwaterMix += (targetMix - s_underwaterMix) * 0.08f;
		if (s_underwaterMix > 0.005f) {
			float alpha = 0.10f + (1.0f - s_underwaterMix) * 0.40f;
			float volScale = 1.0f - s_underwaterMix * 0.35f;
			if (numChans == 2) {
				for (int s = 0; s < numSamps; s += 2) {
					float inL = (float)pcm[s];
					float inR = (float)pcm[s + 1];
					s_lpLeft += alpha * (inL - s_lpLeft);
					s_lpRight += alpha * (inR - s_lpRight);
					float outL = inL * (1.0f - s_underwaterMix) + s_lpLeft * s_underwaterMix;
					float outR = inR * (1.0f - s_underwaterMix) + s_lpRight * s_underwaterMix;
					pcm[s] = (short)(outL * volScale);
					pcm[s + 1] = (short)(outR * volScale);
				}
			} else {
				for (int s = 0; s < numSamps; ++s) {
					float in = (float)pcm[s];
					s_lpLeft += alpha * (in - s_lpLeft);
					float out = in * (1.0f - s_underwaterMix) + s_lpLeft * s_underwaterMix;
					pcm[s] = (short)(out * volScale);
				}
			}
		}
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
					applyUnderwaterFilter(this->pcmBuffer.data(), this->aacInfo.outputSamps, this->aacInfo.nChans);
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

#elif HAS_WINMM
	if (!this->isPlaying || !this->hWaveOut || !this->isDecoderOpen) {
		return;
	}
	bool anyInUse = false;
	for (int i = 0; i < 4; ++i) {
		if (this->winBufferInUse[i]) {
			if (this->waveHeaders[i].dwFlags & WHDR_DONE) {
				waveOutUnprepareHeader(this->hWaveOut, &this->waveHeaders[i], sizeof(WAVEHDR));
				this->winBufferInUse[i] = false;
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
							applyUnderwaterFilter(this->pcmBuffer.data(), this->aacInfo.outputSamps, this->aacInfo.nChans);
							int numSamps = this->aacInfo.outputSamps;
							this->winPcmBuffers[i].assign(this->pcmBuffer.begin(), this->pcmBuffer.begin() + numSamps);
							memset(&this->waveHeaders[i], 0, sizeof(WAVEHDR));
							this->waveHeaders[i].lpData = (LPSTR)this->winPcmBuffers[i].data();
							this->waveHeaders[i].dwBufferLength = numSamps * sizeof(short);
							waveOutPrepareHeader(this->hWaveOut, &this->waveHeaders[i], sizeof(WAVEHDR));
							waveOutWrite(this->hWaveOut, &this->waveHeaders[i], sizeof(WAVEHDR));
							this->winBufferInUse[i] = true;
							anyInUse = true;
						} else {
							if (this->streamBytesLeft == beforeBytes) {
								this->streamData++;
								this->streamBytesLeft--;
							}
						}
					}
				}
			} else {
				anyInUse = true;
			}
		}
	}
	if (!anyInUse && this->streamBytesLeft <= 0) {
		this->stop();
	}

#elif HAS_OPENSLES
	if (!this->isPlaying || !this->slBufferQueueItf || !this->isDecoderOpen) {
		return;
	}
	SLAndroidSimpleBufferQueueState qState;
	(*this->slBufferQueueItf)->GetState(this->slBufferQueueItf, &qState);
	while (qState.count < 4 && this->streamBytesLeft > 0) {
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
			applyUnderwaterFilter(this->pcmBuffer.data(), this->aacInfo.outputSamps, this->aacInfo.nChans);
			int numSamps = this->aacInfo.outputSamps;
			int bIdx = this->slBufferIndex;
			this->slPcmBuffers[bIdx].assign(this->pcmBuffer.begin(), this->pcmBuffer.begin() + numSamps);
			(*this->slBufferQueueItf)->Enqueue(this->slBufferQueueItf, this->slPcmBuffers[bIdx].data(), numSamps * sizeof(short));
			this->slBufferIndex = (bIdx + 1) % 4;
			(*this->slBufferQueueItf)->GetState(this->slBufferQueueItf, &qState);
		} else {
			if (this->streamBytesLeft == beforeBytes) {
				this->streamData++;
				this->streamBytesLeft--;
			}
			break;
		}
	}
	if (qState.count == 0 && this->streamBytesLeft <= 0) {
		this->stop();
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

	if (!this->isInitialized) {
		this->init();
	}

	if (MusicPlayerManager::instance.isPlaying) {
		if (!mc->level) {
			MusicPlayerManager::instance.stopImmediate();
			return;
		}
		this->currentMode = MUSIC_MODE_CUSTOM;
		this->pauseSeconds = 60.0f;
		if (this->isPlaying) {
			this->streamBuffers();
		} else {
			MusicPlayerManager::instance.onTrackFinished();
		}
		MusicPlayerManager::instance.update(mc);
		return;
	}

	float musicOpt = mc->options.musicVolume;
	this->setVolume(musicOpt);
	if (musicOpt <= 0.001f) {
		if (this->isPlaying) {
			this->stop();
		}
		return;
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
