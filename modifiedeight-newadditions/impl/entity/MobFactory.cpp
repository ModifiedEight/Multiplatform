#include <entity/MobFactory.hpp>
#include <entity/Chicken.hpp>
#include <entity/Cow.hpp>
#include <entity/Pig.hpp>
#include <entity/Sheep.hpp>
#include <entity/Zombie.hpp>
#include <entity/Creeper.hpp>
#include <entity/Skeleton.hpp>
#include <entity/Spider.hpp>
#include <entity/PigZombie.hpp>
#include <entity/Squid.hpp>
#include <entity/Cod.hpp>
#include <entity/Salmon.hpp>
#include <entity/Pufferfish.hpp>
#include <entity/TropicalFish.hpp>
#include <entity/Slime.hpp>
#include <entity/Wolf.hpp>
#include <entity/PolarBear.hpp>
#include <entity/Fox.hpp>
#include <entity/Turtle.hpp>
#include <entity/Giant.hpp>
#include <entity/Villager.hpp>
#include <entity/ZombieVillager.hpp>
#include <entity/Frog.hpp>
#include <entity/Ocelot.hpp>

Mob* MobFactory::CreateMob(int32_t type, Level* level) {
	Mob* v3; // r4

	switch(type) {
		case 10:
			v3 = new Chicken(level);
			break;
		case 11:
			v3 = new Cow(level);
			break;
		case 12:
			v3 = new Pig(level);
			break;
		case 13:
			v3 = new Sheep(level);
			break;
		case 14:
			v3 = new Wolf(level);
			break;
		case 17:
			v3 = new Squid(level);
			break;
		case 22:
			v3 = new Ocelot(level);
			break;
		case 26:
			v3 = new PolarBear(level);
			break;
		case 27:
			v3 = new Cod(level);
			break;
		case 28:
			v3 = new Salmon(level);
			break;
		case 29:
			v3 = new Pufferfish(level);
			break;
		case 30:
			v3 = new TropicalFish(level);
			break;
		case 32:
			v3 = new Zombie(level);
			break;
		case 33:
			v3 = new Creeper(level);
			break;
		case 34:
			v3 = new Skeleton(level);
			break;
		case 35:
			v3 = new Spider(level);
			break;
		case 36:
			v3 = new PigZombie(level);
			break;
		case 37:
			v3 = new Slime(level);
			break;
		case 38:
			v3 = new Fox(level);
			break;
		case 39:
			v3 = new Turtle(level);
			break;
		case 40:
			v3 = new Frog(level);
			break;
		case 53:
			v3 = new Giant(level);
			break;
		case 15:
		case 120:
			v3 = new Villager(level);
			break;
		case 44:
		case 121:
			v3 = new ZombieVillager(level);
			break;
		default:
			v3 = 0;
			break;
	}
	if(v3) {
		v3->postInit();
	}
	return v3;
}
std::string MobFactory::GetMobNameID(int32_t type) {
	switch(type) {
		case 10:
			return "entity.Chicken";

		case 11:
			return "entity.Cow";

		case 12:
			return "entity.Pig";

		case 13:
			return "entity.Sheep";

		case 14:
			return "entity.Wolf";

		case 15:
		case 120:
			return "entity.Villager";

		case 17:
			return "entity.Squid";

		case 22:
			return "entity.Ocelot";

		case 26:
			return "entity.PolarBear";

		case 27:
			return "entity.Cod";

		case 28:
			return "entity.Salmon";

		case 29:
			return "entity.Pufferfish";

		case 30:
			return "entity.TropicalFish";

		case 32:
			return "entity.Zombie";

		case 33:
			return "entity.Creeper";

		case 34:
			return "entity.Skeleton";

		case 35:
			return "entity.Spider";

		case 36:
			return "entity.PigZombie";

		case 37:
			return "entity.Slime";

		case 38:
			return "entity.Fox";

		case 39:
			return "entity.Turtle";
		case 44:
		case 121:
			return "entity.ZombieVillager";
		case 40:
			return "entity.Frog";
		case 53:
			return "entity.Giant";
		default:
			return "";
	}
}
Mob* MobFactory::getStaticTestMob(int32_t a2, Level* a3) {
	static std::map<int32_t, Mob*> _mobs;
	static Level* lastLevel;

	bool_t v4;
	if(a2) v4 = 0;
	else v4 = a3 == 0;

	Level* ll = lastLevel;
	lastLevel = a3;
	if(!v4 && a3 == ll) {
		goto LABEL_9;
	}
	for(auto& mp: _mobs) {
		if(mp.second) delete mp.second;
	}
	_mobs.clear();
	if(v4) return 0;

LABEL_9:
	if(_mobs.count(a2) != 0) {
		return _mobs.at(a2);
	}
	Mob* m = MobFactory::CreateMob(a2, a3);
	_mobs.insert({a2, m});
	return m;
}
