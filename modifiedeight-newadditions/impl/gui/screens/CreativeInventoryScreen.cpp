#include <gui/screens/CreativeInventoryScreen.hpp>
#include <gui/screens/ArmorScreen.hpp>
#include <Minecraft.hpp>
#include <entity/LocalPlayer.hpp>
#include <gui/buttons/CategoryButton.hpp>
#include <gui/buttons/ImageWithBackground.hpp>
#include <gui/pane/Touch_InventoryPane.hpp>
#include <input/Mouse.hpp>
#include <inventory/Inventory.hpp>
#include <item/Item.hpp>
#include <item/ItemInstance.hpp>
#include <rendering/entity/ItemRenderer.hpp>
#include <tile/Tile.hpp>
#include <rendering/Tesselator.hpp>
#include <gui/NinePatchFactory.hpp>

std::vector<ItemInstance> CreativeInventoryScreen::items;
std::vector<ItemInstance> CreativeInventoryScreen::filteredItems[CreativeInventoryScreen::NUM_TABS];

CreativeInventoryScreen::TabButtonWithMeta::TabButtonWithMeta(int f0, std::shared_ptr<ImageButton> f4)
	: field_0(f0), field_4(std::move(f4)) {
}
CreativeInventoryScreen::TabButtonWithMeta::TabButtonWithMeta(const CreativeInventoryScreen::TabButtonWithMeta& a2)
	: field_0(a2.field_0), field_4(a2.field_4) {
}
CreativeInventoryScreen::TabButtonWithMeta::TabButtonWithMeta(CreativeInventoryScreen::TabButtonWithMeta&& a2)
	: field_0(a2.field_0), field_4(std::move(a2.field_4)) {
}
CreativeInventoryScreen::TabButtonWithMeta::~TabButtonWithMeta() {
}
CreativeInventoryScreen::CreativeInventoryScreen() {
	this->field_58 = 24;
	this->field_5C = 2;
	this->field_A8 = 0;
	this->field_AC = 0;
	this->field_B0 = 0;
	this->currentPaneMaybe = 0;
	this->field_B8 = 0;
	this->field_BC = 0;
}
void CreativeInventoryScreen::_putItemInToolbar(const ItemInstance*) {
}
void CreativeInventoryScreen::closeWindow() {
	this->minecraft->setScreen(0);
}
std::shared_ptr<ImageButton> CreativeInventoryScreen::createInventoryTabButton(int32_t a3, int32_t a4, bool isRightSide) {
	NinePatchLayer* patch = isRightSide ? this->rightTabPatch.get() : this->field_70.get();
	std::shared_ptr<ImageButton> res(new CategoryButton(a3, patch, patch, &this->field_A4));
	res->height = this->field_58;
	res->width = this->field_58;
	res->setOverrideScreenRendering(1);
	return res;
}
void CreativeInventoryScreen::drawIcon(int a2, std::shared_ptr<ImageButton> a3, bool_t a4, bool_t a5) {
	ItemInstance v14 = this->getItemFromType(a2);
	float v8 = 0.0;
	int v9 = this->field_58;
	float v10 = (float)v9;
	if(a5) {
		v8 = 2.0;
	}
	float v13 = (float)(v10 - v8) / 25.0;
	float v11 = (float)((float)a3->posY + (float)(v10 * 0.5)) - 8.0;
	float v12 = 0.7;
	if(!a4) {
		v12 = 1.0;
	}
	ItemRenderer::renderGuiItemNew(this->minecraft->texturesPtr, &v14, 0, (float)(a3->posX + v9 / 2 - 8) + 1.0, v11, 1.0, v12, v13);
}
int32_t CreativeInventoryScreen::getCategoryFromPanel(const Touch::InventoryPane* a2) {
	for(size_t v2 = 0; v2 < this->field_98.size(); ++v2) {
		if(this->field_78[v2].get() == a2) {
			int meta = this->field_98[v2].field_0;
			switch(meta) {
				case 5: return 5;
				case 6: return 6;
				case 8: return 2;
				case 9: return 3;
				case 4: return 4;
				case 7: return 7;
				case 3: return 0;
				case 2: return 1;
				case 1: return 8;
				default: return 8;
			}
		}
	}
	return 0;
}
ItemInstance CreativeInventoryScreen::getItemFromType(int32_t a3) {
	switch(a3) {
		case 1:
			return Tile::redBrick ? ItemInstance(Tile::redBrick) : ItemInstance();
		case 2:
			return Tile::bookshelf ? ItemInstance(Tile::bookshelf) : ItemInstance();
		case 3:
			return Item::sword_iron ? ItemInstance(Item::sword_iron) : ItemInstance();
		case 4:
			return Item::seeds_wheat ? ItemInstance(Item::seeds_wheat) : ItemInstance();
		case 5:
			return Tile::flowerOrchid ? ItemInstance(Tile::flowerOrchid) : (Tile::rose ? ItemInstance(Tile::rose) : ItemInstance());
		case 6:
			return Tile::redstoneLampOff ? ItemInstance(Tile::redstoneLampOff) : (Tile::lever ? ItemInstance(Tile::lever) : ItemInstance());
		case 7:
			return Item::mobPlacer ? ItemInstance(Item::mobPlacer, 1, 33) : ItemInstance();
		case 8:
			return Tile::cloth ? ItemInstance(Tile::cloth, 1, 11) : ItemInstance();
		case 9:
			return Tile::glowingObsidian ? ItemInstance(Tile::glowingObsidian) : (Tile::netherReactor ? ItemInstance(Tile::netherReactor) : ItemInstance());
		default:
			return Tile::redBrick ? ItemInstance(Tile::redBrick) : ItemInstance();
	}
}

static void addTabItem(int tab, Tile* tile, int count = 1, int aux = 0) {
	if(tile && tab >= 0 && tab < CreativeInventoryScreen::NUM_TABS) {
		CreativeInventoryScreen::filteredItems[tab].emplace_back(ItemInstance(tile, count, aux));
	}
}
static void addTabItem(int tab, Item* item, int count = 1, int aux = 0) {
	if(item && tab >= 0 && tab < CreativeInventoryScreen::NUM_TABS) {
		CreativeInventoryScreen::filteredItems[tab].emplace_back(ItemInstance(item, count, aux));
	}
}

void CreativeInventoryScreen::populateFilteredItems() {
	for(int i = 0; i < NUM_TABS; ++i) {
		CreativeInventoryScreen::filteredItems[i].clear();
	}

	// TAB 0: Building Blocks
	addTabItem(8, Tile::rock);
	addTabItem(8, Tile::stoneBrick);
	addTabItem(8, Tile::mossStone);
	addTabItem(8, Tile::stoneBrickSmooth, 1, 0);
	addTabItem(8, Tile::stoneBrickSmooth, 1, 1);
	addTabItem(8, Tile::stoneBrickSmooth, 1, 2);
	addTabItem(8, Tile::sandStone, 1, 0);
	addTabItem(8, Tile::sandStone, 1, 1);
	addTabItem(8, Tile::sandStone, 1, 2);
	addTabItem(8, Tile::wood, 1, 0);
	addTabItem(8, Tile::wood, 1, 1);
	addTabItem(8, Tile::wood, 1, 2);
	addTabItem(8, Tile::wood, 1, 3);
	addTabItem(8, Tile::treeTrunk, 1, 0);
	addTabItem(8, Tile::treeTrunk, 1, 1);
	addTabItem(8, Tile::treeTrunk, 1, 2);
	addTabItem(8, Tile::treeTrunk, 1, 3);
	addTabItem(8, Tile::dirt);
	addTabItem(8, Tile::grass);
	addTabItem(8, Tile::clay);
	addTabItem(8, Tile::sand);
	addTabItem(8, Tile::gravel);
	addTabItem(8, Tile::redBrick);
	addTabItem(8, Tile::netherBrick);
	addTabItem(8, Tile::netherrack);
	addTabItem(8, Tile::obsidian);
	addTabItem(8, Tile::soulSand);
	addTabItem(8, Tile::ice);
	addTabItem(8, Tile::snow);
	addTabItem(8, Tile::topSnow);
	addTabItem(8, Tile::sponge);
	addTabItem(8, Tile::grassPath);
	addTabItem(8, Tile::grassSlabHalf);
	addTabItem(8, Tile::slimeBlock);
	addTabItem(8, Tile::unbreakable);
	addTabItem(8, Tile::quartzBlock, 1, 0);
	addTabItem(8, Tile::quartzBlock, 1, 1);
	addTabItem(8, Tile::quartzBlock, 1, 2);
	addTabItem(8, Tile::coalOre);
	addTabItem(8, Tile::ironOre);
	addTabItem(8, Tile::goldOre);
	addTabItem(8, Tile::copperOre);
	addTabItem(8, Tile::emeraldOre);
	addTabItem(8, Tile::lapisOre);
	addTabItem(8, Tile::redStoneOre);
	addTabItem(8, Tile::netherQuartz);
	addTabItem(8, Tile::coalBlock);
	addTabItem(8, Tile::ironBlock);
	addTabItem(8, Tile::goldBlock);
	addTabItem(8, Tile::copperBlock);
	addTabItem(8, Tile::emeraldBlock);
	addTabItem(8, Tile::lapisBlock);
	addTabItem(8, Tile::stoneSlabHalf, 1, 0);
	addTabItem(8, Tile::stoneSlabHalf, 1, 1);
	addTabItem(8, Tile::stoneSlabHalf, 1, 3);
	addTabItem(8, Tile::stoneSlabHalf, 1, 4);
	addTabItem(8, Tile::stoneSlabHalf, 1, 5);
	addTabItem(8, Tile::stoneSlabHalf, 1, 6);
	addTabItem(8, Tile::woodSlabHalf, 1, 0);
	addTabItem(8, Tile::woodSlabHalf, 1, 1);
	addTabItem(8, Tile::woodSlabHalf, 1, 2);
	addTabItem(8, Tile::woodSlabHalf, 1, 3);
	addTabItem(8, Tile::dirtSlabHalf);
	addTabItem(8, Tile::rockSlabHalf);
	addTabItem(8, Tile::copperSlabHalf);
	addTabItem(8, Tile::stairs_stone);
	addTabItem(8, Tile::stairs_wood);
	addTabItem(8, Tile::woodStairsDark);
	addTabItem(8, Tile::woodStairsBirch);
	addTabItem(8, Tile::woodStairsJungle);
	addTabItem(8, Tile::stairs_brick);
	addTabItem(8, Tile::stairs_sandStone);
	addTabItem(8, Tile::stairs_stoneBrickSmooth);
	addTabItem(8, Tile::stairs_netherBricks);
	addTabItem(8, Tile::stairs_quartz);
	addTabItem(8, Tile::copperStairs);

	// TAB 1: Decorative Blocks & Doors
	addTabItem(7, Tile::bookshelf);
	addTabItem(7, Tile::torch);
	addTabItem(7, Tile::ladder);
	addTabItem(7, Tile::glass);
	addTabItem(7, Tile::thinGlass);
	addTabItem(7, Tile::fence);
	addTabItem(7, Tile::fence_spruce);
	addTabItem(7, Tile::fence_birch);
	addTabItem(7, Tile::fenceGate);
	addTabItem(7, Tile::ironFence);
	addTabItem(7, Tile::copperFence);
	addTabItem(7, Tile::cobbleWall, 1, 0);
	addTabItem(7, Tile::cobbleWall, 1, 1);
	addTabItem(7, Tile::stoneWall);
	addTabItem(7, Tile::copperWall);
	addTabItem(7, Tile::chest);
	addTabItem(7, Tile::enderChest);
	addTabItem(7, Tile::workBench);
	addTabItem(7, Tile::furnace);
	addTabItem(7, Tile::stonecutterBench);
	addTabItem(7, Tile::musicPlayer);
	addTabItem(7, Item::itemFrame);
	addTabItem(7, Item::armorStand);
	addTabItem(7, Item::painting);
	addTabItem(7, Item::sign);
	addTabItem(7, Item::door_wood);
	addTabItem(7, Item::door_spruce);
	addTabItem(7, Item::door_birch);
	addTabItem(7, Item::door_jungle);
	addTabItem(7, Item::door_iron);
	addTabItem(7, Item::copperDoor);
	addTabItem(7, Tile::trapdoor);
	addTabItem(7, Tile::trapdoor_spruce);
	addTabItem(7, Tile::trapdoor_birch);
	addTabItem(7, Tile::ironTrapdoor);
	addTabItem(7, Tile::copperTrapdoor);
	addTabItem(7, Tile::web);
	addTabItem(7, Item::flowerPot);

	// TAB 2: Tools, Weapons & Armor (strictly tools, armor, bow, flint&steel, clock, compass, fishing rod, shears)
	addTabItem(6, Item::sword_wood);
	addTabItem(6, Item::sword_stone);
	addTabItem(6, Item::sword_iron);
	addTabItem(6, Item::sword_gold);
	addTabItem(6, Item::sword_emerald);
	addTabItem(6, Item::shovel_wood);
	addTabItem(6, Item::shovel_stone);
	addTabItem(6, Item::shovel_iron);
	addTabItem(6, Item::shovel_gold);
	addTabItem(6, Item::shovel_emerald);
	addTabItem(6, Item::pickAxe_wood);
	addTabItem(6, Item::pickAxe_stone);
	addTabItem(6, Item::pickAxe_iron);
	addTabItem(6, Item::pickAxe_gold);
	addTabItem(6, Item::pickAxe_emerald);
	addTabItem(6, Item::hatchet_wood);
	addTabItem(6, Item::hatchet_stone);
	addTabItem(6, Item::hatchet_iron);
	addTabItem(6, Item::hatchet_gold);
	addTabItem(6, Item::hatchet_emerald);
	addTabItem(6, Item::hoe_wood);
	addTabItem(6, Item::hoe_stone);
	addTabItem(6, Item::hoe_iron);
	addTabItem(6, Item::hoe_gold);
	addTabItem(6, Item::hoe_emerald);
	addTabItem(6, Item::helmet_cloth);
	addTabItem(6, Item::chestplate_cloth);
	addTabItem(6, Item::leggings_cloth);
	addTabItem(6, Item::boots_cloth);
	addTabItem(6, Item::helmet_chain);
	addTabItem(6, Item::chestplate_chain);
	addTabItem(6, Item::leggings_chain);
	addTabItem(6, Item::boots_chain);
	addTabItem(6, Item::helmet_iron);
	addTabItem(6, Item::chestplate_iron);
	addTabItem(6, Item::leggings_iron);
	addTabItem(6, Item::boots_iron);
	addTabItem(6, Item::helmet_gold);
	addTabItem(6, Item::chestplate_gold);
	addTabItem(6, Item::leggings_gold);
	addTabItem(6, Item::boots_gold);
	addTabItem(6, Item::helmet_diamond);
	addTabItem(6, Item::chestplate_diamond);
	addTabItem(6, Item::leggings_diamond);
	addTabItem(6, Item::boots_diamond);
	addTabItem(6, Item::bow);
	addTabItem(6, Item::arrow);
	addTabItem(6, Item::flintAndSteel);
	addTabItem(6, Item::clock);
	addTabItem(6, Item::compass);
	addTabItem(6, Item::fishingRod);
	addTabItem(6, Item::shears);
	addTabItem(6, Item::bucket, 1, 0);
	if(Tile::water) addTabItem(6, Item::bucket, 1, Tile::water->blockID);
	if(Tile::lava) addTabItem(6, Item::bucket, 1, Tile::lava->blockID);
	addTabItem(6, Item::bucket, 1, 1);

	// TAB 3: Spawn Eggs (Left side directly below Sword)
	for(int eggId : {10, 11, 12, 13, 14, 15, 17, 22, 26, 27, 28, 29, 30, 32, 33, 34, 35, 36, 37, 38, 39, 40, 120}) {
		addTabItem(5, Item::mobPlacer, 1, eggId);
	}

	// TAB 4: Materials & Items (Wheat seeds icon)
	addTabItem(4, Item::coal, 1, 0);
	addTabItem(4, Item::coal, 1, 1);
	addTabItem(4, Item::ironIngot);
	addTabItem(4, Item::goldIngot);
	addTabItem(4, Item::copperIngot);
	addTabItem(4, Item::emerald);
	addTabItem(4, Item::netherQuartz);
	addTabItem(4, Item::stick);
	addTabItem(4, Item::flint);
	addTabItem(4, Item::clay);
	addTabItem(4, Item::brick);
	addTabItem(4, Item::netherbrick);
	addTabItem(4, Item::feather);
	addTabItem(4, Item::leather);
	addTabItem(4, Item::string);
	addTabItem(4, Item::slimeBall);
	addTabItem(4, Item::bone);
	addTabItem(4, Item::sulphur);
	addTabItem(4, Item::paper);
	addTabItem(4, Item::book);
	for(int i = 0; i < 16; ++i) {
		addTabItem(4, Item::dye_powder, 1, i);
	}
	addTabItem(4, Item::seeds_wheat);
	addTabItem(4, Item::seeds_pumpkin);
	addTabItem(4, Item::seeds_melon);
	addTabItem(4, Item::seeds_beetroot);
	addTabItem(4, Item::wheat);
	addTabItem(4, Item::reeds);
	addTabItem(4, Item::sugar);
	addTabItem(4, Item::egg);
	addTabItem(4, Item::apple);
	addTabItem(4, Item::carrot);
	addTabItem(4, Item::potato);
	addTabItem(4, Item::potatoBaked);
	addTabItem(4, Item::beetroot);
	addTabItem(4, Item::pumpkinPie);
	addTabItem(4, Item::bread);
	addTabItem(4, Tile::cake);
	addTabItem(4, Item::sweetBerries);
	addTabItem(4, Item::bowl);
	addTabItem(4, Item::mushroomStew);
	addTabItem(4, Item::beetrootSoup);
	addTabItem(4, Item::porkChop_raw);
	addTabItem(4, Item::porkChop_cooked);
	addTabItem(4, Item::beef_raw);
	addTabItem(4, Item::beef_cooked);
	addTabItem(4, Item::chicken_raw);
	addTabItem(4, Item::chicken_cooked);
	addTabItem(4, Item::fish_raw);
	addTabItem(4, Item::fish_cooked);
	addTabItem(4, Item::salmon_raw);
	addTabItem(4, Item::salmon_cooked);
	addTabItem(4, Item::clownfish);
	addTabItem(4, Item::pufferfish);
	addTabItem(4, Item::minecart);
	addTabItem(4, Item::boat, 1, 0);
	addTabItem(4, Item::boat, 1, 1);
	addTabItem(4, Item::boat, 1, 2);
	addTabItem(4, Item::boat, 1, 3);
	addTabItem(4, Item::saddle);

	// TAB 5: Vegetation (Rose icon - Right side below armor menu)
	for(int j = 0; j < 4; ++j) {
		addTabItem(0, Tile::leaves, 1, j);
		addTabItem(0, Tile::sapling, 1, j);
	}
	addTabItem(0, Tile::deadBush);
	addTabItem(0, Tile::tallgrass, 1, 1);
	addTabItem(0, Tile::tallgrass, 1, 2);
	addTabItem(0, Tile::flower);
	addTabItem(0, Tile::rose);
	addTabItem(0, Tile::flowerRose);
	addTabItem(0, Tile::flowerOrchid);
	addTabItem(0, Tile::flowerAllium);
	addTabItem(0, Tile::flowerHoustonia);
	addTabItem(0, Tile::flowerDaisy);
	addTabItem(0, Tile::flowerPaeonia);
	for(int d = 0; d < 6; ++d) {
		addTabItem(0, Tile::doublePlant, 1, d);
	}
	addTabItem(0, Tile::sweetBerryBush);
	addTabItem(0, Tile::seagrass);
	addTabItem(0, Tile::waterLily);
	addTabItem(0, Tile::vine);
	addTabItem(0, Tile::cactus);
	addTabItem(0, Tile::mushroom1);
	addTabItem(0, Tile::mushroom2);
	addTabItem(0, Tile::pumpkin);
	addTabItem(0, Tile::litPumpkin);
	addTabItem(0, Tile::melon);
	addTabItem(0, Tile::hayBlock);

	// TAB 6: Redstone / Mechanisms (Lever icon)
	addTabItem(1, Tile::lever);
	addTabItem(1, Tile::button_stone);
	addTabItem(1, Tile::button_wood);
	addTabItem(1, Tile::button_spruce);
	addTabItem(1, Tile::button_birch);
	addTabItem(1, Tile::button_jungle);
	addTabItem(1, Tile::button_cobblestone);
	addTabItem(1, Tile::button_gold);
	addTabItem(1, Tile::button_iron);
	addTabItem(1, Tile::pressurePlateStone);
	addTabItem(1, Tile::pressurePlatePlanks);
	addTabItem(1, Tile::pressurePlate_spruce);
	addTabItem(1, Tile::pressurePlate_birch);
	addTabItem(1, Tile::pressurePlate_jungle);
	addTabItem(1, Tile::pressurePlate_cobblestone);
	addTabItem(1, Tile::pressurePlate_gold);
	addTabItem(1, Tile::pressurePlate_iron);
	addTabItem(1, Tile::daylightDetector);
	addTabItem(1, Tile::rail);
	addTabItem(1, Tile::goldenRail);
	addTabItem(1, Tile::redstoneLampOff);
	addTabItem(1, Tile::tnt);

	// TAB 7: Colored Blocks (Blue wool icon)
	for(int c = 0; c < 16; ++c) {
		addTabItem(2, Tile::cloth, 1, c);
	}
	for(int c = 0; c < 16; ++c) {
		addTabItem(2, Tile::woolCarpet, 1, c);
	}
	for(int c = 0; c < 16; ++c) {
		addTabItem(2, Item::bed, 1, c);
	}
	for(int c = 0; c < 16; ++c) {
		addTabItem(2, Tile::stainedGlass, 1, c);
	}
	for(int c = 0; c < 16; ++c) {
		addTabItem(2, Tile::stainedGlassPane, 1, c);
	}

	// TAB 8: Advanced / Extended Blocks (Activated Nether Portal icon)
	addTabItem(3, Tile::glowingObsidian);
	addTabItem(3, Tile::netherReactor);
	addTabItem(3, Tile::netherReactor, 1, 1);
	addTabItem(3, Tile::netherReactor, 1, 2);
	addTabItem(3, Tile::info_updateGame1);
	addTabItem(3, Tile::info_updateGame2);
	addTabItem(3, Item::camera);
}
void CreativeInventoryScreen::populateItem(Item* a1, int32_t a2, int32_t a3) {
	if(a1) {
		CreativeInventoryScreen::items.emplace_back(ItemInstance(a1, a2, a3));
	}
}
void CreativeInventoryScreen::populateItem(Tile* a1, int32_t a2, int32_t a3) {
	if(a1) {
		CreativeInventoryScreen::items.emplace_back(ItemInstance(a1, a2, a3));
	}
}
void CreativeInventoryScreen::populateItems() {
	CreativeInventoryScreen::items.clear();
}

CreativeInventoryScreen::~CreativeInventoryScreen() {
}

void CreativeInventoryScreen::render(int32_t a2, int32_t a3, float a4) {
	this->renderBackground(0);
	this->minecraft->gui.renderToolBar(a4, 1);
	static Color4 _D6E05DF0(0.75, 0.75, 0.75, 1.0);
	for(const auto& v27: this->field_98) {
		if(v27.field_4.get() != this->field_A4) {
			v27.field_4->color = _D6E05DF0;
			v27.field_4->render(this->minecraft, a2, a3);
			this->drawIcon(v27.field_0, v27.field_4, 1, v27.field_4->pressed);
		}
	}
	glColor4f(1.0, 1.0, 1.0, 1.0);
	this->field_68->draw(Tesselator::instance, this->field_A8, this->field_AC);
	std::shared_ptr<Touch::InventoryPane> v25 = this->field_78[this->currentPaneMaybe];
	this->fill(v25->field_228.minX - this->field_B8 - v25->field_248, v25->field_228.minY - v25->field_24C, v25->field_228.minX - v25->field_248, v25->field_228.height + v25->field_228.minY + v25->field_24C, 0xFF333333);
	int v15 = v25->field_228.minX + v25->field_228.width + v25->field_248;
	this->fill(v15, v25->field_228.minY - v25->field_24C, v15 + this->field_B8, v25->field_228.minY + v25->field_228.height + v25->field_24C, 0xFF333333);
	v25->render(a2, a3, a4);
	Screen::render(a2, a3, a4);
	if (this->armorButton) {
		float v8 = 0.0f;
		int v9 = this->field_58;
		float v10 = (float)v9;
		if (this->armorButton->pressed) {
			v8 = 2.0f;
		}
		float v13 = (float)(v10 - v8) / 25.0f;
		float v11 = (float)((float)this->armorButton->posY + (float)(v10 * 0.5f)) - 8.0f;
		ItemInstance chestplate(Item::chestplate_iron);
		ItemRenderer::renderGuiItemNew(this->minecraft->texturesPtr, &chestplate, 0, (float)(this->armorButton->posX + v9 / 2 - 8) + 1.0f, v11, 1.0f, 1.0f, v13);
	}
	for(const auto& v27: this->field_98) {
		if(v27.field_4.get() == this->field_A4) {
			v27.field_4->color = Color4::WHITE;
			v27.field_4->render(this->minecraft, a2, a3);
			this->drawIcon(v27.field_0, v27.field_4, 0, v27.field_4->pressed);
		}
	}
	this->minecraft->gui.renderOnSelectItemNameText(this->width, this->minecraft->font, this->height - 19);
}
void CreativeInventoryScreen::init()
{
	CreativeInventoryScreen::items.clear();
	for (int i = 0; i < NUM_TABS; i++) {
		this->field_78[i].reset();
		CreativeInventoryScreen::filteredItems[i].clear();
	}
	CreativeInventoryScreen::populateFilteredItems();

	NinePatchFactory v16(this->minecraft->texturesPtr, "gui/spritesheet.png");
	this->field_68 = std::shared_ptr<NinePatchLayer>(v16.createSymmetrical(IntRectangle{34, 43, 14, 14}, 3, 3, 14, 14));
	int max_buttons_side = 6;
	int v4 = (height - 25) / max_buttons_side - this->field_5C;
	if (v4 >= 26) {
		v4 = 26;
	}
	if (v4 < 18) {
		v4 = 18;
	}
	this->field_58 = v4;
	IntRectangle a5 = {this->minecraft->options.leftHanded ? 65 : 49, this->minecraft->options.leftHanded ? 55 : 43, 14, 14};
	IntRectangle a5_right = {this->minecraft->options.leftHanded ? 49 : 65, this->minecraft->options.leftHanded ? 43 : 55, 14, 14};
	this->field_70 = std::shared_ptr<NinePatchLayer>(v16.createSymmetrical(a5, 3, 3, v4, v4));
	this->rightTabPatch = std::shared_ptr<NinePatchLayer>(v16.createSymmetrical(a5_right, 3, 3, v4, v4));
	this->field_98.clear();
	this->field_98.emplace_back(CreativeInventoryScreen::TabButtonWithMeta(5, this->createInventoryTabButton(10, 5, false)));
	this->field_98.emplace_back(CreativeInventoryScreen::TabButtonWithMeta(6, this->createInventoryTabButton(11, 6, false)));
	this->field_98.emplace_back(CreativeInventoryScreen::TabButtonWithMeta(8, this->createInventoryTabButton(14, 8, false)));
	if (this->minecraft->options.extendedInventory) {
		this->field_98.emplace_back(CreativeInventoryScreen::TabButtonWithMeta(9, this->createInventoryTabButton(15, 9, false)));
	}
	this->field_98.emplace_back(CreativeInventoryScreen::TabButtonWithMeta(4, this->createInventoryTabButton(9, 4, true)));
	this->field_98.emplace_back(CreativeInventoryScreen::TabButtonWithMeta(7, this->createInventoryTabButton(13, 7, true)));
	this->field_98.emplace_back(CreativeInventoryScreen::TabButtonWithMeta(3, this->createInventoryTabButton(8, 3, true)));
	this->field_98.emplace_back(CreativeInventoryScreen::TabButtonWithMeta(2, this->createInventoryTabButton(7, 2, true)));
	this->field_98.emplace_back(CreativeInventoryScreen::TabButtonWithMeta(1, this->createInventoryTabButton(6, 1, true)));

	ImageDef v18;
	v18.field_0 = "gui/spritesheet.png";
	v18.field_4 = 0;
	v18.field_8 = 1;
	v18.height = 18.0;
	v18.width = 18.0;
	v18.field_24 = 1;
	v18.field_14 = 60;
	v18.field_18 = 0;
	v18.field_1C = 18;
	v18.field_20 = 18;

	ImageWithBackground* v5 = new ImageWithBackground(5);
	v5->init(this->minecraft->texturesPtr, this->field_58, this->field_58, a5, a5, 2, 2, "gui/spritesheet.png");
	v5->width = this->field_58;
	v5->height = this->field_58 - 1;
	v5->setImageDef(v18, 0);
	this->field_60 = std::shared_ptr<ImageWithBackground>(v5);

	ImageWithBackground* vArmor = new ImageWithBackground(12);
	vArmor->init(this->minecraft->texturesPtr, this->field_58, this->field_58, a5_right, a5_right, 2, 2, "gui/spritesheet.png");
	vArmor->width = this->field_58;
	vArmor->height = this->field_58 - 1;
	this->armorButton = std::shared_ptr<ImageWithBackground>(vArmor);

	this->field_A4 = this->field_98.back().field_4.get();
	this->currentPaneMaybe = (int)this->field_98.size() - 1;
	this->buttons.clear();
	for(const auto& p: this->field_98) {
		this->buttons.emplace_back(p.field_4.get());
	}
	this->buttons.emplace_back(this->field_60.get());
	this->buttons.emplace_back(this->armorButton.get());
	this->field_BC = 1;
}
void CreativeInventoryScreen::setupPositions()
{
	this->field_68->setSize((float)((float)this->width - 4.0) - (float)this->field_58 * 2.0f, (float)this->height - 25.0);
	int v3 = this->field_58 + 2;
	this->field_AC = 2;
	this->field_A8 = v3;
	this->field_B0 = 0;

	float leftX;
	float rightX;
	if(this->minecraft->options.leftHanded) {
		leftX = (float)((float)v3 + this->field_68->width2) + 2.0f;
		rightX = 2.0f;
	} else {
		leftX = (float)(v3 - this->field_58 + 3);
		rightX = (float)((float)v3 + this->field_68->width2) + 2.0f;
	}
	this->field_60->posX = (int)leftX;
	this->field_60->posY = this->field_AC;
	this->field_60->width = this->field_58;
	this->field_60->height = this->field_58 - 1;

	if (this->armorButton) {
		this->armorButton->posX = (int)(rightX - 3.0f);
		this->armorButton->posY = this->field_AC;
		this->armorButton->width = this->field_58;
		this->armorButton->height = this->field_58 - 1;
	}

	int leftCount = this->minecraft->options.extendedInventory ? 4 : 3;
	for(int i = 0; i < leftCount && i < (int)this->field_98.size(); ++i) {
		this->field_98[i].field_4->posX = (int)leftX;
		this->field_98[i].field_4->posY = this->field_AC + this->field_58 + this->field_5C + i * (this->field_58 + this->field_5C);
		this->field_98[i].field_4->width = this->field_58;
		this->field_98[i].field_4->height = this->field_58;
	}

	for(size_t j = leftCount; j < this->field_98.size(); ++j) {
		this->field_98[j].field_4->posX = (int)(rightX - 3.0f);
		this->field_98[j].field_4->posY = this->field_AC + this->field_58 + this->field_5C + (int)(j - leftCount) * (this->field_58 + this->field_5C);
		this->field_98[j].field_4->width = this->field_58;
		this->field_98[j].field_4->height = this->field_58;
	}

	if(!this->field_78[0].get()) {
		int v14 = (int)this->field_68->width2 - 14;
		int v15 = this->field_A8;
		IntRectangle r3_0;
		r3_0.minY = this->field_AC + 8;
		r3_0.width = 26 * (v14 / 26);
		this->field_B8 = v14 % 26 / 2;
		int height2 = (int)this->field_68->height2;
		r3_0.minX = v14 % 26 / 2 + v15 + 7;
		r3_0.height = height2 - 16;

		for (size_t i = 0; i < this->field_98.size(); ++i) {
			int meta = this->field_98[i].field_0;
			int cat = 8;
			switch(meta) {
				case 5: cat = 5; break;
				case 6: cat = 6; break;
				case 8: cat = 2; break;
				case 9: cat = 3; break;
				case 4: cat = 4; break;
				case 7: cat = 7; break;
				case 3: cat = 0; break;
				case 2: cat = 1; break;
				case 1: cat = 8; break;
				default: cat = 8; break;
			}
			int count = CreativeInventoryScreen::filteredItems[cat].size();
			this->field_78[i] = std::shared_ptr<Touch::InventoryPane>(
				new Touch::InventoryPane(this, this->minecraft, r3_0, r3_0.width, 1, count, 26, 1, 0)
			);
		}
	}
}
bool_t CreativeInventoryScreen::handleBackEvent(bool_t a2) {
	if(!a2) {
		this->closeWindow();
	}
	return 1;
}
void CreativeInventoryScreen::tick() {
	if (this->currentPaneMaybe >= 0 && this->currentPaneMaybe < NUM_TABS && this->field_78[this->currentPaneMaybe]) {
		this->field_78[this->currentPaneMaybe]->tick();
	}
}
bool_t CreativeInventoryScreen::renderGameBehind() {
	return this->minecraft->options.graphics;
}
void CreativeInventoryScreen::buttonClicked(Button* a2) {
	if(a2 == this->field_60.get()) {
		this->closeWindow();
	} else if(a2 == this->armorButton.get() || a2->buttonID == 12) {
		this->minecraft->setScreen(new ArmorScreen());
	} else {
		for (size_t i = 0; i < this->field_98.size(); ++i) {
			if (a2 == this->field_98[i].field_4.get()) {
				this->field_A4 = a2;
				this->currentPaneMaybe = (int)i;
				this->setupPositions();
				return;
			}
		}
	}
}
void CreativeInventoryScreen::mouseClicked(int32_t a2, int32_t a3, int32_t a4) {
	if (a4 == 4 || a4 == 5) {
		if (this->currentPaneMaybe >= 0 && this->currentPaneMaybe < NUM_TABS && this->field_78[this->currentPaneMaybe]) {
			auto pane = this->field_78[this->currentPaneMaybe];
			pane->adjustContentSize();
			float delta = (a4 == 4) ? 26.0f : -26.0f;
			float newY = pane->contentOffset()->y + delta;
			if (newY > 0.0f) newY = 0.0f;
			if (newY < (float)pane->field_12C) newY = (float)pane->field_12C;
			pane->setContentOffset(pane->contentOffset()->x, newY);
		}
		return;
	}
	Screen::mouseClicked(a2, a3, a4);
	this->minecraft->gui.handleClick(1, Mouse::getX(), Mouse::getY());
}
void CreativeInventoryScreen::mouseReleased(int32_t a2, int32_t a3, int32_t a4) {
	Screen::mouseReleased(a2, a3, a4);
	for(auto&& p: this->field_98) {
		p.field_4->released(a2, a3);
	}
}
bool_t CreativeInventoryScreen::addItem(const Touch::InventoryPane* a2, int32_t a3) {
	std::vector<ItemInstance>* v7 = &filteredItems[this->getCategoryFromPanel(a2)];
	if(a3 >= v7->size()) {
		return 0;
	}
	const ItemInstance* v8 = &v7->at(a3);
	int32_t id = v8->getId();
	int32_t auxv = v8->getAuxValue();
	int32_t slot = this->minecraft->player->inventory->getLinkedSlotForItemAndAux(id, auxv);
	if(slot < 0 || slot >= this->minecraft->gui.getNumSlots() - 1) {
		ItemInstance v15(*v8);
		int32_t v14 = this->minecraft->player->inventory->selectedSlot + 9;
		this->minecraft->player->inventory->setItem(v14, &v15);
		this->minecraft->player->inventory->linkSlot(this->minecraft->player->inventory->selectedSlot, v14);
		this->minecraft->player->inventory->setItem(this->minecraft->player->inventory->selectedSlot, &v15);
	} else {
		this->minecraft->player->inventory->selectedSlot = slot;
	}
	this->minecraft->gui.resetItemNameOverlay();
	this->minecraft->gui.flashSlot(this->minecraft->player->inventory->selectedSlot);
	return 1;
}
bool_t CreativeInventoryScreen::isAllowed(int32_t) {
	return 1;
}
std::vector<const ItemInstance*> CreativeInventoryScreen::getItems(const Touch::InventoryPane* a3) {
	int32_t cat = this->getCategoryFromPanel(a3);
	int32_t v5 = CreativeInventoryScreen::filteredItems[cat].size();
	std::vector<const ItemInstance*> vec(v5);
	for(int32_t i = 0; i < v5; ++i) {
		vec[i] = &CreativeInventoryScreen::filteredItems[cat][i];
	}
	return vec;
}