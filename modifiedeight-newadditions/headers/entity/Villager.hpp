#pragma once
#include <entity/PathfinderMob.hpp>
#include <item/ItemInstance.hpp>

struct VillagerTrade {
	int32_t inputId;
	int32_t inputCount;
	int32_t outputId;
	int32_t outputCount;
	int32_t inputMeta;
	int32_t outputMeta;
};

struct Villager : PathfinderMob {
	int32_t zombieHitCount;
	int32_t tradeCount;
	VillagerTrade trades[5];
	int32_t houseX, houseY, houseZ;
	bool_t hasBed;
	int32_t sleepTimer;
	int32_t profession;

	Villager(Level*);

	void initTrades();
	void convertToZombieVillager();

	virtual ~Villager();
	virtual int32_t getEntityTypeId() const;
	virtual int32_t getCreatureBaseType();
	virtual void die(Entity*);
	virtual bool_t hurt(Entity*, int32_t);
	virtual float getBaseSpeed();
	virtual int32_t getMaxHealth();
	virtual void aiStep();
	virtual int32_t getDeathLoot();
	virtual const char_t* getAmbientSound();
	virtual std::string getHurtSound();
	virtual std::string getDeathSound();
	virtual bool_t interactWithPlayer(Player*);
	virtual bool_t interactPreventDefault() { return 1; }
	virtual void push(Entity*);
	virtual void readAdditionalSaveData(CompoundTag*);
	virtual void addAdditonalSaveData(CompoundTag*);
};

