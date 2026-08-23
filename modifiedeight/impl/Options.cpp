#include <Options.hpp>
#include <I18n.hpp>
#include <Minecraft.hpp>
#include <OptionStrings.hpp>
#include <entity/player/User.hpp>
#include <math.h>
#include <network/mco/MojangConnector.hpp>
#include <sstream>
#include <util/Util.hpp>

Options* Options::instance = nullptr;

Options::Option Options::Option::MUSIC{1, "options.music", 0};
Options::Option Options::Option::SOUND{1, "options.sound", 1};
Options::Option Options::Option::INVERT_MOUSE{0, "options.invertMouse", 2};
Options::Option Options::Option::SENSITIVITY{1, "options.sensitivity", 3};
Options::Option Options::Option::RENDER_DISTANCE{3, "options.renderDistance", 4};
Options::Option Options::Option::VIEW_BOBBING{0, "options.viewBobbing", 5};
Options::Option Options::Option::LIMIT_FRAMERATE{0, "options.limitFramerate", 7};
Options::Option Options::Option::DIFFICULTY{3, "options.difficulty", 8};
Options::Option Options::Option::GRAPHICS{0, "options.graphics", 9};
Options::Option Options::Option::GUI_SCALE{3, "options.guiScale", 11};
Options::Option Options::Option::THIRD_PERSON{0, "options.thirdperson", 12};
Options::Option Options::Option::HIDE_GUI{0, "options.hidegui", 13};
Options::Option Options::Option::SERVER_VISIBLE{0, "options.servervisible", 14};
Options::Option Options::Option::LEFT_HANDED{0, "options.lefthanded", 15};
Options::Option Options::Option::USE_TOUCHSCREEN{0, "options.usetouchscreen", 16};
Options::Option Options::Option::USE_TOUCH_JOYPAD{0, "options.usetouchpad", 17};
Options::Option Options::Option::DESTROY_VIBRATION{0, "options.destroyvibration", 18};
Options::Option Options::Option::FANCY_SKIES{0, "options.fancyskies", 19};
Options::Option Options::Option::ANIMATE_TEXTURES{0, "options.animatetextures", 20};
Options::Option Options::Option::PIXELS_PER_MILLIMETER{1, "options.pixelspermilimeter", 21};
Options::Option Options::Option::NAME{2, "options.name", 22};
Options::Option Options::Option::NEW_ADDITIONS{3, "options.clientmode", 23};
Options::Option Options::Option::SPRINT{0, "options.sprint", 26};
Options::Option Options::Option::AUTO_JUMP{0, "options.autojump", 37};
Options::Option Options::Option::FOV{1, "options.fov", 38};
Options::Option Options::Option::CHAT_COLOR{3, "options.chatcolor", 24};
Options::Option Options::Option::CHAT_BG_COLOR{3, "options.chatbgcolor", 25};
Options::Option Options::Option::HUD_CAMERA_BUTTON{0, "options.hudcamerabutton", 30};
Options::Option Options::Option::SHOW_COORDINATES{0, "options.showcoordinates", 31};
Options::Option Options::Option::CLASSIC_TEXTURES{0, "options.classictextures", 32};
Options::Option Options::Option::MARKETPLACE{0, "options.marketplace", 33};
Options::Option Options::Option::FOG_ENABLED{0, "options.fogenabled", 34};
Options::Option Options::Option::SHOW_FPS{0, "options.showfps", 35};
Options::Option Options::Option::DEBUG_SCREEN{0, "options.debugscreen", 36};
Options::Option Options::Option::DISCORD_RPC{0, "options.discordrpc", 39};
Options::Option Options::Option::ANIMATE_WATER{0, "options.animatewater", 41};
Options::Option Options::Option::ANIMATE_LAVA{0, "options.animatelava", 42};
Options::Option Options::Option::ANIMATE_FIRE{0, "options.animatefire", 43};
std::vector<int32_t> Options::DIFFICULTY_LEVELS = {0, 2};
std::vector<int32_t> Options::RENDERDISTANCE_LEVELS = {3, 2, 1, 0, -1, -2, -3};
std::vector<int32_t> Options::CHAT_COLOR_LEVELS = {0, 1, 2, 3, 4, 5, 6, 7, 8};
std::vector<int32_t> Options::CHAT_BG_COLOR_LEVELS = {0, 1, 2, 3, 4};
std::vector<int32_t> Options::NEW_ADDITIONS_LEVELS = {0, 1};

void Options::update() {
	std::vector<std::string> v13 = this->settingFolderPath.getOptionStrings();
	for(int i = 0;; i += 2) {
		if(i >= v13.size()) break;
		if(v13[i] == OptionStrings::Multiplayer_Username) {
			if(v13[i + 1].size()) {
				this->username = v13[i + 1];
			} else {
				this->username = "Steve";
			}
		} else {
			if(v13[i] == OptionStrings::Multiplayer_ServerVisible) {
				this->readBool(v13[i + 1], this->serverVisible);
			} else if(v13[i] == OptionStrings::Controls_Sensitivity) {
				float v12;
				if(this->readFloat(v13[i + 1], v12)) {
					this->sensitity = (float)(powf(v12 * 1.1, 1.3) * 0.42) + 0.3;
				}
			} else {
				if(v13[i] == OptionStrings::Controls_InvertMouse) {
					this->readBool(v13[i + 1], this->invertMouse);
				} else if(v13[i] == OptionStrings::Controls_IsLefthanded) {
					this->readBool(v13[i + 1], this->leftHanded);
				} else if(v13[i] == OptionStrings::Controls_UseTouchJoypad) {
					this->readBool(v13[i + 1], this->useJoypad);
					if(!this->minecraft->useTouchscreen()) {
						this->useJoypad = 0;
					}
				} else {
					if(v13[i] == OptionStrings::Controls_FeedbackVibration) {
						this->readBool(v13[i + 1], this->destroyVibration);
					} else if(v13[i] == OptionStrings::Graphics_RenderDistance) {
						this->readInt(v13[i + 1], this->renderDistance);
					} else if(v13[i] == OptionStrings::Graphics_PixelsPerMilimeter) {
						this->readFloat(v13[i + 1], this->pixelDensity);
						if(this->pixelDensity > 12) {
							this->pixelDensity = 12;
						} else if(this->pixelDensity <= 3) {
							this->pixelDensity = 3;
						}
					} else {
						if(v13[i] == OptionStrings::Graphics_FancyGraphics) {
							this->readBool(v13[i + 1], this->graphics);
						} else if(v13[i] == OptionStrings::Graphics_FancySkies) {
							this->readBool(v13[i + 1], this->fancySkies);
						} else if(v13[i] == OptionStrings::Graphics_AnimateTextures) {
							this->readBool(v13[i + 1], this->animateTextures);
						} else if(v13[i] == "options.newadditions") {
							// this->readBool(v13[i + 1], this->newAdditions);
						} else if(v13[i] == "options.sprint") {
							this->readBool(v13[i + 1], this->sprintEnabled);
						} else if(v13[i] == "options.autojump") {
							this->readBool(v13[i + 1], this->autoJump);
						} else if(v13[i] == "options.fov") {
							this->readFloat(v13[i + 1], this->fov);
						} else if(v13[i] == "options.hudcamerabutton") {
							this->readBool(v13[i + 1], this->hudCameraButton);
						} else if(v13[i] == "options.showcoordinates") {
							this->readBool(v13[i + 1], this->showCoordinates);
						} else if(v13[i] == "options.classictextures") {
							this->readBool(v13[i + 1], this->classicTextures);
						} else if(v13[i] == "options.marketplace") {
							this->readBool(v13[i + 1], this->marketplace);
						} else if(v13[i] == "options.fogenabled") {
							this->readBool(v13[i + 1], this->fogEnabled);
						} else if(v13[i] == "options.showfps") {
							this->readBool(v13[i + 1], this->showFps);
						} else if(v13[i] == "options.debugscreen") {
							this->readBool(v13[i + 1], this->debugScreen);
						} else if(v13[i] == "options.discordrpc") {
							this->readBool(v13[i + 1], this->discordIntegration);
						} else if(v13[i] == "options.animatewater") {
							this->readBool(v13[i + 1], this->animateWater);
						} else if(v13[i] == "options.animatelava") {
							this->readBool(v13[i + 1], this->animateLava);
						} else if(v13[i] == "options.animatefire") {
							this->readBool(v13[i + 1], this->animateFire);
						} else if(v13[i] == "options.chatcolor") {
							this->readInt(v13[i + 1], this->chatColor);
						} else if(v13[i] == "options.chatbgcolor") {
							this->readInt(v13[i + 1], this->chatBgColor);
						} else if(v13[i] == OptionStrings::Game_ThirdPerson) {
							this->readBool(v13[i + 1], this->thirdPerson);
						} else if(v13[i] == OptionStrings::Controls_UseTouchScreen) {
							if(this->minecraft->supportNonTouchscreen()) {
								this->readBool(v13[i + 1], this->useTouchscreen);
							}
							this->useTouchscreen = 0;
						} else {
							if(v13[i] == OptionStrings::Graphics_HideGUI) {
								this->readBool(v13[i + 1], this->hideGUI);
							} else if(v13[i] == OptionStrings::AUDIO_Sound) {
								this->readFloat(v13[i + 1], this->soundVolume);
							} else if(!(v13[i] == OptionStrings::Game_DifficultyLevel)) {
								if(v13[i] == OptionStrings::Last_Game_Version_Major) {
									this->readInt(v13[i + 1], this->major);
								} else if(v13[i] == OptionStrings::Last_Game_Version_Minor) {
									this->readInt(v13[i + 1], this->minor);
								} else if(v13[i] == OptionStrings::Last_Game_Version_Patch) {
									this->readInt(v13[i + 1], this->patch);
								} else {
									if(!(v13[i] == OptionStrings::Last_Game_Version_Beta)) {
										continue;
									}
									this->readInt(v13[i + 1], this->beta);
								}
							} else {
								this->readInt(v13[i + 1], this->difficulty);
								if(this->difficulty) {
									if(this->difficulty != 2) {
										this->difficulty = 2;
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void Options::validateVersion(void) {
	if(this->major || this->minor != 8 || this->patch != 1 || this->beta) {
		this->minecraft->onUpdatedClient(this->major, this->minor, this->patch, this->beta);
		this->major = 0;
		this->minor = 8;
		this->beta = 0;
		this->patch = 1;
		this->save();
	}
}

void Options::toggle(const Options::Option* a2, int32_t a3) {
	bool_t bv; // r0

	if(a2 == &Options::Option::INVERT_MOUSE) {
		this->invertMouse ^= 1u;
	} else if(a2 == &Options::Option::RENDER_DISTANCE) {
		int idx = 0;
		for(int i = 0; i < Options::RENDERDISTANCE_LEVELS.size(); i++) {
			if(Options::RENDERDISTANCE_LEVELS[i] == this->renderDistance) {
				idx = i;
				break;
			}
		}
		idx = (idx + a3) % Options::RENDERDISTANCE_LEVELS.size();
		if(idx < 0) idx += Options::RENDERDISTANCE_LEVELS.size();
		this->renderDistance = Options::RENDERDISTANCE_LEVELS[idx];
	} else if(a2 == &Options::Option::GUI_SCALE) {
		this->guiScale = (a3 + (uint8_t)this->guiScale) & 3;
	} else if(a2 == &Options::Option::VIEW_BOBBING) {
		this->viewBobbing ^= 1u;
	} else if(a2 == &Options::Option::THIRD_PERSON) {
		this->thirdPerson = (this->thirdPerson + 1) % 3;
	} else if(a2 == &Options::Option::HIDE_GUI) {
		this->hideGUI ^= 1u;
	} else if(a2 == &Options::Option::SERVER_VISIBLE) {
		this->serverVisible ^= 1u;
	} else if(a2 == &Options::Option::LEFT_HANDED) {
		this->leftHanded ^= 1u;
	} else if(a2 == &Options::Option::USE_TOUCHSCREEN) {
		this->useTouchscreen ^= 1u;
	} else if(a2 == &Options::Option::USE_TOUCH_JOYPAD) {
		this->useJoypad ^= 1u;
	} else if(a2 == &Options::Option::DESTROY_VIBRATION) {
		this->destroyVibration ^= 1u;
	} else if(a2 == &Options::Option::GRAPHICS) {
		this->graphics ^= 1u;
	} else if(a2 == &Options::Option::FANCY_SKIES) {
		this->fancySkies ^= 1u;
	} else if(a2 == &Options::Option::ANIMATE_TEXTURES) {
		this->animateTextures ^= 1u;
	} else if(a2 == &Options::Option::SPRINT) {
		this->sprintEnabled ^= 1u;
	} else if(a2 == &Options::Option::AUTO_JUMP) {
		this->autoJump ^= 1u;
	} else if(a2 == &Options::Option::HUD_CAMERA_BUTTON) {
		this->hudCameraButton ^= 1u;
	} else if(a2 == &Options::Option::SHOW_COORDINATES) {
		this->showCoordinates ^= 1u;
	} else if(a2 == &Options::Option::CLASSIC_TEXTURES) {
		this->classicTextures ^= 1u;
	} else if(a2 == &Options::Option::MARKETPLACE) {
		this->marketplace ^= 1u;
	} else if(a2 == &Options::Option::FOG_ENABLED) {
		this->fogEnabled ^= 1u;
	} else if(a2 == &Options::Option::SHOW_FPS) {
		this->showFps ^= 1u;
	} else if(a2 == &Options::Option::DEBUG_SCREEN) {
		this->debugScreen ^= 1u;
	} else if(a2 == &Options::Option::DISCORD_RPC) {
		this->discordIntegration ^= 1u;
	} else if(a2 == &Options::Option::ANIMATE_WATER) {
		this->animateWater ^= 1u;
	} else if(a2 == &Options::Option::ANIMATE_LAVA) {
		this->animateLava ^= 1u;
	} else if(a2 == &Options::Option::ANIMATE_FIRE) {
		this->animateFire ^= 1u;
	} else if(a2 == &Options::Option::LIMIT_FRAMERATE) {
		this->limitFramerate ^= 1u;
	} else if(a2 == &Options::Option::DIFFICULTY) {
		this->difficulty = (a3 + (uint8_t)this->difficulty) & 3;
	}
	bv = this->getBooleanValue(a2);
	this->notifyOptionUpdate(a2, bv);
	this->save();
}
void Options::setKey(int32_t a2, int32_t a3) {
	this->keys[a2]->keyCode = a3;
	this->save();
}
void Options::setAdditionalHiddenOptions(const std::vector<const Options::Option*>& a2) {
	this->hiddenOptionsVec = a2;
}
void Options::set(const Options::Option* a2, std::string a3) {
	if(&Options::Option::NAME == a2) {
		this->username = a3;
		if(this->minecraft->user) {
			this->minecraft->user->username = a3.empty() ? "Steve" : a3;
		}
	}
}
void Options::set(const Options::Option* a2, int32_t a3) {
	if(a2 == &Options::Option::DIFFICULTY) {
		this->difficulty = a3;
	} else if(a2 == &Options::Option::RENDER_DISTANCE) {
		this->renderDistance = a3;
	} else if(a2 == &Options::Option::CHAT_COLOR) {
		this->chatColor = a3;
	} else if(a2 == &Options::Option::CHAT_BG_COLOR) {
		this->chatBgColor = a3;
	} else if(a2 == &Options::Option::NEW_ADDITIONS) {
		this->newAdditions = a3;
	}
	this->notifyOptionUpdate(a2, a3);
}
void Options::set(const Options::Option* a2, float a3) {
	if(a2 == &Options::Option::MUSIC) {
		this->musicVolume = a3;
	} else if(a2 == &Options::Option::SOUND) {
		this->soundVolume = a3;
	} else if(a2 == &Options::Option::FOV) {
		this->fov = a3;
	} else if(a2 == &Options::Option::SENSITIVITY) {
		this->sensitity = a3;
	} else if(a2 == &Options::Option::PIXELS_PER_MILLIMETER) {
		this->pixelDensity = a3;
	}
	this->notifyOptionUpdate(a2, a3);
}
void Options::save(void) {
	std::vector<std::string> v4;
	this->addOptionToSaveOutput(v4, OptionStrings::Multiplayer_Username, this->username);
	this->addOptionToSaveOutput(v4, OptionStrings::Game_DifficultyLevel, this->difficulty);
	this->addOptionToSaveOutput(v4, OptionStrings::Game_ThirdPerson, this->thirdPerson);
	this->addOptionToSaveOutput(v4, OptionStrings::Graphics_PixelsPerMilimeter, this->pixelDensity);
	this->addOptionToSaveOutput(v4, OptionStrings::Multiplayer_ServerVisible, this->serverVisible);
	this->addOptionToSaveOutput(v4, OptionStrings::Controls_Sensitivity, this->sensitity);
	this->addOptionToSaveOutput(v4, OptionStrings::Controls_InvertMouse, this->invertMouse);
	this->addOptionToSaveOutput(v4, OptionStrings::Controls_IsLefthanded, this->leftHanded);
	this->addOptionToSaveOutput(v4, OptionStrings::Controls_UseTouchScreen, this->useTouchscreen);
	this->addOptionToSaveOutput(v4, OptionStrings::Controls_UseTouchJoypad, this->useJoypad);
	this->addOptionToSaveOutput(v4, OptionStrings::Controls_FeedbackVibration, this->destroyVibration);
	this->addOptionToSaveOutput(v4, OptionStrings::Graphics_RenderDistance, this->renderDistance);
	this->addOptionToSaveOutput(v4, OptionStrings::Graphics_FancyGraphics, this->graphics);
	this->addOptionToSaveOutput(v4, OptionStrings::Graphics_FancySkies, this->fancySkies);
	this->addOptionToSaveOutput(v4, OptionStrings::Graphics_AnimateTextures, this->animateTextures);
	this->addOptionToSaveOutput(v4, "options.newadditions", this->newAdditions);
	this->addOptionToSaveOutput(v4, "options.sprint", this->sprintEnabled);
	this->addOptionToSaveOutput(v4, "options.autojump", this->autoJump);
	this->addOptionToSaveOutput(v4, "options.fov", this->fov);
	this->addOptionToSaveOutput(v4, "options.hudcamerabutton", this->hudCameraButton);
	this->addOptionToSaveOutput(v4, "options.showcoordinates", this->showCoordinates);
	this->addOptionToSaveOutput(v4, "options.classictextures", this->classicTextures);
	this->addOptionToSaveOutput(v4, "options.marketplace", this->marketplace);
	this->addOptionToSaveOutput(v4, "options.fogenabled", this->fogEnabled);
	this->addOptionToSaveOutput(v4, "options.showfps", this->showFps);
	this->addOptionToSaveOutput(v4, "options.debugscreen", this->debugScreen);
	this->addOptionToSaveOutput(v4, "options.discordrpc", this->discordIntegration);
	this->addOptionToSaveOutput(v4, "options.animatewater", this->animateWater);
	this->addOptionToSaveOutput(v4, "options.animatelava", this->animateLava);
	this->addOptionToSaveOutput(v4, "options.animatefire", this->animateFire);
	this->addOptionToSaveOutput(v4, "options.chatcolor", this->chatColor);
	this->addOptionToSaveOutput(v4, "options.chatbgcolor", this->chatBgColor);
	this->addOptionToSaveOutput(v4, OptionStrings::Graphics_HideGUI, this->hideGUI);
	this->addOptionToSaveOutput(v4, OptionStrings::AUDIO_Sound, this->soundVolume);
	this->addOptionToSaveOutput(v4, OptionStrings::Last_Game_Version_Major, this->major);
	this->addOptionToSaveOutput(v4, OptionStrings::Last_Game_Version_Minor, this->minor);
	this->addOptionToSaveOutput(v4, OptionStrings::Last_Game_Version_Patch, this->patch);
	this->addOptionToSaveOutput(v4, OptionStrings::Last_Game_Version_Beta, this->beta);
	this->settingFolderPath.save(v4);
}
bool_t Options::readInt(const std::string& a1, int32_t& a2) {
	if(a1 == "true" || a1 == "YES") {
		a2 = 1;
		return 1;
	} else {
		if(a1 == "false" || a1 == "NO") {
			a2 = 0;
			return 1;
		}
		return sscanf(a1.c_str(), "%d", &a2) != 0;
	}
}
bool_t Options::readFloat(const std::string& a1, float& a2) {
	if(a1 == "true" || a1 == "YES") {
		a2 = 1;
		return 1;
	} else {
		if(a1 == "false" || a1 == "NO") {
			a2 = 0;
			return 1;
		}
		return sscanf(a1.c_str(), "%f", &a2) != 0;
	}
}
bool_t Options::readBool(const std::string& r0, bool_t& a2) {
	std::string a1 = Util::stringTrim(r0);
	if(a1 == "true" || a1 == "1" || a1 == "YES") {
		a2 = 1;
		return 1;
	} else {
		if(a1 == "false" || a1 == "0" || a1 == "NO") {
			a2 = 0;
			return 1;
		}
		return 0;
	}
}
void Options::notifyOptionUpdate(const Options::Option* a2, int32_t a3) {
	this->minecraft->optionUpdated(a2, a3);
}
void Options::notifyOptionUpdate(const Options::Option* a2, float a3) {
	this->minecraft->optionUpdated(a2, a3);
}
void Options::notifyOptionUpdate(const Options::Option* a2, bool_t a3) {
	this->minecraft->optionUpdated(a2, a3);
}
void Options::load(void) {
}
void Options::initDefaultValues(void) {
	this->beta = 0;
	this->field_F0 = 1;
	this->patch = 0;
	this->minor = 0;
	this->major = 0;
	this->hideGUI = 0;
	this->thirdPerson = 0;
	this->field_EE = 0;
	this->enableNoclipMaybe = 0;
	this->field_F1 = 0;
	this->difficulty = 2;
	this->destroyVibration = 1;
	this->viewBobbing = 1;
	this->graphics = 1;
	this->fancySkies = 1;
	this->animateTextures = 1;
	this->newAdditions = 0;
	this->sprintEnabled = 1;
	this->autoJump = 1;
	this->hudCameraButton = 1;
	this->showCoordinates = 1;
	this->classicTextures = 0;
	this->marketplace = 1;
	this->fogEnabled = 1;
	this->showFps = 0;
	this->debugScreen = 0;
	this->discordIntegration = 1;
	this->animateWater = 1;
	this->animateLava = 1;
	this->animateFire = 1;
	this->chatColor = 0;
	this->chatBgColor = 0;
	this->field_F4 = 1.0;
	this->field_F8 = 1.0;
	this->fov = 70.0f;
	this->guiScale = 0;
	this->field_17 = 0;
	this->leftHanded = 0;
	this->useJoypad = 0;
	this->musicVolume = 1.0;
	this->soundVolume = 1.0;
	this->invertMouse = 0;
	this->sensitity = 0.5;
	this->limitFramerate = 0;
	this->renderDistance = 2;
	this->useTouchscreen = this->minecraft->supportNonTouchscreen();
	float v4 = this->minecraft->platform()->getPixelsPerMillimeter();
	if(v4 > 12) v4 = 12;
	else if(v4 <= 3) v4 = 3;

	this->pixelDensity = v4;
	this->username = "Steve";
	this->serverVisible = 1;
	this->keyForward = KeyMapping("key.forward", 87);
	this->keyLeft = KeyMapping("key.left", 65);
	this->keyBack = KeyMapping("key.back", 83);
	this->keyRight = KeyMapping("key.right", 68);
	this->keyJump = KeyMapping("key.jump", 32);
	this->keyInventory = KeyMapping("key.inventory", 69);
	this->keySneak = KeyMapping("key.sneak", 10);
	this->keyCrafting = KeyMapping("key.crafting", 81);
	this->keyDrop = KeyMapping("key.drop", 81);
	this->keyChat = KeyMapping("key.chat", 84);
	this->keyFog = KeyMapping("key.fog", 70);
	this->keyDestroy = KeyMapping("key.destroy", 88);
	this->keyUse = KeyMapping("key.use", 85);
	this->keyMenuNext = KeyMapping("key.menu.next", 40);
	this->keyMenuPrevious = KeyMapping("key.menu.previous", 38);
	this->keyMenuOk = KeyMapping("key.menu.ok", 13);
	this->keyMenuCancel = KeyMapping("key.menu.cancel", 8);
	this->keys[14] = &this->keyMenuOk;
	this->keys[15] = &this->keyMenuCancel;
	this->keys[2] = &this->keyBack;
	this->thirdPerson = 0;
	this->field_17 = 0;
	this->keys[3] = &this->keyRight;
	this->keyForward.keyCode = 19;
	this->keyLeft.keyCode = 21;
	this->keys[4] = &this->keyJump;
	this->keyRight.keyCode = 22;
	this->keyUse.keyCode = 103;
	this->keys[5] = &this->keySneak;
	this->keyJump.keyCode = 23;
	this->keys[0] = &this->keyForward;
	this->keys[6] = &this->keyDrop;
	this->keys[1] = &this->keyLeft;
	this->keys[10] = &this->keyDestroy;
	this->keys[7] = &this->keyInventory;
	this->keys[11] = &this->keyUse;
	this->keys[12] = &this->keyMenuNext;
	this->keys[8] = &this->keyChat;
	this->keyBack.keyCode = 20;
	this->keys[9] = &this->keyFog;
	this->keys[13] = &this->keyMenuPrevious;
	this->keyDestroy.keyCode = 102;
	this->keyMenuPrevious.keyCode = 19;
	this->keyMenuOk.keyCode = 23;
	this->keyCrafting.keyCode = 109;
	this->keyMenuCancel.keyCode = 4;
	this->keyMenuNext.keyCode = 20;
	this->setAdditionalHiddenOptions({});
}
bool_t Options::hideOption(const Options::Option* a2) {
	if(a2 == &Options::Option::USE_TOUCHSCREEN && !this->minecraft->supportNonTouchscreen()) {
		return 1;
	}
	for(int32_t i = 0; i < this->hiddenOptionsVec.size(); ++i) {
		if(this->hiddenOptionsVec[i] == a2) {
			return 1;
		}
	}
	return 0;
}
std::vector<int> Options::getValues(const Options::Option* a2) {
	if(a2 == &Options::Option::DIFFICULTY) {
		return Options::DIFFICULTY_LEVELS;
	}
	if(a2 == &Options::Option::RENDER_DISTANCE) {
		return Options::RENDERDISTANCE_LEVELS;
	}
	if(a2 == &Options::Option::CHAT_COLOR) {
		return Options::CHAT_COLOR_LEVELS;
	}
	if(a2 == &Options::Option::CHAT_BG_COLOR) {
		return Options::CHAT_BG_COLOR_LEVELS;
	}
	if(a2 == &Options::Option::NEW_ADDITIONS) {
		return Options::NEW_ADDITIONS_LEVELS;
	}
	return {};
}
std::string Options::getStringValue(const Options::Option* a2) {
	if(&Options::Option::NAME == a2) {
		return this->username;
	}
	return "";
}
float Options::getProgrssMin(const Options::Option* a2) {
	if(a2 == &Options::Option::MUSIC || a2 == &Options::Option::SOUND || a2 == &Options::Option::SENSITIVITY) {
		return 0;
	}
	if(a2 == &Options::Option::FOV) {
		return 30.0f;
	}
	if(a2 == &Options::Option::PIXELS_PER_MILLIMETER) {
		return 3;
	}
	return 0;
}
float Options::getProgrssMax(const Options::Option* a2) {
	if(a2 == &Options::Option::MUSIC || a2 == &Options::Option::SOUND || a2 == &Options::Option::SENSITIVITY) {
		return 1;
	}
	if(a2 == &Options::Option::FOV) {
		return 110.0f;
	}
	if(a2 == &Options::Option::PIXELS_PER_MILLIMETER) {
		return 12;
	}
	return 1;
}
float Options::getProgressValue(const Options::Option* a2) {
	if(a2 == &Options::Option::MUSIC) {
		return this->musicVolume;
	}
	if(a2 == &Options::Option::SOUND) {
		return this->soundVolume;
	}
	if(a2 == &Options::Option::SENSITIVITY) {
		return this->sensitity;
	}
	if(a2 == &Options::Option::FOV) {
		return this->fov;
	}
	if(a2 == &Options::Option::PIXELS_PER_MILLIMETER) {
		return this->pixelDensity;
	}
	return 0;
}
std::string Options::getMessage(const Options::Option*) {
	return "Options::getMessage - Not implemented"; //actual mcpe code
}
std::string Options::getKeyMessage(int32_t) {
	return "Options::getKeyMessage not implemented"; //actual mcpe code
}
std::string Options::getKeyDescription(int32_t) {
	return "Options::getKeyDescription not implemented"; //yes
}
int32_t Options::getIntValue(const Options::Option* a2) {
	if(a2 == &Options::Option::DIFFICULTY) {
		return this->difficulty;
	}
	if(a2 == &Options::Option::RENDER_DISTANCE) {
		return this->renderDistance;
	}
	if(a2 == &Options::Option::CHAT_COLOR) {
		return this->chatColor;
	}
	if(a2 == &Options::Option::CHAT_BG_COLOR) {
		return this->chatBgColor;
	}
	if(a2 == &Options::Option::NEW_ADDITIONS) {
		return this->newAdditions;
	}
	return 0;
}
std::string Options::getDescription(const Options::Option* a2, std::string a4) {
	if(a2 == &Options::Option::CHAT_COLOR) {
		std::string colors[] = {"White", "Gray", "Yellow", "Green", "Red", "Blue", "Gold", "Aqua", "Purple"};
		if(this->chatColor >= 0 && this->chatColor <= 8) return a4 + ": " + colors[this->chatColor];
	}
	if(a2 == &Options::Option::CHAT_BG_COLOR) {
		std::string colors[] = {"Black", "Dark Red", "Dark Green", "Dark Blue", "Transparent"};
		if(this->chatBgColor >= 0 && this->chatBgColor <= 4) return a4 + ": " + colors[this->chatBgColor];
	}
	if(a2 == &Options::Option::NEW_ADDITIONS) {
		std::string modes[] = {"Modded", "New Additions"};
		if(this->newAdditions >= 0 && this->newAdditions <= 1) return a4 + ": " + modes[this->newAdditions];
	}
	if(a2 == &Options::Option::RENDER_DISTANCE) {
		std::string modes[] = {"Tiny", "Short", "Normal", "Far", "Very Far", "Ultra", "Extreme"};
		int idx = 3 - this->renderDistance;
		if(idx >= 0 && idx < 7) return a4 + ": " + modes[idx];
	}
	return a4;
}
bool_t Options::getBooleanValue(const Options::Option* a2) {
	if(a2 == &Options::Option::INVERT_MOUSE) {
		return this->invertMouse;
	}
	if(a2 == &Options::Option::VIEW_BOBBING) {
		return this->viewBobbing;
	}
	if(a2 == &Options::Option::LIMIT_FRAMERATE) {
		return this->limitFramerate;
	}
	if(a2 == &Options::Option::THIRD_PERSON) {
		return this->thirdPerson;
	}
	if(a2 == &Options::Option::HIDE_GUI) {
		return this->hideGUI;
	}
	if(a2 == &Options::Option::SERVER_VISIBLE) {
		return this->serverVisible;
	}
	if(a2 == &Options::Option::LEFT_HANDED) {
		return this->leftHanded;
	}
	if(a2 == &Options::Option::USE_TOUCHSCREEN) {
		return this->useTouchscreen;
	}
	if(a2 == &Options::Option::USE_TOUCH_JOYPAD) {
		return this->useJoypad;
	}
	if(a2 == &Options::Option::DESTROY_VIBRATION) {
		return this->destroyVibration;
	}
	if(a2 == &Options::Option::FANCY_SKIES) {
		return this->fancySkies;
	}
	if(a2 == &Options::Option::ANIMATE_TEXTURES) {
		return this->animateTextures;
	}
	if(a2 == &Options::Option::SPRINT) {
		return this->sprintEnabled;
	}
	if(a2 == &Options::Option::AUTO_JUMP) {
		return this->autoJump;
	}
	if(a2 == &Options::Option::HUD_CAMERA_BUTTON) {
		return this->hudCameraButton;
	}
	if(a2 == &Options::Option::SHOW_COORDINATES) {
		return this->showCoordinates;
	}
	if(a2 == &Options::Option::CLASSIC_TEXTURES) {
		return this->classicTextures;
	}
	if(a2 == &Options::Option::MARKETPLACE) {
		return this->marketplace;
	}
	if(a2 == &Options::Option::FOG_ENABLED) {
		return this->fogEnabled;
	} else if(a2 == &Options::Option::SHOW_FPS) {
		return this->showFps;
	} else if(a2 == &Options::Option::DEBUG_SCREEN) {
		return this->debugScreen;
	} else if(a2 == &Options::Option::DISCORD_RPC) {
		return this->discordIntegration;
	} else if(a2 == &Options::Option::ANIMATE_WATER) {
		return this->animateWater;
	} else if(a2 == &Options::Option::ANIMATE_LAVA) {
		return this->animateLava;
	} else if(a2 == &Options::Option::ANIMATE_FIRE) {
		return this->animateFire;
	}
	if(a2 == &Options::Option::GRAPHICS) {
		return this->graphics;
	}
	return 0;
}
std::string Options::formatDescriptionString(const Options::Option* a2, const char_t* a3, const char_t** a4, int32_t a5) {
	char_t v9[100];
	std::string v8 = I18n::get(a4[a5]);
	sprintf(v9, a3, v8.c_str());
	return v9;
}
bool_t Options::canModify(const Options::Option* a2) {
	return &Options::Option::NAME != a2 || this->minecraft->mojangConnector->getConnectionStatus() == STATUS_0;
}
void Options::addOptionToSaveOutput(std::vector<std::string>& a2, std::string a3, std::string a4) {
	std::stringstream v11;
	v11 << a3;
	v11 << ":";
	v11 << a4;
	a2.emplace_back(v11.str());
}
void Options::addOptionToSaveOutput(std::vector<std::string>& a2, std::string a3, int32_t a4) {
	std::stringstream v11;
	v11 << a3;
	v11 << ":";
	v11 << a4;
	a2.emplace_back(v11.str());
}
void Options::addOptionToSaveOutput(std::vector<std::string>& a2, std::string a3, float a4) {
	std::stringstream v11;
	v11 << a3;
	v11 << ":";
	v11 << a4;
	a2.emplace_back(v11.str());
}
void Options::addOptionToSaveOutput(std::vector<std::string>& a2, std::string a3, bool a4) {
	std::stringstream v11;
	v11 << a3;
	v11 << ":";
	v11 << a4;
	a2.emplace_back(v11.str());
}

void Options::init(Minecraft* mc, std::string a3) {
	Options::instance = this;
	this->minecraft = mc;
	this->settingFolderPath.setSettingsFolderPath(a3);
	this->showFps = 0;
	this->debugScreen = 0;
	this->initDefaultValues();
}
