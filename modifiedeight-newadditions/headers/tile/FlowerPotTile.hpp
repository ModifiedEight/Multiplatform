#pragma once
#include <tile/Tile.hpp>
#include <tile/material/Material.hpp>
#include <entity/Player.hpp>
#include <level/Level.hpp>
#include <item/Item.hpp>
#include <item/ItemInstance.hpp>

class FlowerPotTile : public Tile {
public:
	FlowerPotTile(int32_t id, const std::string& name)
		: Tile(id, name, Material::decoration) {
		this->setShape(0.3125f, 0.0f, 0.3125f, 0.6875f, 0.375f, 0.6875f);
	}

	bool_t isSolidRender() override {
		return 0;
	}

	bool_t isCubeShaped() override {
		return 0;
	}

	int32_t getRenderShape() override {
		return 36;
	}

	int32_t getRenderLayer() override {
		return 1;
	}

	bool_t mayPlace(Level* level, int32_t x, int32_t y, int32_t z) override {
		if (!level) return 0;
		int below = level->getTile(x, y - 1, z);
		return Tile::solid[below] || (Tile::mixedSlab && below == Tile::mixedSlab->blockID);
	}

	void neighborChanged(Level* level, int32_t x, int32_t y, int32_t z, int32_t a5, int32_t a6, int32_t a7, int32_t a8) override {
		if (!mayPlace(level, x, y, z)) {
			spawnResources(level, x, y, z, level->getData(x, y, z), 1.0f);
			level->setTile(x, y, z, 0, 3);
		}
	}

	static void getPlant(int data, Tile*& plant, int& aux) {
		plant = nullptr;
		aux = 0;
		if (data == 1) plant = Tile::flowerRose;
		else if (data == 2) plant = Tile::rose;
		else if (data == 3) plant = Tile::flower;
		else if (data == 4) { plant = Tile::sapling; aux = 0; }
		else if (data == 5) { plant = Tile::sapling; aux = 1; }
		else if (data == 6) { plant = Tile::sapling; aux = 2; }
		else if (data == 7) { plant = Tile::sapling; aux = 3; }
		else if (data == 8) plant = Tile::mushroom2;
		else if (data == 9) plant = Tile::mushroom1;
		else if (data == 10) plant = Tile::cactus;
		else if (data == 11) plant = Tile::deadBush;
		else if (data == 12) { plant = Tile::tallgrass; aux = 2; }
		else if (data == 13) plant = Tile::flowerAllium;
		else if (data == 14) plant = Tile::flowerOrchid;
		else if (data == 15) plant = Tile::flowerDaisy;
	}

	void dropPlant(Level* level, int32_t x, int32_t y, int32_t z, int32_t data) {
		if (data <= 0) return;
		Tile* plant = nullptr;
		int aux = 0;
		getPlant(data, plant, aux);
		if (plant) {
			ItemInstance inst(plant, 1, aux);
			this->popResource(level, x, y, z, inst);
		}
	}

	void attack(Level* level, int32_t x, int32_t y, int32_t z, Player* player) override {
		if (!level) return;
		int data = level->getData(x, y, z);
		if (data > 0) {
			dropPlant(level, x, y, z, data);
			level->setData(x, y, z, 0, 3);
			return;
		}
	}

	void playerDestroy(Level* level, Player* player, int32_t x, int32_t y, int32_t z, int32_t data) override {
		if (data > 0) {
			dropPlant(level, x, y, z, data);
			level->setTileAndData(x, y, z, this->blockID, 0, 3);
			return;
		}
		Tile::playerDestroy(level, player, x, y, z, data);
	}

	void playerWillDestroy(Level* level, int32_t x, int32_t y, int32_t z, int32_t data, Player* player) override {
		if (level && data > 0) {
			dropPlant(level, x, y, z, data);
		}
	}

	bool_t use(Level* level, int32_t x, int32_t y, int32_t z, Player* player) override {
		if (!level || !player) return 0;
		int currentData = level->getData(x, y, z);
		if (currentData == 0) {
			ItemInstance* held = player->getSelectedItem();
			if (!held || held->count <= 0) return 0;

			int plantData = 0;
			int id = held->getId();
			int aux = held->getAuxValue();

			if (Tile::flowerRose && id == Tile::flowerRose->blockID) plantData = 1;
			else if (Tile::rose && id == Tile::rose->blockID) plantData = 2;
			else if (Tile::flower && id == Tile::flower->blockID) plantData = 3;
			else if (Tile::sapling && id == Tile::sapling->blockID) {
				if (aux == 0) plantData = 4;
				else if (aux == 1) plantData = 5;
				else if (aux == 2) plantData = 6;
				else if (aux == 3) plantData = 7;
			}
			else if (Tile::mushroom2 && id == Tile::mushroom2->blockID) plantData = 8;
			else if (Tile::mushroom1 && id == Tile::mushroom1->blockID) plantData = 9;
			else if (Tile::cactus && id == Tile::cactus->blockID) plantData = 10;
			else if (Tile::deadBush && id == Tile::deadBush->blockID) plantData = 11;
			else if (Tile::tallgrass && id == Tile::tallgrass->blockID && aux == 2) plantData = 12;
			else if (Tile::flowerAllium && id == Tile::flowerAllium->blockID) plantData = 13;
			else if (Tile::flowerOrchid && id == Tile::flowerOrchid->blockID) plantData = 14;
			else if (Tile::flowerDaisy && id == Tile::flowerDaisy->blockID) plantData = 15;
			else if (Tile::flowerPaeonia && id == Tile::flowerPaeonia->blockID) plantData = 15;
			else if (Tile::flowerHoustonia && id == Tile::flowerHoustonia->blockID) plantData = 15;

			if (plantData > 0) {
				level->setData(x, y, z, plantData, 3);
				level->playSound((float)x + 0.5f, (float)y + 0.5f, (float)z + 0.5f, "step.grass", 1.0f, 1.0f);
				if (!player->abilities.instabuild) {
					held->count--;
				}
				return 1;
			}
		}
		return 0;
	}

	TextureUVCoordinateSet* getCarriedTexture(int32_t side, int32_t data) override {
		static TextureUVCoordinateSet potUV = Tile::getTextureUVCoordinateSet("flower_pot", 0);
		return &potUV;
	}

	int32_t getResource(int32_t data, Random* rand) override {
		return Item::flowerPot ? Item::flowerPot->itemID : (this->blockID - 256);
	}

	void spawnResources(Level* level, int32_t x, int32_t y, int32_t z, int32_t data, float chance) override {
		Tile::spawnResources(level, x, y, z, 0, chance);
		if (data > 0) {
			dropPlant(level, x, y, z, data);
		}
	}
};
