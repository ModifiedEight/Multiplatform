#include <item/FishingRodItem.hpp>
#include <item/ItemInstance.hpp>
#include <entity/FishingHook.hpp>
#include <entity/Player.hpp>
#include <level/Level.hpp>

FishingRodItem::FishingRodItem(int32_t id)
	: Item(id) {
	this->setMaxDamage(64);
	this->setMaxStackSize(1);
	this->handEquipped();
	this->castTexture = this->getTextureUVCoordinateSet("fishing_rod_cast", 0);
}

FishingRodItem::~FishingRodItem() {
}

bool_t FishingRodItem::isMirroredArt() {
	return 1;
}

TextureUVCoordinateSet* FishingRodItem::getIcon(int32_t damage, int32_t frame, bool_t inHand) {
	if (frame > 0) {
		return &this->castTexture;
	}
	return &this->iconUV;
}

int32_t FishingRodItem::getAnimationFrameFor(Mob* mob) {
	Player* player = (Player*)mob;
	if (player && player->fishing) {
		return 1;
	}
	return 0;
}

ItemInstance* FishingRodItem::use(ItemInstance* item, Level* level, Player* player) {
	if (player->fishing) {
		int32_t damage = player->fishing->retrieve(item);
		if (damage > 0 && !player->abilities.instabuild) {
			item->hurtAndBreak(damage, player);
		}
		player->swing();
	} else {
		if (level) {
			level->playSound(player, "random.bow", 0.5f, 0.4f / (Item::random.nextFloat() * 0.4f + 0.8f));
			FishingHook* hook = new FishingHook(level, player);
			level->addEntity(hook);
		}
		player->swing();
	}
	return item;
}
