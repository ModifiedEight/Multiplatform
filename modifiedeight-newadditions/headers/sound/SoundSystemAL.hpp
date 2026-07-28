#pragma once
#if (defined(__linux__) and not defined(ANDROID)) or defined(MCPE_IOS) or defined(__APPLE__)
#include <_types.h>
#include <sound/SoundSystem.hpp>

#if defined(MCPE_IOS) || defined(__APPLE__)
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

struct SoundSystemAL: public SoundSystem
{
	static const int MAX_PLAYED = 4;

	ALCdevice* device = 0;
	ALCcontext* context = 0;
	int playedCnt = 0;
	uint32_t buffers[MAX_PLAYED] = {0};
	uint32_t sources[MAX_PLAYED] = {0};

	SoundSystemAL(void);
	virtual ~SoundSystemAL();
	bool_t checkErr(uint32_t);
	void destroy(void);
	void init(void);
	void removeStoppedSounds(void);
	virtual void setListenerPos(float, float, float);
	virtual void setListenerAngle(float);
	virtual void load(const std::string&);
	virtual void play(const std::string&);
	virtual void pause(const std::string&);
	virtual void stop(const std::string&);
	virtual void playAt(const struct SoundDesc&, float, float, float, float, float);
};
#endif
