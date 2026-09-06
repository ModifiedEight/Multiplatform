#include <crafting/Recipes.hpp>
#include <Options.hpp>
#include <crafting/FurnaceRecipes.hpp>
#include <crafting/ShapelessRecipe.hpp>
#include <crafting/ShapedRecipe.hpp>
#include <crafting/ToolRecipes.hpp>
#include <crafting/WeaponRecipes.hpp>
#include <crafting/OreRecipes.hpp>
#include <crafting/FoodRecipes.hpp>
#include <crafting/StructureRecipes.hpp>
#include <crafting/ArmorRecipes.hpp>
#include <crafting/ClothDyeRecipes.hpp>
#include <item/Item.hpp>
#include <tile/Tile.hpp>

Recipes::Shape::Shape(const std::string& a2) {
	this->shape.push_back(a2);
}
Recipes::Shape::Shape(const std::string& a2, const std::string& a3) {
	this->shape.push_back(a2);
	this->shape.push_back(a3);
}
Recipes::Shape::Shape(const std::string& a2, const std::string& a3, const std::string& a4) {
	this->shape.push_back(a2);
	this->shape.push_back(a3);
	this->shape.push_back(a4);
}

Recipes::Type::Type(const Recipes::Type& a2)
	: itemInstance(a2.itemInstance) {
	this->item = a2.item;
	this->tile = a2.tile;
	this->chr = a2.chr;
}
Recipes::Type::Type(char_t a2, Item* a3) {
	this->item = a3;
	this->tile = 0;
	this->chr = a2;
}
Recipes::Type::Type(char_t a2, const ItemInstance& a3)
	: itemInstance(a3) {
	this->item = 0;
	this->tile = 0;
	this->chr = a2;
}
Recipes::Type::Type(char_t a2, Tile* a3) {
	this->tile = a3;
	this->item = 0;
	this->chr = a2;
}

Recipes* Recipes::instance = 0;

Recipes::Recipes() {
	ToolRecipes::addRecipes(this);
	WeaponRecipes::addRecipes(this);
	OreRecipes::addRecipes(this);
	FoodRecipes::addRecipes(this);
	StructureRecipes::addRecipes(this);
	ArmorRecipes::addRecipes(this);
	ClothDyeRecipes::addRecipes(this);

	this->addShapedRecipe(ItemInstance(Item::paper, 3), "###", definition<Item*>('#', Item::reeds));
	this->addShapedRecipe(ItemInstance(Item::dye_powder, 1, 15), "###", "###", "###", definition<Item*>('#', Item::bone));
	this->addShapedRecipe(ItemInstance(Item::lever, 1), "#", "X", definition<Item*, Tile*>('#', Item::stick, 'X', Tile::stoneBrick));
	this->addShapedRecipe(ItemInstance(Item::redstoneLamp, 1), " I ", "ITI", " I ", definition<Item*, Tile*>('I', Item::ironIngot, 'T', Tile::torch));
	
	for(int i = 0; i < 16; i++) {
		int dyeMeta = (~i) & 0xF;
		// this->addShapedRecipe(ItemInstance(Tile::stainedGlass, 8, i), "###", "#D#", "###", definition<Tile*, ItemInstance>('#', Tile::glass, 'D', ItemInstance(Item::dye_powder, 1, dyeMeta)));
		// this->addShapedRecipe(ItemInstance(Tile::stainedGlassPane, 16, i), "###", "###", definition<ItemInstance>('#', ItemInstance(Tile::stainedGlass, 1, i)));
	}
	this->addShapedRecipe(ItemInstance(Item::book, 1), "#", "#", "#", definition<Item*>('#', Item::paper));
	this->addShapedRecipe(ItemInstance(Tile::fence, 2), "###", "###", definition<Item*>('#', Item::stick));
	this->addShapedRecipe(ItemInstance(Tile::cobbleWall, 6, 0), "###", "###", definition<Tile*>('#', Tile::stoneBrick));
	if (Tile::stoneWall) this->addShapedRecipe(ItemInstance(Tile::stoneWall, 6), "###", "###", definition<Tile*>('#', Tile::rock));
	if (Tile::oakWall) this->addShapedRecipe(ItemInstance(Tile::oakWall, 6), "###", "###", definition<ItemInstance>('#', ItemInstance(Tile::wood, 1, 0)));
	if (Tile::spruceWall) this->addShapedRecipe(ItemInstance(Tile::spruceWall, 6), "###", "###", definition<ItemInstance>('#', ItemInstance(Tile::wood, 1, 1)));
	if (Tile::birchWall) this->addShapedRecipe(ItemInstance(Tile::birchWall, 6), "###", "###", definition<ItemInstance>('#', ItemInstance(Tile::wood, 1, 2)));
	if (Tile::jungleWall) this->addShapedRecipe(ItemInstance(Tile::jungleWall, 6), "###", "###", definition<ItemInstance>('#', ItemInstance(Tile::wood, 1, 3)));
	if (Tile::fence_jungle) this->addShapedRecipe(ItemInstance(Tile::fence_jungle, 3), "#W#", "#W#", definition<Item*, ItemInstance>('#', Item::stick, 'W', ItemInstance(Tile::wood, 1, 3)));
	if (Tile::brickWall) this->addShapedRecipe(ItemInstance(Tile::brickWall, 6), "###", "###", definition<Tile*>('#', Tile::redBrick));
	if (Tile::quartzWall) this->addShapedRecipe(ItemInstance(Tile::quartzWall, 6), "###", "###", definition<ItemInstance>('#', ItemInstance(Tile::quartzBlock, 1, 0)));
	if (Tile::chiseledQuartzWall) this->addShapedRecipe(ItemInstance(Tile::chiseledQuartzWall, 6), "###", "###", definition<ItemInstance>('#', ItemInstance(Tile::quartzBlock, 1, 1)));
	if (Tile::ironWall) this->addShapedRecipe(ItemInstance(Tile::ironWall, 6), "###", "###", definition<Tile*>('#', Tile::ironBlock));
	if (Tile::sandstoneWall) this->addShapedRecipe(ItemInstance(Tile::sandstoneWall, 6), "###", "###", definition<Tile*>('#', Tile::sandStone));
	if (Tile::stoneBrickWall) this->addShapedRecipe(ItemInstance(Tile::stoneBrickWall, 6), "###", "###", definition<ItemInstance>('#', ItemInstance(Tile::stoneBrickSmooth, 1, 0)));
	if (Tile::mossyStoneBrickWall) this->addShapedRecipe(ItemInstance(Tile::mossyStoneBrickWall, 6), "###", "###", definition<ItemInstance>('#', ItemInstance(Tile::stoneBrickSmooth, 1, 1)));
	if (Tile::crackedStoneBrickWall) this->addShapedRecipe(ItemInstance(Tile::crackedStoneBrickWall, 6), "###", "###", definition<ItemInstance>('#', ItemInstance(Tile::stoneBrickSmooth, 1, 2)));
	if (Tile::copperWall) this->addShapedRecipe(ItemInstance(Tile::copperWall, 6), "###", "###", definition<Tile*>('#', Tile::copperBlock));
	this->addShapedRecipe(ItemInstance(Tile::fenceGate, 1), "#W#", "#W#", definition<Item*, Tile*>('#', Item::stick, 'W', Tile::wood));
	this->addShapedRecipe(ItemInstance(Tile::bookshelf, 1), "###", "XXX", "###", definition<Tile*, Item*>('#', Tile::wood, 'X', Item::book) /*inlined*/);
	this->addShapedRecipe(ItemInstance(Tile::snow, 1), "##", "##", definition<Item*>('#', Item::snowBall));
	this->addShapedRecipe(ItemInstance(Tile::clay, 1), "##", "##", definition<Item*>('#', Item::clay));
	this->addShapedRecipe(ItemInstance(Tile::redBrick, 1), "##", "##", definition<Item*>('#', (Item*)Item::brick));
	this->addShapedRecipe(ItemInstance(Tile::lightGem, 1), "##", "##", definition<Item*>('#', Item::yellowDust));
	this->addShapedRecipe(ItemInstance(Tile::cloth, 1), "##", "##", definition<Item*>('#', Item::string));
	this->addShapedRecipe(ItemInstance(Tile::tnt, 1), "X#X", "#X#", "X#X", definition<Item*, Tile*>('X', Item::sulphur, '#', Tile::sand));
	this->addShapedRecipe(ItemInstance(Tile::stoneSlabHalf, 6, 3), "###", definition<Tile*>('#', Tile::stoneBrick));
	this->addShapedRecipe(ItemInstance(Tile::stoneSlabHalf, 6, 0), "###", definition<Tile*>('#', Tile::rock));
	this->addShapedRecipe(ItemInstance(Tile::stoneSlabHalf, 6, 1), "###", definition<Tile*>('#', Tile::sandStone));
	if (Tile::dirtSlabHalf) this->addShapedRecipe(ItemInstance(Tile::dirtSlabHalf, 6, 0), "###", definition<Tile*>('#', Tile::dirt));
	if (Tile::grassSlabHalf) this->addShapedRecipe(ItemInstance(Tile::grassSlabHalf, 6, 0), "###", definition<Tile*>('#', Tile::grass));
	if (Tile::rockSlabHalf) this->addShapedRecipe(ItemInstance(Tile::rockSlabHalf, 6, 0), "###", definition<Tile*>('#', Tile::rock));


	for(int32_t a3 = 0; a3 != 4; ++a3) {
		if(a3 != 3) {
			this->addShapedRecipe(ItemInstance(Tile::woodSlabHalf, 6, a3), "###", definition<ItemInstance>('#', ItemInstance(Tile::wood, 1, a3)));
		}
	}

	this->addShapedRecipe(ItemInstance(Tile::stoneSlabHalf, 6, 4), "###", definition<Tile*>('#', Tile::redBrick));
	this->addShapedRecipe(ItemInstance(Tile::stoneSlabHalf, 6, 5), "###", definition<Tile*>('#', Tile::stoneBrickSmooth));
	this->addShapedRecipe(ItemInstance(Tile::stoneSlabHalf, 6, 6), "###", definition<Tile*>('#', Tile::quartzBlock));
	this->addShapedRecipe(ItemInstance(Tile::ladder, 2), "# #", "###", "# #", definition<Item*>('#', Item::stick));
	this->addShapedRecipe(ItemInstance(Item::door_wood, 1), "##", "##", "##", definition<ItemInstance>('#', ItemInstance(Tile::wood, 1, 0)));
	this->addShapedRecipe(ItemInstance(Item::door_spruce, 1), "##", "##", "##", definition<ItemInstance>('#', ItemInstance(Tile::wood, 1, 1)));
	this->addShapedRecipe(ItemInstance(Item::door_birch, 1), "##", "##", "##", definition<ItemInstance>('#', ItemInstance(Tile::wood, 1, 2)));
	if (Item::door_jungle) this->addShapedRecipe(ItemInstance(Item::door_jungle, 1), "##", "##", "##", definition<ItemInstance>('#', ItemInstance(Tile::wood, 1, 3)));
	this->addShapedRecipe(ItemInstance(Item::door_iron, 1), "##", "##", "##", definition<Item*>('#', Item::ironIngot));
	this->addShapedRecipe(ItemInstance(Tile::trapdoor, 2), "###", "###", definition<Tile*>('#', Tile::wood));
	this->addShapedRecipe(ItemInstance(Tile::trapdoor_spruce, 2), "###", "###", definition<ItemInstance>('#', ItemInstance(Tile::wood, 1, 1)));
	this->addShapedRecipe(ItemInstance(Tile::trapdoor_birch, 2), "###", "###", definition<ItemInstance>('#', ItemInstance(Tile::wood, 1, 2)));
	if (Tile::trapdoor_jungle) this->addShapedRecipe(ItemInstance(Tile::trapdoor_jungle, 2), "###", "###", definition<ItemInstance>('#', ItemInstance(Tile::wood, 1, 3)));
	if (Tile::ironTrapdoor) this->addShapedRecipe(ItemInstance(Tile::ironTrapdoor, 1), "##", "##", definition<Item*>('#', Item::ironIngot));
	if (Item::boat) {
		this->addShapedRecipe(ItemInstance(Item::boat, 1, 0), "# #", "#S#", "###", definition<ItemInstance, Item*>('#', ItemInstance(Tile::wood, 1, 0), 'S', Item::shovel_wood));
		this->addShapedRecipe(ItemInstance(Item::boat, 1, 1), "# #", "#S#", "###", definition<ItemInstance, Item*>('#', ItemInstance(Tile::wood, 1, 1), 'S', Item::shovel_wood));
		this->addShapedRecipe(ItemInstance(Item::boat, 1, 2), "# #", "#S#", "###", definition<ItemInstance, Item*>('#', ItemInstance(Tile::wood, 1, 2), 'S', Item::shovel_wood));
		this->addShapedRecipe(ItemInstance(Item::boat, 1, 3), "# #", "#S#", "###", definition<ItemInstance, Item*>('#', ItemInstance(Tile::wood, 1, 3), 'S', Item::shovel_wood));
	}
	if (Tile::copperBlock && Item::copperIngot) {
		this->addShapedRecipe(ItemInstance(Tile::copperBlock, 1), "###", "###", "###", definition<ItemInstance>('#', ItemInstance(Item::copperIngot, 1)));
		this->addShapedRecipe(ItemInstance(Item::copperIngot, 9), "#", definition<Tile*>('#', Tile::copperBlock));
	}
	if (Tile::copperFence && Item::copperIngot) {
		this->addShapedRecipe(ItemInstance(Tile::copperFence, 16), "###", "###", definition<ItemInstance>('#', ItemInstance(Item::copperIngot, 1)));
	}
	if (Tile::copperWall && Tile::copperBlock) {
		this->addShapedRecipe(ItemInstance(Tile::copperWall, 6), "###", "###", definition<ItemInstance>('#', ItemInstance(Tile::copperBlock, 1)));
	}
	if (Tile::oakWall && Tile::wood) {
		this->addShapedRecipe(ItemInstance(Tile::oakWall, 6), "###", "###", definition<ItemInstance>('#', ItemInstance(Tile::wood, 1, 0)));
	}
	if (Tile::spruceWall && Tile::wood) {
		this->addShapedRecipe(ItemInstance(Tile::spruceWall, 6), "###", "###", definition<ItemInstance>('#', ItemInstance(Tile::wood, 1, 1)));
	}
	if (Tile::birchWall && Tile::wood) {
		this->addShapedRecipe(ItemInstance(Tile::birchWall, 6), "###", "###", definition<ItemInstance>('#', ItemInstance(Tile::wood, 1, 2)));
	}
	if (Tile::jungleWall && Tile::wood) {
		this->addShapedRecipe(ItemInstance(Tile::jungleWall, 6), "###", "###", definition<ItemInstance>('#', ItemInstance(Tile::wood, 1, 3)));
	}
	if (Tile::brickWall && Tile::redBrick) {
		this->addShapedRecipe(ItemInstance(Tile::brickWall, 6), "###", "###", definition<Tile*>('#', Tile::redBrick));
	}
	if (Tile::quartzWall && Tile::quartzBlock) {
		this->addShapedRecipe(ItemInstance(Tile::quartzWall, 6), "###", "###", definition<ItemInstance>('#', ItemInstance(Tile::quartzBlock, 1, 0)));
	}
	if (Tile::chiseledQuartzWall && Tile::quartzBlock) {
		this->addShapedRecipe(ItemInstance(Tile::chiseledQuartzWall, 6), "###", "###", definition<ItemInstance>('#', ItemInstance(Tile::quartzBlock, 1, 1)));
	}
	if (Tile::ironWall && Tile::ironBlock) {
		this->addShapedRecipe(ItemInstance(Tile::ironWall, 6), "###", "###", definition<Tile*>('#', Tile::ironBlock));
	}
	if (Tile::sandstoneWall && Tile::sandStone) {
		this->addShapedRecipe(ItemInstance(Tile::sandstoneWall, 6), "###", "###", definition<ItemInstance>('#', ItemInstance(Tile::sandStone, 1, 0)));
	}
	if (Tile::copperStairs && Tile::copperBlock) {
		this->addShapedRecipe(ItemInstance(Tile::copperStairs, 4), "#  ", "## ", "###", definition<ItemInstance>('#', ItemInstance(Tile::copperBlock, 1)));
	}
	if (Tile::copperSlabHalf && Tile::copperBlock) {
		this->addShapedRecipe(ItemInstance(Tile::copperSlabHalf, 6), "###", definition<ItemInstance>('#', ItemInstance(Tile::copperBlock, 1)));
	}
	if (Item::copperDoor && Item::copperIngot) {
		this->addShapedRecipe(ItemInstance(Item::copperDoor, 3), "##", "##", "##", definition<ItemInstance>('#', ItemInstance(Item::copperIngot, 1)));
	}
	if (Tile::copperTrapdoor && Item::copperIngot) {
		this->addShapedRecipe(ItemInstance(Tile::copperTrapdoor, 2), "##", "##", definition<ItemInstance>('#', ItemInstance(Item::copperIngot, 1)));
	}
	if (Tile::enderChest && Tile::obsidian && Item::emerald) {
		this->addShapedRecipe(ItemInstance(Tile::enderChest, 1), "###", "#D#", "###", definition<Tile*, Item*>('#', Tile::obsidian, 'D', Item::emerald));
	}
	this->addShapedRecipe(ItemInstance(Tile::fence_spruce, 3), "#X#", "#X#", definition<Item*, ItemInstance>('#', Item::stick, 'X', ItemInstance(Tile::wood, 1, 1)));
	this->addShapedRecipe(ItemInstance(Tile::fence_birch, 3), "#X#", "#X#", definition<Item*, ItemInstance>('#', Item::stick, 'X', ItemInstance(Tile::wood, 1, 2)));
	this->addShapedRecipe(ItemInstance(Item::sign, 1), "###", "###", " X ", definition<Tile*, Item*>('#', Tile::wood, 'X', Item::stick) /*inlined*/);
	std::vector<ItemInstance> cakestuff = {ItemInstance(Item::cake, 1), ItemInstance(Item::bucket, 3)};
	this->addShapedRecipe(cakestuff, Recipes::Shape("AAA", "BEB", "CCC").shape, definition<ItemInstance, Item*, Item*, Item*>('A', ItemInstance(Item::bucket, 1, 1), 'B', Item::sugar, 'C', Item::wheat, 'E', Item::egg));
	this->addShapedRecipe(ItemInstance(Item::sugar, 1), "#", definition<Item*>('#', Item::reeds));
	this->addShapedRecipe(ItemInstance(Tile::wood, 4, 0), "#", definition<ItemInstance>('#', ItemInstance(Tile::treeTrunk, 1, 0)));
	this->addShapedRecipe(ItemInstance(Tile::wood, 4, 1), "#", definition<ItemInstance>('#', ItemInstance(Tile::treeTrunk, 1, 1)));
	this->addShapedRecipe(ItemInstance(Tile::wood, 4, 2), "#", definition<ItemInstance>('#', ItemInstance(Tile::treeTrunk, 1, 2)));
	this->addShapedRecipe(ItemInstance(Tile::wood, 4, 3), "#", definition<ItemInstance>('#', ItemInstance(Tile::treeTrunk, 1, 3)));
	this->addShapedRecipe(ItemInstance(Item::stick, 4), "#", "#", definition<Tile*>('#', Tile::wood));
	this->addShapedRecipe(ItemInstance(Tile::torch, 4), "X", "#", definition<Item*, Item*>('X', Item::coal, '#', Item::stick));
	this->addShapedRecipe(ItemInstance(Tile::torch, 4), "X", "#", definition<ItemInstance, Item*>('X', ItemInstance(Item::coal, 1, 1), '#', Item::stick));
	this->addShapedRecipe(ItemInstance(Item::bowl, 4), "# #", " # ", definition<Tile*>('#', Tile::wood));
	this->addShapedRecipe(ItemInstance(Tile::rail, 16), "X X", "X#X", "X X", definition<Item*, Item*>('X', Item::ironIngot, '#', Item::stick));
	this->addShapedRecipe(ItemInstance(Tile::goldenRail, 6), "X X", "X#X", "XRX", definition<Item*, Item*, Item*>('X', Item::goldIngot, 'R', Item::redStone, '#', Item::stick));
	this->addShapedRecipe(ItemInstance(Item::minecart, 1), "# #", "###", definition<Item*>('#', Item::ironIngot));
	this->addShapedRecipe(ItemInstance(Tile::litPumpkin, 1), "A", "B", definition<Tile*, Tile*>('A', Tile::pumpkin, 'B', Tile::torch));
	this->addShapedRecipe(ItemInstance(Item::bucket, 1, 0), "# #", " # ", definition<Item*>('#', Item::ironIngot));
	this->addShapedRecipe(ItemInstance(Item::flintAndSteel, 1), "A ", " B", definition<Item*, Item*>('A', Item::ironIngot, 'B', Item::flint));
	this->addShapedRecipe(ItemInstance(Item::bread, 1), "###", definition<Item*>('#', Item::wheat));
	this->addShapedRecipe(ItemInstance(Tile::stairs_wood, 4, 0), "#  ", "## ", "###", definition<ItemInstance>('#', ItemInstance(Tile::wood, 1, 0)));
	this->addShapedRecipe(ItemInstance(Tile::woodStairsDark, 4), "#  ", "## ", "###", definition<ItemInstance>('#', ItemInstance(Tile::wood, 1, 1)));
	this->addShapedRecipe(ItemInstance(Tile::woodStairsBirch, 4), "#  ", "## ", "###", definition<ItemInstance>('#', ItemInstance(Tile::wood, 1, 2)));
	this->addShapedRecipe(ItemInstance(Tile::stairs_stone, 4), "#  ", "## ", "###", definition<Tile*>('#', Tile::stoneBrick));
	this->addShapedRecipe(ItemInstance(Tile::stairs_brick, 4), "#  ", "## ", "###", definition<Tile*>('#', (Tile*)Tile::redBrick));
	this->addShapedRecipe(ItemInstance(Tile::stairs_sandStone, 4), "#  ", "## ", "###", definition<Tile*>('#', Tile::sandStone));
	this->addShapedRecipe(ItemInstance(Tile::stairs_stoneBrickSmooth, 4), "#  ", "## ", "###", definition<Tile*>('#', Tile::stoneBrickSmooth));
	this->addShapedRecipe(ItemInstance(Tile::stairs_netherBricks, 4), "#  ", "## ", "###", definition<Tile*>('#', (Tile*)Tile::netherBrick));
	this->addShapedRecipe(ItemInstance(Item::painting, 1), "###", "#X#", "###", definition<Item*, Tile*>('#', Item::stick, 'X', Tile::cloth));
	this->addShapedRecipe(ItemInstance(Item::clock, 1), " # ", "#X#", " # ", definition<Item*, Item*>('#', Item::goldIngot, 'X', Item::redStone));
	this->addShapedRecipe(ItemInstance(Item::compass, 1), " # ", "#X#", " # ", definition<Item*, Item*>('#', Item::ironIngot, 88, Item::redStone));
	this->addShapedRecipe(ItemInstance(Item::bed, 1, 0), "###", "XXX", definition<ItemInstance, Tile*>('#', ItemInstance(Tile::cloth, 1, 0), 'X', Tile::wood));
	this->addShapedRecipe(ItemInstance(Tile::ironFence, 16), "###", "###", definition<Item *>('#', Item::ironIngot));
	this->addShapedRecipe(ItemInstance(Tile::netherReactor, 1), "X#X", "X#X", "X#X", definition<Item *,Item *>('#', Item::emerald, 'X', Item::ironIngot));
	this->addShapedRecipe(ItemInstance(Tile::woolCarpet, 3, 14), "##", definition<ItemInstance>('#', ItemInstance(Tile::cloth, 1, 14)));

	// this->addShapedRecipe(ItemInstance(Tile::fence_spruce, 3), "#X#", "#X#", definition<Item*, ItemInstance>('#', Item::stick, 'X', ItemInstance(Tile::wood, 1, 1)));
	// this->addShapedRecipe(ItemInstance(Tile::fence_birch, 3), "#X#", "#X#", definition<Item*, ItemInstance>('#', Item::stick, 'X', ItemInstance(Tile::wood, 1, 2)));
	// this->addShapedRecipe(ItemInstance(Tile::trapdoor_spruce, 2), "###", "###", definition<ItemInstance>('#', ItemInstance(Tile::wood, 1, 1)));
	// this->addShapedRecipe(ItemInstance(Tile::trapdoor_birch, 2), "###", "###", definition<ItemInstance>('#', ItemInstance(Tile::wood, 1, 2)));
	
	for (int i = 0; i < 16; i++) {
		if (i != 0) { // white bed is already added via Tile::cloth
			// this->addShapedRecipe(ItemInstance(Item::bed, 1, i), "###", "XXX", definition<ItemInstance, Tile*>('#', ItemInstance(Tile::cloth, 1, i), 'X', Tile::wood));
		}
	}
}
void Recipes::addShapedRecipe(const ItemInstance& a2, const std::string& a3, const std::string& a4, const std::string& a5, const std::vector<Recipes::Type>& a6) {
	Recipes::Shape v8(a3, a4, a5);
	this->addShapedRecipe(a2, v8.shape, a6);
}
void Recipes::addShapedRecipe(const ItemInstance& a2, const std::string& a3, const std::string& a4, const std::vector<Recipes::Type>& a5) {
	Recipes::Shape v7(a3, a4);
	this->addShapedRecipe(a2, v7.shape, a5);
}
void Recipes::addShapedRecipe(const ItemInstance& a2, const std::string& a3, const std::vector<Recipes::Type>& a4) {
	Recipes::Shape v7(a3);
	this->addShapedRecipe(a2, v7.shape, a4);
}
void Recipes::addShapedRecipe(const ItemInstance& a2, const std::vector<std::string> a3, const std::vector<Recipes::Type>& a4) { //TODO last arg type might be slightly different
	std::vector<ItemInstance> v7;
	v7.emplace_back(ItemInstance(a2));
	this->addShapedRecipe(v7, a3, a4); //TODO check
}
void Recipes::addShapedRecipe(const std::vector<ItemInstance>& results, const std::vector<std::string, std::allocator<std::string>> shape, const std::vector<Recipes::Type>& ingridients) { //TODO check signature
	if(shape.begin() != shape.end()) {
		std::string v39 = "";
		int32_t v8 = shape.size();
		int32_t v9 = shape[0].length();
		for(int32_t v7 = 0; v7 < shape.size(); ++v7) {
			v39 += shape[v7];
		}

		int32_t v10 = 0;
		std::map<char_t, ItemInstance> v43;
		while(1) {
			if(v10 >= ingridients.size()) {
				int32_t v20 = v8 * v9;
				ItemInstance* v22 = new ItemInstance[v20];

				for(int32_t i = 0; i < v20; ++i) {
					auto&& p = v43.find(v39[i]);
					if(p != v43.end()) { //TODO
						v22[i] = p->second;
					}
				}
				ShapedRecipe* v29 = new ShapedRecipe(v9, v8, v22, results);
				this->recipes.emplace_back(v29);
				return;
			}
			const Recipes::Type* v11 = &ingridients[v10];
			if(v11->item) {
				v43.insert({v11->chr, ItemInstance(v11->item)});
			} else if(v11->tile) {
				v43.insert({v11->chr, ItemInstance(v11->tile, 1, -1)});
			} else if(!v11->itemInstance.isNull()) {
				v43.insert({v11->chr, ItemInstance(v11->itemInstance)});
			}
			++v10;
		}
	}
}
void Recipes::addShapelessRecipe(const ItemInstance& a2, const std::vector<Recipes::Type>& a3) {
	std::vector<ItemInstance> v14;
	for(int32_t v3 = 0; v3 < a3.size(); ++v3) {
		const Recipes::Type* v7 = &a3[v3];
		if(v7->item) {
			v14.emplace_back(ItemInstance(v7->item));
		} else if(v7->tile) {
			v14.emplace_back(ItemInstance(v7->tile));
		} else if(!v7->itemInstance.isNull()) {
			v14.push_back(v7->itemInstance);
		}
	}
	std::vector<ItemInstance> a1;
	a1.push_back(a2);
	this->recipes.emplace_back(new ShapelessRecipe(a1, v14));
}
Recipes* Recipes::getInstance() {
	if(!Recipes::instance) {
		Recipes::instance = new Recipes();
	}
	return Recipes::instance;
}
Recipe* Recipes::getRecipeFor(const ItemInstance& a2) {
	for(uint32_t i = 0; i < this->recipes.size(); ++i) {
		Recipe* v5 = this->recipes[i];
		ItemInstance v10(v5->getResultItem()->at(0));
		if ( !a2.sameItem(&v10) )
		{
			if (a2.count == 0 )
			{
				if ( a2.getAuxValue() == v10.getAuxValue() )
				{
					return v5;
				}
			}
			if ( a2.count == v10.count )
			{
				if ( a2.getAuxValue() == v10.getAuxValue() )
				{
					return v5;
				}
			}
		}
	}
	return 0;
}
std::vector<Recipe*>* Recipes::getRecipes() { //TODO prob different type
	return &this->recipes;
}
void Recipes::teardownRecipes() {
	if(Recipes::instance) {
		delete Recipes::instance;
		Recipes::instance = 0;
	}
	FurnaceRecipes::teardownFurnaceRecipes();
}

Recipes::~Recipes() {
	for(int32_t i = 0; i < this->recipes.size(); ++i) {
		if(this->recipes[i]) {
			delete this->recipes[i];
		}
	}
}
