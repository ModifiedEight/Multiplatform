#include <tile/DoublePlantTile.hpp>
#include <level/Level.hpp>
#include <item/Item.hpp>
#include <item/ItemInstance.hpp>
#include <entity/Player.hpp>
#include <tile/material/Material.hpp>
#include <tile/BlockColorRegistry.hpp>
#include <rendering/TextureAtlasTextureItem.hpp>

DoublePlantTile::DoublePlantTile(int32_t id, const std::string& name)
	: Bush(id, name, Material::replaceable_plant) {
	this->setShape(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	this->replaceable = 1;

	TextureAtlasTextureItem texGrassB(*this->getTextureItem("double_plant_grass_bottom"));
	this->texGrassBottom = *texGrassB.getUV(0);
	TextureAtlasTextureItem texGrassT(*this->getTextureItem("double_plant_grass_top"));
	this->texGrassTop = *texGrassT.getUV(0);

	TextureAtlasTextureItem texFernB(*this->getTextureItem("double_plant_fern_bottom"));
	this->texFernBottom = *texFernB.getUV(0);
	TextureAtlasTextureItem texFernT(*this->getTextureItem("double_plant_fern_top"));
	this->texFernTop = *texFernT.getUV(0);

	TextureAtlasTextureItem texPaeB(*this->getTextureItem("double_plant_paeonia_bottom"));
	this->texPaeoniaBottom = *texPaeB.getUV(0);
	TextureAtlasTextureItem texPaeT(*this->getTextureItem("double_plant_paeonia_top"));
	this->texPaeoniaTop = *texPaeT.getUV(0);

	TextureAtlasTextureItem texRoseB(*this->getTextureItem("double_plant_rose_bottom"));
	this->texRoseBottom = *texRoseB.getUV(0);
	TextureAtlasTextureItem texRoseT(*this->getTextureItem("double_plant_rose_top"));
	this->texRoseTop = *texRoseT.getUV(0);
}

DoublePlantTile::~DoublePlantTile() {
}

TextureUVCoordinateSet* DoublePlantTile::getTexture(int32_t face, int32_t data) {
	bool isTop = (data & 8) != 0;
	int32_t type = data & 7;

	switch (type) {
		case 1:
			return isTop ? &this->texFernTop : &this->texFernBottom;
		case 2:
			return isTop ? &this->texPaeoniaTop : &this->texPaeoniaBottom;
		case 3:
			return isTop ? &this->texRoseTop : &this->texRoseBottom;
		case 0:
		default:
			return isTop ? &this->texGrassTop : &this->texGrassBottom;
	}
}

TextureUVCoordinateSet* DoublePlantTile::getCarriedTexture(int32_t face, int32_t data) {
	int32_t type = data & 7;
	switch (type) {
		case 1:
			return &this->texFernTop;
		case 2:
			return &this->texPaeoniaTop;
		case 3:
			return &this->texRoseTop;
		case 0:
		default:
			return &this->texGrassTop;
	}
}

int32_t DoublePlantTile::getColor(int32_t data) {
	int32_t type = data & 7;
	if (type == 0 || type == 1) {
		if (type == 1) return 0x5B8F32;
		return 0x66A538;
	}
	return 0xFFFFFF;
}

int32_t DoublePlantTile::getColor(LevelSource* level, int32_t x, int32_t y, int32_t z) {
	int32_t data = level->getData(x, y, z);
	int32_t type = data & 7;
	if (data & 8) {
		type = level->getData(x, y - 1, z) & 7;
	}
	if (type == 0 || type == 1) {
		if (level && BlockColorRegistry::hasBlockColor(x, y, z)) {
			return BlockColorRegistry::getBlockColor(x, y, z) & 0xFFFFFF;
		}
		if (type == 1) return 0x5B8F32;
		return Tile::grass ? Tile::grass->getColor(level, x, (data & 8) ? y - 1 : y, z) : 0x66A538;
	}
	return 0xFFFFFF;
}

bool_t DoublePlantTile::canSurvive(Level* level, int32_t x, int32_t y, int32_t z) {
	int32_t data = level->getData(x, y, z);
	if (data & 8) {
		return level->getTile(x, y - 1, z) == this->blockID;
	}
	return Bush::canSurvive(level, x, y, z);
}

bool_t DoublePlantTile::mayPlace(Level* level, int32_t x, int32_t y, int32_t z, uint8_t side) {
	return Bush::mayPlace(level, x, y, z, side) && level->isEmptyTile(x, y + 1, z);
}

void DoublePlantTile::neighborChanged(Level* level, int32_t x, int32_t y, int32_t z, int32_t fromX, int32_t fromY, int32_t fromZ, int32_t fromTileId) {
	int32_t data = level->getData(x, y, z);
	if (data & 8) {
		if (level->getTile(x, y - 1, z) != this->blockID) {
			level->setTile(x, y, z, 0, 3);
		}
	} else {
		if (level->getTile(x, y + 1, z) != this->blockID || !Bush::canSurvive(level, x, y, z)) {
			this->popResource(level, x, y, z, ItemInstance(this->blockID, 1, data & 7));
			level->setTile(x, y, z, 0, 3);
			if (level->getTile(x, y + 1, z) == this->blockID) {
				level->setTile(x, y + 1, z, 0, 3);
			}
		}
	}
}

void DoublePlantTile::playerDestroy(Level* level, Player* player, int32_t x, int32_t y, int32_t z, int32_t meta) {
	if (meta & 8) {
		if (level->getTile(x, y - 1, z) == this->blockID) {
			int32_t bData = level->getData(x, y - 1, z);
			if (player && !player->abilities.instabuild) {
				this->popResource(level, x, y, z, ItemInstance(this->blockID, 1, bData & 7));
			}
			level->setTile(x, y - 1, z, 0, 3);
		}
	} else {
		if (level->getTile(x, y + 1, z) == this->blockID) {
			level->setTile(x, y + 1, z, 0, 3);
		}
		if (player && !player->abilities.instabuild) {
			this->popResource(level, x, y, z, ItemInstance(this->blockID, 1, meta & 7));
		}
	}
	level->setTile(x, y, z, 0, 3);
}

int32_t DoublePlantTile::getSpawnResourcesAuxValue(int32_t meta) {
	return meta & 7;
}

int32_t DoublePlantTile::getResource(int32_t meta, Random* random) {
	return this->blockID;
}

int32_t DoublePlantTile::getResourceCount(Random* random) {
	return 1;
}
