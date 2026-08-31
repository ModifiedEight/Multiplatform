#pragma once
#include <item/Item.hpp>
#include <rendering/TextureUVCoordinateSet.hpp>

struct FishingRodItem: Item {
	TextureUVCoordinateSet castTexture;

	FishingRodItem(int32_t);
	virtual ~FishingRodItem();
	virtual bool_t isMirroredArt();
	virtual TextureUVCoordinateSet* getIcon(int32_t, int32_t, bool_t);
	virtual int32_t getAnimationFrameFor(Mob*);
	virtual ItemInstance* use(ItemInstance*, Level*, Player*);
};
