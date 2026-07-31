#include <I18n.hpp>
#include <item/ItemInstance.hpp>
#include <tile/Tile.hpp>
#include <util/Util.hpp>
#include <AppPlatform.hpp>
#include <_AssetFile.hpp>
#include <sstream>
#include <cstdlib> // Для atoi

std::map<std::string, std::string> I18n::_strings;

void I18n::loadLanguage(struct AppPlatform* a1, const std::string& a2) {
	I18n::_strings.clear();
	I18n::fillTranslations(a1, "lang/en_US.lang", 1);
	if(a2 == "en_US") {
		I18n::fillTranslations(a1, "lang/" + a2 + ".lang", 1);
	}
}

const char_t* _d6e03d98[] = {"wood", "iron", "stone", "diamond", "gold", "brick", "emerald", "lapis", "cloth"};

std::string I18n::getDescriptionString(const struct ItemInstance& a2) {
	std::string v15 = a2.getDescriptionId();
	std::string v16 = v15;
	if(a2.tileClass == Tile::cloth && Tile::cloth) {
		if(a2.getAuxValue()) {
			return I18n::get("desc.wool");
		} else {
			return I18n::get("desc.woolstring");
		}
	}
	if(Tile::fenceGate == a2.tileClass && a2.tileClass) {
		return I18n::get("desc.fence");
	}
	if(Tile::stoneSlabHalf == a2.tileClass && a2.tileClass) {
		return I18n::get("desc.slab");
	}
	v16 = Util::toLower(v16);
	if(v16[0] == 't') {
		v16 = *Util::stringReplace(v16, "tile.", "desc.", -1);
	}
	if(v16[0] == 'i') {
		v16 = *Util::stringReplace(v16, "item.", "desc.", -1);
	}

	std::string v17;
	if(!I18n::get(v16, v17)) {
		std::string _d6e09320[] = {"tile.workbench", "craftingtable", "tile.quartzBlockChiseled", "quartzBlockChiseled", "tile.quartzBlockPillar", "quartzBlockPillar"};
		Util::removeAll(v16, _d6e03d98, 9);
		if(!I18n::get(v16, v17)) {
			int32_t v9 = 0;
			while(1) {
				if(v15 == _d6e09320[v9]) {
					if(I18n::get("desc."+_d6e09320[v9 + 1], v17)) break;
				}
				v9 += 2;
				if(v9 == 6) {
					return v15 + " : couldn't find desc";
				}
			}
		}
	}
	return v17;
}

static bool translateWoodVariant(const std::string& key, std::string& out) {
	std::string colors[] = {"white", "orange", "magenta", "lightBlue", "yellow", "lime", "pink", "gray", "silver", "cyan", "purple", "blue", "brown", "green", "red", "black"};
	std::string colorNames[] = {"White", "Orange", "Magenta", "Light Blue", "Yellow", "Lime", "Pink", "Gray", "Light Gray", "Cyan", "Purple", "Blue", "Brown", "Green", "Red", "Black"};

	auto getIdx = [](const std::string& str) -> int {
		return atoi(str.c_str());
	};

	if (key.find("tile.coloredPlanks.") == 0) {
		std::string color = key.substr(19);
		size_t dot = color.find('.');
		if (dot != std::string::npos) color = color.substr(0, dot);
		for (int i = 0; i < 16; i++) {
			if (color == colors[i]) {
				out = colorNames[i] + " Planks";
				return true;
			}
		}
		out = "Colored Planks";
		return true;
	}

	if (key.find("tile.coloredStairs.") == 0) {
		std::string indexStr = key.substr(19);
		size_t dot = indexStr.find('.');
		if (dot != std::string::npos) indexStr = indexStr.substr(0, dot);
		int idx = getIdx(indexStr);
		if (idx >= 0 && idx < 16) {
			out = colorNames[idx] + " Stairs";
			return true;
		}
		out = "Colored Stairs";
		return true;
	}

	if (key.find("tile.coloredFence.") == 0) {
		std::string indexStr = key.substr(18);
		size_t dot = indexStr.find('.');
		if (dot != std::string::npos) indexStr = indexStr.substr(0, dot);
		int idx = getIdx(indexStr);
		if (idx >= 0 && idx < 16) {
			out = colorNames[idx] + " Fence";
			return true;
		}
		out = "Colored Fence";
		return true;
	}

	if (key.find("tile.coloredLog.") == 0) {
		std::string indexStr = key.substr(16);
		size_t dot = indexStr.find('.');
		if (dot != std::string::npos) indexStr = indexStr.substr(0, dot);
		int idx = getIdx(indexStr);
		if (idx >= 0 && idx < 16) {
			out = colorNames[idx] + " Wood";
			return true;
		}
		out = "Colored Wood";
		return true;
	}

	if (key.find("tile.coloredSlab.") == 0) {
		std::string color = key.substr(17);
		size_t dot = color.find('.');
		if (dot != std::string::npos) color = color.substr(0, dot);
		for (int i = 0; i < 16; i++) {
			if (color == colors[i]) {
				out = colorNames[i] + " Slab";
				return true;
			}
		}
		out = "Colored Slab";
		return true;
	}

	if (key.find("tile.coloredBricks.") == 0) {
		std::string color = key.substr(19);
		size_t dot = color.find('.');
		if (dot != std::string::npos) color = color.substr(0, dot);
		for (int i = 0; i < 16; i++) {
			if (color == colors[i]) {
				out = colorNames[i] + " Bricks";
				return true;
			}
		}
		out = "Colored Bricks";
		return true;
	}

	if (key.find("tile.coloredBrickStairs.") == 0) {
		std::string indexStr = key.substr(24);
		size_t dot = indexStr.find('.');
		if (dot != std::string::npos) indexStr = indexStr.substr(0, dot);
		int idx = getIdx(indexStr);
		if (idx >= 0 && idx < 16) {
			out = colorNames[idx] + " Brick Stairs";
			return true;
		}
		out = "Colored Brick Stairs";
		return true;
	}

	if (key.find("tile.coloredBrickSlab.") == 0) {
		std::string color = key.substr(22);
		size_t dot = color.find('.');
		if (dot != std::string::npos) color = color.substr(0, dot);
		for (int i = 0; i < 16; i++) {
			if (color == colors[i]) {
				out = colorNames[i] + " Brick Slab";
				return true;
			}
		}
		out = "Colored Brick Slab";
		return true;
	}

	return false;
}

bool_t I18n::get(const std::string& a1, std::string& a2) {
	if (a1 == "options.newadditions") { a2 = "ModifiedEight New Additions"; return 1; }
	if (a1 == "options.newadditions.desc") { a2 = "ModifiedEight New Additions Options"; return 1; }
	if (a1 == "options.clientmode") { a2 = "Client mode"; return 1; }
	if (a1 == "options.sprint") { a2 = "Sprint"; return 1; }
	if (a1 == "options.sprint.desc") { a2 = "Enables sprinting (double-tap forward)"; return 1; }
	if (a1 == "options.autojump") { a2 = "Auto-Jump"; return 1; }
	if (a1 == "options.fov") { a2 = "FOV"; return 1; }
	if (a1 == "options.chatcolor") { a2 = "Chat Color"; return 1; }
	if (a1 == "options.chatbgcolor") { a2 = "Chat BG Color"; return 1; }
	if (a1 == "options.showfps") { a2 = "Show FPS"; return 1; }
	if (a1 == "options.debugscreen") { a2 = "Debug Screen"; return 1; }
	if (a1 == "options.discordrpc") { a2 = "Discord Integration"; return 1; }
	if (a1 == "options.classicbackground") { a2 = "Classic Background"; return 1; }
	if (a1 == "options.classicgui") { a2 = "Classic GUI"; return 1; }
	if (a1 == "options.neoncolortheme") { a2 = "Neon Theme"; return 1; }
	if (a1 == "options.hudcamerabutton") { a2 = "HUD Camera Button"; return 1; }
	if (a1 == "options.hudcamerabutton.desc") { a2 = "Shows a camera toggle button in the HUD"; return 1; }
	if (a1 == "options.showcoordinates") { a2 = "Show Coordinates"; return 1; }
	if (a1 == "options.showcoordinates.desc") { a2 = "Shows the player coordinates on screen"; return 1; }
	if (a1 == "options.classictextures") { a2 = "Classic texture loader"; return 1; }
	if (a1 == "options.classictextures.desc") { a2 = "Loads textures from terrain-atlas / items-opaque"; return 1; }
	if (a1 == "options.marketplace") { a2 = "Texture Packs button"; return 1; }
	if (a1 == "options.fogenabled") { a2 = "Fog"; return 1; }
	if (a1 == "tile.grassPath.name") { a2 = "Grass Path"; return 1; }
	if (translateWoodVariant(a1, a2)) { return 1; }

	auto&& v3 = I18n::_strings.find(a1);
	if(v3 == I18n::_strings.end()) return 0;
	a2 = v3->second;
	return 1;
}

std::string I18n::get(const std::string& a2) {
	if (a2 == "options.newadditions") return "ModifiedEight New Additions";
	if (a2 == "options.newadditions.desc") return "ModifiedEight New Additions Options";
	if (a2 == "options.clientmode") return "Client mode";
	if (a2 == "options.sprint") return "Sprint";
	if (a2 == "options.sprint.desc") return "Enables sprinting (double-tap forward)";
	if (a2 == "options.autojump") return "Auto-Jump";
	if (a2 == "options.fov") return "FOV";
	if (a2 == "options.chatcolor") return "Chat Color";
	if (a2 == "options.chatbgcolor") return "Chat BG Color";
	if (a2 == "options.classicbackground") return "Classic Background";
	if (a2 == "options.classicgui") return "Classic GUI";
	if (a2 == "options.neoncolortheme") return "Neon Theme";
	if (a2 == "options.hudcamerabutton") return "HUD Camera Button";
	if (a2 == "options.hudcamerabutton.desc") return "Shows a camera toggle button in the HUD";
	if (a2 == "options.showcoordinates") return "Show Coordinates";
	if (a2 == "options.showcoordinates.desc") return "Shows the player coordinates on screen";
	if (a2 == "options.classictextures") return "Classic texture loader";
	if (a2 == "options.classictextures.desc") return "Loads textures from terrain-atlas / items-opaque";
	if (a2 == "options.marketplace") return "Texture Packs button";
	if (a2 == "options.fogenabled") return "Fog";
	if (a2 == "options.showfps") return "Show FPS";
	if (a2 == "options.debugscreen") return "Debug Screen";
	if (a2 == "options.discordrpc") return "Discord Integration";
	if (a2 == "tile.grassPath.name") return "Grass Path";

	std::string out;
	if (translateWoodVariant(a2, out)) return out;

	auto&& v3 = I18n::_strings.find(a2);
	if(v3 == I18n::_strings.end()) return a2 + '<';
	return v3->second;
}

void I18n::fillTranslations(struct AppPlatform* a1, const std::string& a2, bool_t a3) {
	AssetFile v13 = a1->readAssetFile(a2);
	if(v13.bytes) {
		if(v13.length >= 0) {
			std::string v9((char*)v13.bytes, v13.length);
			std::stringstream v17(v9);
			for(std::string v10; std::getline(v17, v10);) {
				size_t v5 = v10.find('=');
				if(v5 != std::string::npos) {
					std::string key = Util::stringTrim(v10.substr(0, v5));
					auto&& v7 = I18n::_strings.find(key);
					if(v7 == I18n::_strings.end()) {
						std::string value = Util::stringTrim(v10.substr(v5 + 1));
						I18n::_strings.insert({key, value});
					}
				}
			}
			delete[] v13.bytes;
		}
	}
}