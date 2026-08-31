#include <tile/SweetBerryBushTile.hpp>
#include <tile/material/Material.hpp>
#include <level/Level.hpp>
#include <entity/Entity.hpp>
#include <entity/Player.hpp>
#include <inventory/Inventory.hpp>
#include <item/Item.hpp>
#include <item/ItemInstance.hpp>
#include <rendering/TextureAtlasTextureItem.hpp>
#include <cmath>

SweetBerryBushTile::SweetBerryBushTile(int32_t id, const std::string& name)
	: Bush(id, name, Material::plant) {
	this->setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	this->setTicking(true);
}

SweetBerryBushTile::~SweetBerryBushTile() {
}

TextureUVCoordinateSet* SweetBerryBushTile::getTexture(int32_t face, int32_t data) {
	int32_t age = data & 3;
	static const char* stageNames[4] = {
		"sweet_berry_bush_stage0",
		"sweet_berry_bush_stage1",
		"sweet_berry_bush_stage2",
		"sweet_berry_bush_stage3"
	};
	TextureAtlasTextureItem* item = this->getTextureItem(stageNames[age]);
	if (item) {
		return item->getUV(0);
	}
	return &this->textureUV;
}

TextureUVCoordinateSet* SweetBerryBushTile::getCarriedTexture(int32_t face, int32_t data) {
	TextureAtlasTextureItem* item = this->getTextureItem("sweet_berry_bush_stage3");
	if (item) {
		return item->getUV(0);
	}
	return &this->textureUV;
}

AABB* SweetBerryBushTile::getAABB(Level* level, int32_t x, int32_t y, int32_t z) {
	return nullptr;
}

bool_t SweetBerryBushTile::isSolidRender() {
	return 0;
}

bool_t SweetBerryBushTile::isCubeShaped() {
	return 0;
}

int32_t SweetBerryBushTile::getRenderLayer() {
	return 1;
}

int32_t SweetBerryBushTile::getRenderShape() {
	return 1;
}

void SweetBerryBushTile::entityInside(Level* level, int32_t x, int32_t y, int32_t z, Entity* ent) {
	if (!ent) return;
	ent->motionX *= 0.35f;
	ent->motionZ *= 0.35f;
	int32_t data = level->getData(x, y, z);
	int32_t age = data & 3;
	if (age > 0) {
		float speedSq = ent->motionX * ent->motionX + ent->motionZ * ent->motionZ;
		if (speedSq > 0.00005f || std::abs(ent->motionY) > 0.05f) {
			static int s_dmgTick = 0;
			if ((++s_dmgTick % 15) == 0) {
				ent->hurt(nullptr, 1);
			}
		}
	}
}

bool_t SweetBerryBushTile::use(Level* level, int32_t x, int32_t y, int32_t z, Player* player) {
	int32_t data = level->getData(x, y, z);
	int32_t age = data & 3;
	if (player && player->inventory) {
		ItemInstance* held = player->inventory->getSelected();
		if (held && !held->isNull() && held->getId() == Item::dye_powder->itemID && held->getAuxValue() == 15) {
			if (age < 3) {
				level->setData(x, y, z, std::min(3, age + 1), 3);
				for (int i = 0; i < 6; ++i) {
					float px = (float)x + 0.2f + (float)(rand() % 60) / 100.0f;
					float py = (float)y + 0.2f + (float)(rand() % 60) / 100.0f;
					float pz = (float)z + 0.2f + (float)(rand() % 60) / 100.0f;
					level->addParticle(PT_SMOKE, px, py, pz, 0.0, 0.02, 0.0, 0);
				}
				if (player->inventory->field_20 == 0) {
					--held->count;
					if (held->count <= 0) player->inventory->setItem(player->inventory->selectedSlot, nullptr);
				}
				return 1;
			}
		}
	}
	if (age >= 2) {
		int32_t dropCount = (age == 3) ? (2 + (rand() % 2)) : 1;
		if (!level->isClientMaybe && Item::sweetBerries) {
			for (int32_t i = 0; i < dropCount; ++i) {
				this->popResource(level, x, y, z, ItemInstance(Item::sweetBerries, 1, 0));
			}
		}
		level->playSound((float)x + 0.5f, (float)y + 0.5f, (float)z + 0.5f, "block.sweet_berry_bush.pick_berries", 1.0f, 0.8f + (float)(rand() % 40) / 100.0f);
		level->setData(x, y, z, 1, 3);
		return 1;
	}
	return 0;
}

void SweetBerryBushTile::tick(Level* level, int32_t x, int32_t y, int32_t z, Random* rand) {
	if (level->isClientMaybe) return;
	int32_t data = level->getData(x, y, z);
	int32_t age = data & 3;
	if (age < 3 && (rand->genrand_int32() % 5) == 0 && level->getBrightness(x, y + 1, z) >= 0.5f) {
		level->setData(x, y, z, age + 1, 3);
	}
}

int32_t SweetBerryBushTile::getResource(int32_t data, Random* rand) {
	if ((data & 3) >= 3) {
		return Item::sweetBerries ? Item::sweetBerries->itemID : 0;
	}
	return this->blockID;
}

int32_t SweetBerryBushTile::getResourceCount(Random* rand) {
	return 1;
}

int32_t SweetBerryBushTile::getSpawnResourcesAuxValue(int32_t data) {
	return 0;
}

bool_t SweetBerryBushTile::mayPlaceOn(int32_t tileId) {
	return tileId == Tile::grass->blockID || tileId == Tile::dirt->blockID || tileId == Tile::farmland->blockID || (Tile::grassPath && tileId == Tile::grassPath->blockID);
}
