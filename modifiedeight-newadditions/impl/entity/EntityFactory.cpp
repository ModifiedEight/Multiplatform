#include <entity/EntityFactory.hpp>
#include <entity/Mob.hpp>
#include <entity/ItemEntity.hpp>
#include <entity/PrimedTnt.hpp>
#include <entity/FallingTile.hpp>
#include <entity/Arrow.hpp>
#include <entity/Snowball.hpp>
#include <entity/ThrownEgg.hpp>
#include <entity/Painting.hpp>
#include <entity/MinecartRideable.hpp>
#include <nbt/CompoundTag.hpp>
#include <entity/MobFactory.hpp>
#include <entity/Villager.hpp>
#include <entity/ZombieVillager.hpp>
#include <entity/ItemFrame.hpp>
#include <entity/ArmorStand.hpp>
#include <entity/Frog.hpp>
#include <entity/Boat.hpp>

Entity* EntityFactory::CreateEntity(int32_t type, Level* level) {
	switch(type) {
		case 40:
		case 41:
			return new Boat(level);
		case 64:
			return new ItemEntity(level);
		case 65:
			return new PrimedTnt(level);
		case 66:
			return new FallingTile(level);
		case 71:
			return new ItemFrame(level);
		case 78:
			return new ArmorStand(level);
		case 80:
			return new Arrow(level);
		case 81:
			return new Snowball(level);
		case 82:
			return new ThrownEgg(level);
		case 83:
			return new Painting(level);
		case 84:
			return new MinecartRideable(level);
		case 120:
			return new Villager(level);
		case 121:
			return new ZombieVillager(level);
		case 122:
			return new Frog(level);
		default:
			return 0;
	}
}
Entity* EntityFactory::loadEntity(CompoundTag* tag, Level* level) {
	int32_t id = tag->getInt("id");
	if(id < 0) {
		return 0;
	}
	Entity* v7 = id > 63 ? EntityFactory::CreateEntity(id, level) : MobFactory::CreateMob(id, level);
	if(!v7) {
		return 0;
	}
	v7->load(tag);
	if(v7->isItemEntity() && !((ItemEntity*)v7)->itemInstance.isValid) {
		delete v7;
		return 0;
	}
	return v7;
}
