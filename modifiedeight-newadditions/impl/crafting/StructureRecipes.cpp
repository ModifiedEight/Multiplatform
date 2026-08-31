#include <crafting/StructureRecipes.hpp>
#include <crafting/Recipes.hpp>
#include <item/Item.hpp>
#include <tile/Tile.hpp>

void StructureRecipes::addRecipes(Recipes* a1) {
	if (Tile::slimeBlock) a1->addShapedRecipe(ItemInstance(Tile::slimeBlock), "##", "##", definition<Item*>('#', Item::slimeBall));
	a1->addShapedRecipe(ItemInstance(Tile::chest), "###", "# #", "###", definition<Tile*>('#', Tile::wood));
	a1->addShapedRecipe(ItemInstance(Tile::furnace), "###", "# #", "###", definition<Tile*>('#', Tile::stoneBrick));
	a1->addShapedRecipe(ItemInstance(Tile::workBench), "##", "##", definition<Tile*>('#', Tile::wood));
	a1->addShapedRecipe(ItemInstance(Tile::stonecutterBench), "##", "##", definition<Tile*>('#', Tile::stoneBrick));
	a1->addShapedRecipe(ItemInstance(Tile::sandStone), "##", "##", definition<Tile*>('#', Tile::sand));
	a1->addShapedRecipe(ItemInstance(Tile::sandStone, 4, 2), "##", "##", definition<Tile*>('#', Tile::sandStone));
	a1->addShapedRecipe(ItemInstance(Tile::sandStone, 1, 1), "#", "#", definition<ItemInstance>('#', ItemInstance(Tile::stoneSlabHalf, 1, 1)));
	a1->addShapedRecipe(ItemInstance(Tile::stoneBrickSmooth, 4), "##", "##", definition<Tile*>('#', Tile::rock));
	a1->addShapedRecipe(ItemInstance(Tile::thinGlass, 16), "###", "###", definition<Tile*>('#', Tile::glass));
	a1->addShapedRecipe(ItemInstance(Tile::netherBrick, 1), "NN", "NN", definition<Item*>('N', Item::netherbrick));
	a1->addShapedRecipe(ItemInstance(Tile::quartzBlock, 1), "NN", "NN", definition<Item*>('N', Item::netherQuartz));
	a1->addShapedRecipe(ItemInstance(Tile::quartzBlock, 1, 1), "#", "#", definition<ItemInstance>('#', ItemInstance(Tile::stoneSlabHalf, 1, 6)));
	a1->addShapedRecipe(ItemInstance(Tile::pressurePlateStone, 1), "##", definition<Tile*>('#', Tile::rock));
	a1->addShapedRecipe(ItemInstance(Tile::pressurePlate_cobblestone, 1), "##", definition<Tile*>('#', Tile::stoneBrick));
	a1->addShapedRecipe(ItemInstance(Tile::pressurePlatePlanks, 1), "##", definition<ItemInstance>('#', ItemInstance(Tile::wood, 1, 0)));
	a1->addShapedRecipe(ItemInstance(Tile::pressurePlate_spruce, 1), "##", definition<ItemInstance>('#', ItemInstance(Tile::wood, 1, 1)));
	a1->addShapedRecipe(ItemInstance(Tile::pressurePlate_birch, 1), "##", definition<ItemInstance>('#', ItemInstance(Tile::wood, 1, 2)));
	a1->addShapedRecipe(ItemInstance(Tile::pressurePlate_jungle, 1), "##", definition<ItemInstance>('#', ItemInstance(Tile::wood, 1, 3)));
	a1->addShapedRecipe(ItemInstance(Tile::pressurePlate_gold, 1), "##", definition<Item*>('#', Item::goldIngot));
	a1->addShapedRecipe(ItemInstance(Tile::pressurePlate_iron, 1), "##", definition<Item*>('#', Item::ironIngot));
	a1->addShapedRecipe(ItemInstance(Tile::button_stone, 1), "#", definition<Tile*>('#', Tile::rock));
	a1->addShapedRecipe(ItemInstance(Tile::button_cobblestone, 1), "#", definition<Tile*>('#', Tile::stoneBrick));
	a1->addShapedRecipe(ItemInstance(Tile::button_wood, 1), "#", definition<ItemInstance>('#', ItemInstance(Tile::wood, 1, 0)));
	a1->addShapedRecipe(ItemInstance(Tile::button_spruce, 1), "#", definition<ItemInstance>('#', ItemInstance(Tile::wood, 1, 1)));
	a1->addShapedRecipe(ItemInstance(Tile::button_birch, 1), "#", definition<ItemInstance>('#', ItemInstance(Tile::wood, 1, 2)));
	a1->addShapedRecipe(ItemInstance(Tile::button_jungle, 1), "#", definition<ItemInstance>('#', ItemInstance(Tile::wood, 1, 3)));
	a1->addShapedRecipe(ItemInstance(Tile::button_gold, 1), "#", definition<Item*>('#', Item::goldIngot));
	a1->addShapedRecipe(ItemInstance(Tile::button_iron, 1), "#", definition<Item*>('#', Item::ironIngot));
	a1->addShapedRecipe(ItemInstance(Item::flowerPot, 1), "# #", " # ", definition<Item*>('#', Item::brick));
	for (int32_t s = 0; s < 4; ++s) {
		a1->addShapedRecipe(ItemInstance(Tile::daylightDetector, 1), "GGG", "III", "SSS", definition<Tile*, Item*, ItemInstance>('G', Tile::glass, 'I', Item::ironIngot, 'S', ItemInstance(Tile::woodSlabHalf, 1, s)));
	}

	if (Tile::redstoneLampOff && Item::redStone && Tile::lightGem) {
		a1->addShapedRecipe(ItemInstance(Tile::redstoneLampOff, 1), " R ", "RGR", " R ", definition<Item*, Tile*>('R', Item::redStone, 'G', Tile::lightGem));
	}
}
