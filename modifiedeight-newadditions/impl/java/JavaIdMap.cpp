#include <java/JavaIdMap.hpp>
#include <item/Item.hpp>
#include <tile/Tile.hpp>

/*
 * Per-block target for Java ids 0..197.
 *
 * A value of -1 means "handled specially below" (metadata has to be rewritten
 * as well as the id). Everything else is a plain m8 tile id, with 0 for air.
 */
#define SPECIAL (-1)

static const int16_t gJavaToM8Block[256] = {
	/*   0 air                 */ 0,
	/*   1 stone               */ 1,
	/*   2 grass               */ 2,
	/*   3 dirt                */ 3,
	/*   4 cobblestone         */ 4,
	/*   5 planks              */ SPECIAL,
	/*   6 sapling             */ SPECIAL,
	/*   7 bedrock             */ 7,
	/*   8 flowing_water       */ 8,
	/*   9 water               */ 9,
	/*  10 flowing_lava        */ 10,
	/*  11 lava                */ 11,
	/*  12 sand                */ SPECIAL,   // red sand has no m8 tile
	/*  13 gravel              */ 13,
	/*  14 gold_ore            */ 14,
	/*  15 iron_ore            */ 15,
	/*  16 coal_ore            */ 16,
	/*  17 log                 */ SPECIAL,   // strip the axis bits
	/*  18 leaves              */ SPECIAL,   // strip decay/check bits
	/*  19 sponge              */ 19,
	/*  20 glass               */ 20,
	/*  21 lapis_ore           */ 21,
	/*  22 lapis_block         */ 22,
	/*  23 dispenser           */ SPECIAL,   // -> furnace, remap facing
	/*  24 sandstone           */ 24,
	/*  25 noteblock           */ 5,         // -> planks
	/*  26 bed                 */ 26,
	/*  27 golden_rail         */ 27,
	/*  28 detector_rail       */ 27,        // -> powered rail
	/*  29 sticky_piston       */ 4,
	/*  30 web                 */ 30,
	/*  31 tallgrass           */ 31,
	/*  32 deadbush            */ 32,
	/*  33 piston              */ 4,
	/*  34 piston_head         */ 4,
	/*  35 wool                */ 35,
	/*  36 piston_extension    */ 0,         // technical block
	/*  37 yellow_flower       */ 37,
	/*  38 red_flower          */ SPECIAL,   // spread over m8's separate flowers
	/*  39 brown_mushroom      */ 39,
	/*  40 red_mushroom        */ 40,
	/*  41 gold_block          */ 41,
	/*  42 iron_block          */ 42,
	/*  43 double_stone_slab   */ SPECIAL,
	/*  44 stone_slab          */ 44,
	/*  45 brick_block         */ 45,
	/*  46 tnt                 */ 46,
	/*  47 bookshelf           */ 47,
	/*  48 mossy_cobblestone   */ 48,
	/*  49 obsidian            */ 49,
	/*  50 torch               */ 50,
	/*  51 fire                */ 51,
	/*  52 mob_spawner         */ 4,
	/*  53 oak_stairs          */ 53,
	/*  54 chest               */ 54,
	/*  55 redstone_wire       */ 0,
	/*  56 diamond_ore         */ 56,        // m8's "emeraldOre" is diamond ore
	/*  57 diamond_block       */ 57,
	/*  58 crafting_table      */ 58,
	/*  59 wheat               */ 59,
	/*  60 farmland            */ 60,
	/*  61 furnace             */ 61,
	/*  62 lit_furnace         */ 62,
	/*  63 standing_sign       */ 63,
	/*  64 wooden_door         */ 64,
	/*  65 ladder              */ 65,
	/*  66 rail                */ 66,
	/*  67 stone_stairs        */ 67,
	/*  68 wall_sign           */ 68,
	/*  69 lever               */ 69,
	/*  70 stone_pressure_plate*/ 44,        // -> stone slab
	/*  71 iron_door           */ 71,
	/*  72 wooden_press_plate  */ 158,       // -> wooden slab
	/*  73 redstone_ore        */ 73,
	/*  74 lit_redstone_ore    */ 74,
	/*  75 unlit_rs_torch      */ 50,
	/*  76 redstone_torch      */ 50,
	/*  77 stone_button        */ 0,
	/*  78 snow_layer          */ 78,
	/*  79 ice                 */ 79,
	/*  80 snow                */ 80,
	/*  81 cactus              */ 81,
	/*  82 clay                */ 82,
	/*  83 reeds               */ 83,
	/*  84 jukebox             */ 47,        // -> bookshelf
	/*  85 fence               */ 85,
	/*  86 pumpkin             */ 86,
	/*  87 netherrack          */ 87,
	/*  88 soul_sand           */ 12,        // -> sand
	/*  89 glowstone           */ 89,
	/*  90 portal              */ 0,
	/*  91 lit_pumpkin         */ 91,
	/*  92 cake                */ 92,
	/*  93 unpowered_repeater  */ 0,
	/*  94 powered_repeater    */ 0,
	/*  95 stained_glass       */ 195,       // m8 95 is invisible_bedrock
	/*  96 trapdoor            */ 96,
	/*  97 monster_egg         */ SPECIAL,
	/*  98 stonebrick          */ 98,
	/*  99 brown_mushroom_block*/ 17,        // -> log
	/* 100 red_mushroom_block  */ 45,        // -> brick
	/* 101 iron_bars           */ 101,
	/* 102 glass_pane          */ 102,
	/* 103 melon_block         */ 103,
	/* 104 pumpkin_stem        */ 104,
	/* 105 melon_stem          */ 105,
	/* 106 vine                */ 106,
	/* 107 fence_gate          */ 107,
	/* 108 brick_stairs        */ 108,
	/* 109 stone_brick_stairs  */ 109,
	/* 110 mycelium            */ 2,         // -> grass
	/* 111 waterlily           */ 111,
	/* 112 nether_brick        */ 112,
	/* 113 nether_brick_fence  */ 85,        // -> fence
	/* 114 nether_brick_stairs */ 114,
	/* 115 nether_wart         */ 59,        // m8 115 is a coloured log
	/* 116 enchanting_table    */ 47,
	/* 117 brewing_stand       */ 58,
	/* 118 cauldron            */ 42,
	/* 119 end_portal          */ 0,
	/* 120 end_portal_frame    */ 24,
	/* 121 end_stone           */ 24,
	/* 122 dragon_egg          */ 49,
	/* 123 redstone_lamp       */ 123,
	/* 124 lit_redstone_lamp   */ 124,
	/* 125 double_wooden_slab  */ 157,       // m8 125 is a coloured log
	/* 126 wooden_slab         */ 158,
	/* 127 cocoa               */ 0,
	/* 128 sandstone_stairs    */ 128,
	/* 129 emerald_ore         */ 56,
	/* 130 ender_chest         */ 130,
	/* 131 tripwire_hook       */ 0,
	/* 132 tripwire            */ 0,
	/* 133 emerald_block       */ 57,
	/* 134 spruce_stairs       */ 134,
	/* 135 birch_stairs        */ 135,
	/* 136 jungle_stairs       */ 136,
	/* 137 command_block       */ 4,
	/* 138 beacon              */ 20,
	/* 139 cobblestone_wall    */ 139,
	/* 140 flower_pot          */ 140,
	/* 141 carrots             */ 141,
	/* 142 potatoes            */ 142,
	/* 143 wooden_button       */ 0,
	/* 144 skull               */ 0,
	/* 145 anvil               */ 42,
	/* 146 trapped_chest       */ 54,
	/* 147 light_weighted_pp   */ 0,
	/* 148 heavy_weighted_pp   */ 0,
	/* 149 unpowered_comparator*/ 0,
	/* 150 powered_comparator  */ 0,
	/* 151 daylight_detector   */ 151,
	/* 152 redstone_block      */ 45,
	/* 153 quartz_ore          */ 87,
	/* 154 hopper              */ 42,
	/* 155 quartz_block        */ 155,
	/* 156 quartz_stairs       */ 156,
	/* 157 activator_rail      */ 126,
	/* 158 dropper             */ 61,
	/* 159 stained_hardened_cl */ 238,
	/* 160 stained_glass_pane  */ 196,
	/* 161 leaves2             */ SPECIAL,
	/* 162 log2                */ SPECIAL,
	/* 163 acacia_stairs       */ 53,
	/* 164 dark_oak_stairs     */ 134,
	/* 165 slime               */ 165,
	/* 166 barrier             */ 95,
	/* 167 iron_trapdoor       */ 167,
	/* 168 prismarine          */ 98,
	/* 169 sea_lantern         */ 89,
	/* 170 hay_block           */ 170,
	/* 171 carpet              */ 171,
	/* 172 hardened_clay       */ 45,
	/* 173 coal_block          */ 173,
	/* 174 packed_ice          */ 174,
	/* 175 double_plant        */ 190,
	/* 176 standing_banner     */ 0,
	/* 177 wall_banner         */ 0,
	/* 178 daylight_det_inv    */ 178,
	/* 179 red_sandstone       */ 179,
	/* 180 red_sandstone_stairs*/ 180,
	/* 181 double_stone_slab2  */ 181,
	/* 182 stone_slab2         */ 182,
	/* 183 spruce_fence_gate   */ 183,
	/* 184 birch_fence_gate    */ 107,
	/* 185 jungle_fence_gate   */ 107,
	/* 186 dark_oak_fence_gate */ 107,
	/* 187 acacia_fence_gate   */ 107,
	/* 188 spruce_fence        */ 197,
	/* 189 birch_fence         */ 198,
	/* 190 jungle_fence        */ 85,
	/* 191 dark_oak_fence      */ 197,
	/* 192 acacia_fence        */ 85,
	/* 193 spruce_door         */ 193,
	/* 194 birch_door          */ 194,
	/* 195 jungle_door         */ 199,
	/* 196 acacia_door         */ 200,
	/* 197 dark_oak_door       */ 202,
	/* 198..255 unused in 1.8  */ 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void JavaIdMap::javaBlockToM8(int32_t javaId, int32_t javaMeta, int32_t* outId, int32_t* outMeta) {
	int32_t id = 0, meta = 0;

	if(javaId >= 0 && javaId < 256) {
		int32_t mapped = gJavaToM8Block[javaId];
		if(mapped != SPECIAL) {
			id = mapped;
			meta = javaMeta & 0xF;

			// A few straight mappings still need their metadata clamped,
			// because m8 indexes texture arrays by it.
			switch(javaId) {
				case 35:  // wool, stained glass and carpets share the 16 dye slots
				case 95:
				case 171:
				case 159:
				case 160:
					break;
				case 44:  // slabs: low 3 bits pick the material, bit 3 is "upper half"
					if((meta & 7) > 6) meta = (meta & 8);
					break;
				case 125: // double wooden slab
				case 126: // wooden slab
					/*
					 * Tile::WOOD_NAMES has four entries and WoodSlabTile indexes
					 * it with "meta & 7", so acacia (4) and dark oak (5) - which
					 * a 1.8 server hands out freely, they grow in savannas and
					 * roofed forests - would read a std::string off the end of
					 * the array.  Fold them onto oak and keep the upper-half bit,
					 * which is the part that decides whether you can walk on it.
					 */
					if((meta & 7) > 3) meta = (meta & 8);
					break;
				case 17:
				case 18:
					meta &= 3;
					break;
				default:
					break;
			}
		} else {
			// Cases where the metadata has to be rewritten too.
			switch(javaId) {
				case 5:   // planks: acacia/dark oak fall back to oak
					id = 5;
					meta = (javaMeta & 0xF) > 3 ? 0 : (javaMeta & 3);
					break;
				case 6:   // sapling: drop the growth-stage bit, clamp species
					id = 6;
					meta = (javaMeta & 7) > 3 ? 0 : (javaMeta & 3);
					break;
				case 12:  // red sand -> plain sand
					id = 12;
					meta = 0;
					break;
				case 17:  // log: low 2 bits species, high bits axis
					id = 17;
					meta = javaMeta & 3;
					break;
				case 18:  // leaves: low 2 bits species
					id = 18;
					meta = javaMeta & 3;
					break;
				case 23:  // dispenser -> furnace; furnaces only face sideways
					id = 61;
					meta = (javaMeta & 7) < 2 ? 3 : (javaMeta & 7);
					break;
				case 38:  // red_flower spreads over m8's individual flower tiles
					switch(javaMeta & 0xF) {
						case 1: id = 240; meta = 0; break; // blue orchid
						case 2: id = 241; meta = 0; break; // allium
						case 3: id = 239; meta = 0; break; // azure bluet
						case 8: id = 192; meta = 0; break; // oxeye daisy
						default: id = 38; meta = 0; break; // poppy and the tulips
					}
					break;
				case 43:  // double stone slab: bit 3 is "seamless", not a material
					id = 43;
					meta = javaMeta & 7;
					if(meta > 6) meta = 0;
					break;
				case 97:  // monster_egg mimics whatever it is hiding in
					switch(javaMeta & 0xF) {
						case 0: id = 1; break;
						case 1: id = 4; break;
						default: id = 98; break;
					}
					meta = 0;
					break;
				case 161: // leaves2 (acacia, dark oak) -> oak leaves
					id = 18;
					meta = 0;
					break;
				case 162: // log2: acacia -> oak, dark oak -> spruce
					id = 17;
					meta = (javaMeta & 1) ? 1 : 0;
					break;
				case 181: // red sandstone double slab -> sandstone slab
					id = 43;
					meta = 1;
					break;
				case 182: // red sandstone slab, keeping the upper-half bit
					id = 44;
					meta = 1 | (javaMeta & 8);
					break;
				default:
					id = 0;
					meta = 0;
					break;
			}
		}
	}

	/*
	 * Last line of defence.  A mapping typo, or a target this build does not
	 * register, would otherwise hand Level::setTileAndData an id whose
	 * Tile::tiles[] slot is null, and the first onPlace / neighbourChanged /
	 * getAABB call on it takes the game down - which is exactly what "some
	 * special blocks crash the game" looks like from the outside.  Degrade to
	 * stone instead of crashing.
	 */
	if(id < 0 || id > 255) {
		id = 0;
		meta = 0;
	} else if(id != 0 && !Tile::tiles[id]) {
		id = 1;
		meta = 0;
	}

	if(outId) *outId = id;
	if(outMeta) *outMeta = meta;
}

void JavaIdMap::m8BlockToJava(int32_t m8Id, int32_t m8Meta, int32_t* outId, int32_t* outMeta) {
	int32_t id = m8Id, meta = m8Meta & 0xF;

	// Undo the remaps above for the ids where m8 and Java disagree. Everything
	// not listed here shares its id with vanilla 1.8.
	switch(m8Id) {
		case 115: case 116: case 117: case 118: case 119: case 120:
		case 121: case 122: case 125: case 127: case 129: case 130:
		case 131: case 132: case 133: case 137:
			id = 17; meta = 0; break;            // coloured logs -> oak log
		case 126: id = 157; break;               // activator rail
		case 157: id = 125; break;               // double wooden slab
		case 158: id = 126; break;               // wooden slab
		case 159: case 160: case 161: case 162:
			id = 44; meta = 0; break;            // coloured brick slabs -> stone slab
		case 143: case 144: case 145: case 146: case 147: case 148:
		case 149: case 150: case 151: case 152: case 153: case 154:
		case 163: case 164: case 165: case 166:
			id = 108; meta = m8Meta & 3; break;  // coloured brick stairs -> brick stairs
		case 174: case 175: case 176: case 177: case 178: case 179:
		case 180: case 181: case 182: case 183: case 184: case 185:
		case 186: case 187: case 188: case 189:
			id = 85; meta = 0; break;            // coloured fences -> oak fence
		case 190: id = 175; break;               // double plant
		case 191: case 192: case 239: case 240: case 241:
			id = 38; meta = 0; break;            // extra flowers -> poppy
		case 193: id = 193; break;               // spruce door
		case 194: id = 194; break;               // birch door
		case 95:  id = 166; meta = 0; break;     // invisible bedrock -> barrier
		case 195: id = 95; break;                // stained glass
		case 196: id = 160; break;               // stained glass pane
		case 197: id = 188; break;               // spruce fence
		case 198: id = 189; break;               // birch fence
		case 201: case 202: case 203: case 204: case 205: case 206:
		case 207: case 208: case 209: case 210: case 211: case 212:
		case 213: case 214: case 215: case 216:
			id = 26; meta = 0; break;            // coloured beds -> bed
		case 217: id = 5; meta = 0; break;       // coloured planks -> oak planks
		case 218: case 219: case 220: case 221: case 222: case 223:
		case 224: case 225: case 226: case 227: case 228: case 229:
		case 230: case 231: case 232: case 233:
			id = 53; meta = m8Meta & 3; break;   // coloured stairs -> oak stairs
		case 234: case 235: case 236: case 237:
			id = 126; break;                     // coloured slabs -> wooden slab
		case 238: id = 159; break;               // coloured bricks -> stained clay
		case 244: id = 141; break;               // beetroot -> carrots
		case 245: id = 58; meta = 0; break;      // stonecutter -> crafting table
		case 246: id = 49; meta = 0; break;      // glowing obsidian -> obsidian
		case 247: id = 4; meta = 0; break;       // nether reactor -> cobblestone
		case 248: case 249: case 255:
			id = 1; meta = 0; break;             // info blocks -> stone
		case 250: id = 3; meta = 1; break;       // grass path -> coarse dirt
		case 251: id = 44; break;                // mixed slab -> stone slab
		case 252: id = 31; meta = 1; break;       // seagrass -> tall grass
		default:
			if(m8Id > 197) { id = 1; meta = 0; }
			break;
	}

	if(outId) *outId = id;
	if(outMeta) *outMeta = meta;
}

// ---------------------------------------------------------------------------
// Items
// ---------------------------------------------------------------------------

/*
 * m8's item ids line up with vanilla for everything it implements (256 iron
 * shovel through 406 quartz), so translation is mostly the identity with a
 * substitute for what m8 is missing. m8's own additions live at 456+.
 */
static int32_t javaItemFallback(int32_t javaItemId) {
	switch(javaItemId) {
		case 322: return 260; // golden_apple    -> apple
		case 326: return 325; // water_bucket    -> bucket
		case 327: return 325; // lava_bucket     -> bucket
		case 333: return 280; // boat            -> stick
		case 335: return 325; // milk_bucket     -> bucket
		case 342: return 328; // chest_minecart  -> minecart
		case 343: return 328; // furnace_minecart-> minecart
		case 346: return 280; // fishing_rod     -> stick
		case 349: return 319; // fish            -> raw porkchop
		case 350: return 320; // cooked_fish     -> cooked porkchop
		case 356: return 331; // repeater        -> redstone
		case 357: return 297; // cookie          -> bread
		case 358: return 339; // filled_map      -> paper
		case 367: return 319; // rotten_flesh    -> raw porkchop
		case 368: return 332; // ender_pearl     -> snowball
		case 369: return 280; // blaze_rod       -> stick
		case 370: return 341; // ghast_tear      -> slimeball
		case 371: return 266; // gold_nugget     -> gold ingot
		case 372: return 351; // nether_wart     -> dye
		case 373: return 374; // potion          -> glass bottle (also missing, caught below)
		case 375: return 351; // spider_eye      -> dye
		case 376: return 351;
		case 377: return 348; // blaze_powder    -> glowstone dust
		case 378: return 341; // magma_cream     -> slimeball
		case 379: return 340; // brewing_stand   -> book
		case 380: return 265; // cauldron        -> iron ingot
		case 381: return 264; // ender_eye       -> diamond
		case 382: return 360; // speckled_melon  -> melon
		case 384: return 332; // xp bottle       -> snowball
		case 385: return 259; // fire_charge     -> flint and steel
		case 386: return 340; // writable_book   -> book
		case 387: return 340; // written_book    -> book
		case 388: return 264; // emerald         -> diamond
		case 389: return 321; // item_frame      -> painting
		case 390: return 337; // flower_pot      -> clay
		case 394: return 392; // poisonous_potato-> potato
		case 395: return 339; // map             -> paper
		case 396: return 391; // golden_carrot   -> carrot
		case 397: return 352; // skull           -> bone
		case 398: return 391; // carrot_on_stick -> carrot
		case 399: return 348; // nether_star     -> glowstone dust
		case 401: return 289; // fireworks       -> gunpowder
		case 402: return 289;
		case 403: return 340; // enchanted_book  -> book
		case 404: return 331; // comparator      -> redstone
		case 407: return 328; // tnt_minecart    -> minecart
		case 408: return 328; // hopper_minecart -> minecart
		case 409: return 406; // prismarine_shard-> quartz
		case 410: return 406;
		case 411: return 365; // rabbit          -> raw chicken
		case 412: return 366; // cooked_rabbit    -> cooked chicken
		case 413: return 282; // rabbit_stew     -> mushroom stew
		case 414: return 341; // rabbit_foot     -> slimeball
		case 415: return 334; // rabbit_hide     -> leather
		case 416: return 280; // armor_stand     -> stick
		case 417: return 306; // horse armor     -> iron chestplate
		case 418: return 314;
		case 419: return 310;
		case 420: return 287; // lead            -> string
		case 421: return 339; // name_tag        -> paper
		case 422: return 328; // cmd minecart    -> minecart
		case 423: return 319; // mutton          -> raw porkchop
		case 424: return 320; // cooked_mutton   -> cooked porkchop
		case 425: return 287; // banner          -> string
		case 427: return 460; // spruce_door     -> m8 door_spruce
		case 428: return 461; // birch_door      -> m8 door_birch
		case 429: return 324; // jungle_door     -> wooden door
		case 430: return 324; // acacia_door     -> wooden door
		case 431: return 460; // dark_oak_door   -> m8 door_spruce
		default:  return 0;
	}
}

// The item ids m8 actually implements above 255, so we never hand Item::items
// an index that is null.
static bool_t m8HasItem(int32_t id) {
	if(id < 256) return 0;
	int32_t n = id - 256;
	if(n <= 64) return 1;                                     // 256..320
	switch(n) {
		case 65: case 67: case 68: case 69: case 72: case 73: case 74:
		case 75: case 76: case 78: case 80: case 81: case 82: case 83:
		case 84: case 85: case 88: case 89: case 91: case 92: case 95:
		case 96: case 97: case 98: case 99: case 103: case 104: case 105:
		case 106: case 107: case 108: case 109: case 110: case 127:
		case 135: case 136: case 137: case 144: case 149: case 150:
		case 200: case 201: case 202: case 203: case 204: case 205:
			return 1;
		default:
			return 0;
	}
}

/*
 * A tile existing is not the same as it being carryable: Item::items[] has no
 * entry for the blocks 0.8.1 never handed the player (pressure plates, fire,
 * flowing water...), and neither does it have one for every id m8HasItem's
 * table claims.  An ItemInstance built on such an id still reports itself
 * valid but leaves itemClass null, and the first getIcon / getName /
 * getMaxStackSize on it takes the game down.  So nothing leaves this function
 * without a registered Item behind it.
 */
static bool_t m8ItemRegistered(int32_t id) {
	return id > 0 && id < 512 && Item::items[id] != 0;
}

int32_t JavaIdMap::javaItemToM8(int32_t javaItemId) {
	if(javaItemId <= 0) return 0;

	// Block items translate through the block table.
	if(javaItemId < 256) {
		int32_t id = 0, meta = 0;
		JavaIdMap::javaBlockToM8(javaItemId, 0, &id, &meta);
		if(!m8ItemRegistered(id) && !(id > 0 && id < 256 && Tile::tiles[id])) {
			/*
			 * Either the block has no item behind it, or - and this is the one
			 * that took the game down - the block itself translates to air.  A
			 * barrier used to land here: nothing in 0.8.1 looks like it, so the
			 * table answered 0, and ItemInstance(0) reports itself *valid* with
			 * both itemClass and tileClass null, because id 0 is how an empty
			 * slot is spelled.  Hand that to ItemRenderer as a dropped stack
			 * with a count and it dereferences itemClass->itemTexture on the
			 * first frame it is on screen.  Anything holdable is better than
			 * that, and it is a block either way.
			 */
			return m8ItemRegistered(1) ? 1 : 0;
		}
		return id;
	}

	if(m8HasItem(javaItemId) && m8ItemRegistered(javaItemId)) return javaItemId;

	int32_t sub = javaItemFallback(javaItemId);
	if(m8ItemRegistered(sub)) return sub;
	return 0;
}

int32_t JavaIdMap::m8ItemToJava(int32_t m8ItemId) {
	if(m8ItemId <= 0) return 0;

	if(m8ItemId < 256) {
		int32_t id = 0, meta = 0;
		JavaIdMap::m8BlockToJava(m8ItemId, 0, &id, &meta);
		return id;
	}

	switch(m8ItemId) {
		case 456: return 280; // camera         -> stick
		case 457: return 391; // beetroot       -> carrot
		case 458: return 361; // beetroot seeds -> pumpkin seeds
		case 459: return 282; // beetroot soup  -> mushroom stew
		case 460: return 427; // door_spruce
		case 461: return 428; // door_birch
		case 383: return 383; // spawn egg
		default:  return m8ItemId <= 406 ? m8ItemId : 0;
	}
}

// ---------------------------------------------------------------------------
// Entities
// ---------------------------------------------------------------------------

int32_t JavaIdMap::javaMobToM8(int32_t javaType) {
	switch(javaType) {
		case 14: return 14;  // wolf
		case 15: return 15;  // villager
		case 17: return 17;  // squid
		case 22: return 22;  // ocelot
		case 26: return 26;  // polar bear
		case 27: return 27;  // cod
		case 28: return 28;  // salmon
		case 29: return 29;  // pufferfish
		case 30: return 30;  // tropical fish
		case 37: return 37;  // slime
		case 38: return 38;  // fox
		case 39: return 39;  // turtle
		case 40: return 40;  // frog
		case 50: return 33;  // creeper
		case 51: return 34;  // skeleton
		case 52: return 35;  // spider
		case 53: return 53;  // giant
		case 54: return 32;  // zombie
		case 55: return 37;  // slime
		case 56: return 33;  // ghast
		case 57: return 36;  // zombie pigman
		case 58: return 32;  // enderman
		case 59: return 35;  // cave spider
		case 60: return 35;  // silverfish
		case 61: return 34;  // blaze
		case 62: return 37;  // magma cube -> slime
		case 63: return 35;  // ender dragon
		case 64: return 34;  // wither
		case 65: return 10;  // bat
		case 66: return 32;  // witch
		case 67: return 35;  // endermite
		case 68: return 35;  // guardian
		case 90: return 12;  // pig
		case 91: return 13;  // sheep
		case 92: return 11;  // cow
		case 93: return 10;  // chicken
		case 94: return 17;  // squid
		case 95: return 14;  // wolf
		case 96: return 11;  // mooshroom
		case 97: return 13;  // snow golem
		case 98: return 22;  // ocelot
		case 99: return 32;  // iron golem
		case 100: return 12; // horse
		case 101: return 10; // rabbit
		case 102: return 26; // polar bear
		case 107: return 39; // turtle
		case 109: return 27; // cod
		case 110: return 28; // salmon
		case 111: return 29; // pufferfish
		case 112: return 30; // tropical fish
		case 120: return 15; // villager
		case 121: return 38; // fox
		default: return 0;
	}
}

int32_t JavaIdMap::javaObjectToM8(int32_t javaObjectType) {
	switch(javaObjectType) {
		case 1:  return 40; // boat
		case 2:  return 64; // dropped item
		case 10: return 84; // minecart
		case 40: return 40; // boat
		case 41: return 40; // boat
		case 50: return 65; // primed tnt
		case 51: return 0;  // ender crystal
		case 60: return 80; // arrow
		case 61: return 81; // snowball
		case 62: return 82; // thrown egg
		case 63: return 81; // ghast fireball
		case 64: return 81; // blaze fireball
		case 65: return 81; // ender pearl
		case 66: return 81; // wither skull
		case 70: return 66; // falling block
		case 71: return 71; // item frame
		case 72: return 81; // eye of ender
		case 73: return 81; // thrown potion
		case 75: return 81; // xp bottle
		case 76: return 81; // firework
		case 77: return 0;  // leash knot
		case 78: return 78; // armor stand
		case 90: return 0;  // fishing bobber
		default: return 0;
	}
}
