#pragma once
#include <entity/WaterAnimal.hpp>

struct Squid: WaterAnimal {
	float xBodyRot;
	float xBodyRotO;
	float zBodyRot;
	float zBodyRotO;
	float tentacleMovement;
	float oldTentacleMovement;
	float tentacleAngle;
	float oldTentacleAngle;
	float tentacleSpeed;
	float rotateSpeed;
	float tx;
	float ty;
	float tz;

	Squid(Level*);
	virtual ~Squid();

	virtual int32_t getMaxHealth();
	virtual int32_t getEntityTypeId() const;
	virtual std::string* getTexture();
	virtual void aiStep();
	virtual void die(Entity*);
	virtual void dropDeathLoot();
	virtual int32_t getAmbientSoundInterval();
	virtual const char_t* getAmbientSound();
	virtual std::string getHurtSound();
	virtual std::string getDeathSound();
	virtual bool_t hurt(Entity*, int32_t);
	virtual void travel(float, float);
};
