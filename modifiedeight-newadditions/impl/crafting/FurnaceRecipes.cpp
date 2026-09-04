#include <crafting/FurnaceRecipes.hpp>
#include <item/Item.hpp>
#include <item/ItemInstance.hpp>
#include <tile/Tile.hpp>

FurnaceRecipes* FurnaceRecipes::instance = 0;

FurnaceRecipes::FurnaceRecipes() {
	this->addFurnaceRecipe(Tile::ironOre->blockID, ItemInstance(Item::ironIngot));
	this->addFurnaceRecipe(Tile::goldOre->blockID, ItemInstance(Item::goldIngot));
	if (Tile::copperOre && Item::copperIngot) {
		this->addFurnaceRecipe(Tile::copperOre->blockID, ItemInstance(Item::copperIngot));
	}
	this->addFurnaceRecipe(Tile::emeraldOre->blockID, ItemInstance(Item::emerald));
	this->addFurnaceRecipe(Tile::sand->blockID, ItemInstance(Tile::glass));
	this->addFurnaceRecipe(Item::porkChop_raw->itemID, ItemInstance(Item::porkChop_cooked));
	this->addFurnaceRecipe(Item::beef_raw->itemID, ItemInstance(Item::beef_cooked));
	this->addFurnaceRecipe(Item::chicken_raw->itemID, ItemInstance(Item::chicken_cooked));
	this->addFurnaceRecipe(Tile::stoneBrick->blockID, ItemInstance(Tile::rock));
	this->addFurnaceRecipe(Item::clay->itemID, ItemInstance(Item::brick));
	this->addFurnaceRecipe(Tile::cactus->blockID, ItemInstance(Item::dye_powder, 1, 2));
	this->addFurnaceRecipe(Tile::tallgrass->blockID, ItemInstance(Item::dye_powder, 1, 2));
	if (Tile::doublePlant) {
		this->addFurnaceRecipe(Tile::doublePlant->blockID, ItemInstance(Item::dye_powder, 1, 2));
	}
	if (Tile::seagrass) {
		this->addFurnaceRecipe(Tile::seagrass->blockID, ItemInstance(Item::dye_powder, 1, 2));
	}
	this->addFurnaceRecipe(Tile::mushroom2->blockID, ItemInstance(Item::dye_powder, 1, 1));
	this->addFurnaceRecipe(Tile::treeTrunk->blockID, ItemInstance(Item::coal, 1, 1));
	this->addFurnaceRecipe(Tile::netherrack->blockID, ItemInstance(Item::netherbrick));
	this->addFurnaceRecipe(Item::potato->itemID, ItemInstance(Item::potatoBaked));
	this->addFurnaceRecipe(Item::fish_raw ? Item::fish_raw->itemID : 349, ItemInstance(Item::fish_cooked ? Item::fish_cooked->itemID : 350, 1, 0));
	this->addFurnaceRecipe(Item::salmon_raw ? Item::salmon_raw->itemID : 462, ItemInstance(Item::salmon_cooked ? Item::salmon_cooked->itemID : 463, 1, 0));
}
void FurnaceRecipes::addFurnaceRecipe(int32_t a2, const ItemInstance& a3) {
	this->recipes.insert({a2, ItemInstance(a3)});
}
FurnaceRecipes* FurnaceRecipes::getInstance() {
	if(!FurnaceRecipes::instance) {
		FurnaceRecipes::instance = new FurnaceRecipes();
	}
	return FurnaceRecipes::instance;
}
void FurnaceRecipes::getRecipes() {
}
ItemInstance FurnaceRecipes::getResult(int32_t a2) {
	auto&& p = this->recipes.find(a2);
	if(p == this->recipes.end()) {
		return ItemInstance();
	}
	return ItemInstance(p->second);
}
bool_t FurnaceRecipes::isFurnaceItem(int32_t a2) {
	return this->recipes.find(a2) != this->recipes.end();
}
void FurnaceRecipes::teardownFurnaceRecipes() {
	if(FurnaceRecipes::instance) {
		delete FurnaceRecipes::instance;
		FurnaceRecipes::instance = 0;
	}
}
