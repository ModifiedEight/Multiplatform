#ifndef ANDROID
#include <AppPlatform_sdl.hpp>
#include <DiscordRPC.hpp>
#include <NinecraftApp.hpp>
#include <_AssetFile.hpp>
#include <entity/LocalPlayer.hpp>
#include <input/Keyboard.hpp>
#include <input/KeyboardInput.hpp>
#include <input/Mouse.hpp>
#include <inventory/Inventory.hpp>
#include <level/Level.hpp>
#include <m8_icon.h>
#include <main.hpp>
#include <network/mco/LoginInformation.hpp>
#include <rendering/Font.hpp>
#include <rendering/Tesselator.hpp>
#include <rendering/Textures.hpp>
#include <stb_image.h>
#include <utils.h>

#include <item/Item.hpp>
#include <tile/Tile.hpp>
#include <tile/entity/MixedSlabTileEntity.hpp>

static void performPickBlock(Minecraft *mc) {
  if (!mc || !mc->player || !mc->level || !mc->player->inventory)
    return;
  if (mc->selectedObject.hitType != 0)
    return;
  int x = mc->selectedObject.field_4;
  int y = mc->selectedObject.field_8;
  int z = mc->selectedObject.field_C;
  int tileId = mc->level->getTile(x, y, z);
  if (tileId <= 0)
    return;
  int data = mc->level->getData(x, y, z);

  int pickId = tileId;
  int pickData = data;

  Tile *tile = Tile::tiles[tileId];
  if (tile) {
    if (tile == Tile::mixedSlab) {
      MixedSlabTileEntity *te =
          (MixedSlabTileEntity *)mc->level->getTileEntity(x, y, z);
      if (te) {
        int mode = te->mode;
        float hitCoord = 0.5f;
        if (mode == 1) {
          hitCoord = mc->selectedObject.hitVec.z - (float)z;
        } else if (mode == 2) {
          hitCoord = mc->selectedObject.hitVec.x - (float)x;
        } else {
          hitCoord = mc->selectedObject.hitVec.y - (float)y;
        }
        bool hitTop = (hitCoord >= 0.5f);
        if (te->bottomTileId > 0 && te->topTileId == 0)
          hitTop = false;
        if (te->topTileId > 0 && te->bottomTileId == 0)
          hitTop = true;

        if (hitTop && te->topTileId > 0) {
          pickId = te->topTileId;
          pickData = te->topAux;
        } else if (te->bottomTileId > 0) {
          pickId = te->bottomTileId;
          pickData = te->bottomAux;
        }
      }
    } else if (tile == Tile::sign || tile == Tile::wallSign) {
      pickId = Item::sign ? Item::sign->itemID : 323;
      pickData = 0;
    } else if (tile == Tile::door_wood) {
      pickId = Item::door_wood ? Item::door_wood->itemID : 324;
      pickData = 0;
    } else if (Tile::door_iron && tile == Tile::door_iron) {
      pickId = Item::door_iron ? Item::door_iron->itemID : 330;
      pickData = 0;
    } else if (Tile::bed && tile == Tile::bed) {
      pickId = Item::bed ? Item::bed->itemID : 355;
      pickData = 0;
    } else if (Tile::reeds && tile == Tile::reeds) {
      pickId = Item::reeds ? Item::reeds->itemID : 338;
      pickData = 0;
    } else if (Tile::cake && tile == Tile::cake) {
      pickId = Item::cake ? Item::cake->itemID : 354;
      pickData = 0;
    } else if (Tile::crops && tile == Tile::crops) {
      pickId = Item::seeds_wheat ? Item::seeds_wheat->itemID : 295;
      pickData = 0;
    } else if (tile == Tile::farmland) {
      pickId = Tile::dirt->blockID;
      pickData = 0;
    } else if (Tile::redStoneOre_lit && tile == Tile::redStoneOre_lit) {
      pickId = Tile::redStoneOre->blockID;
    } else if (Tile::furnace_lit && tile == Tile::furnace_lit) {
      pickId = Tile::furnace->blockID;
    } else if (Tile::litPumpkin && tile == Tile::litPumpkin) {
      pickId = Tile::pumpkin->blockID;
    } else if (tile == Tile::topSnow) {
      pickId = Tile::snow->blockID;
      pickData = 0;
    } else if (tile == Tile::water || tile == Tile::calmWater) {
      pickId = Tile::water->blockID;
    } else if (tile == Tile::lava || tile == Tile::calmLava) {
      pickId = Tile::lava->blockID;
    } else if (Tile::stoneSlab &&
               (tile == Tile::stoneSlab || tile == Tile::stoneSlabHalf)) {
      pickId = Tile::stoneSlabHalf ? Tile::stoneSlabHalf->blockID : 44;
      pickData = data & 7;
    } else if (Tile::woodSlab &&
               (tile == Tile::woodSlab || tile == Tile::woodSlabHalf)) {
      pickId = Tile::woodSlabHalf ? Tile::woodSlabHalf->blockID : 158;
      pickData = data & 7;
    } else if (Tile::treeTrunk && tile == Tile::treeTrunk) {
      pickData = data & 3;
    } else if (Tile::leaves && tile == Tile::leaves) {
      pickData = data & 3;
    } else if (Tile::sapling && tile == Tile::sapling) {
      pickData = data & 7;
    } else if (Tile::cloth && tile == Tile::cloth) {
      pickData = data & 15;
    } else if (Tile::wood && tile == Tile::wood) {
      pickData = data & 15;
    }
  }

  Inventory *inv = mc->player->inventory;
  int selSlot = inv->selectedSlot;
  int existingHotbarSlot = inv->getLinkedSlotForItemAndAux(pickId, pickData);
  if (existingHotbarSlot >= 0 && existingHotbarSlot < 9) {
    inv->selectSlot(existingHotbarSlot);
  } else {
    ItemInstance *newInst =
        new ItemInstance(pickId, inv->field_20 ? 64 : 1, pickData);
    if (inv->field_20) {
      int targetIdx = selSlot + 9;
      inv->linkSlot(selSlot, targetIdx);
      inv->replaceSlot(targetIdx, newInst);
      inv->replaceSlot(selSlot, newInst);
    } else {
      int foundSlot = -1;
      for (size_t i = 9; i < inv->items.size(); ++i) {
        if (inv->items[i] && inv->items[i]->getId() == pickId &&
            inv->items[i]->getAuxValue() == pickData) {
          foundSlot = (int)i;
          break;
        }
      }
      if (foundSlot >= 0) {
        inv->linkSlot(selSlot, foundSlot);
      }
    }
  }
  mc->gui.inventoryUpdated();
}

#ifdef USEGLES
#include <GLES/glext.h>
#else

#endif
#ifdef __WIN32__
#include <sound/SoundSystemDirectSound.hpp>
#endif
std::string AppPlatform_sdl::getImagePath(const std::string &name, bool_t t) {
  return "assets/images/" + name;
}

AssetFile AppPlatform_sdl::readAssetFile(const std::string &path) {
  return AppPlatform::readAssetFile("assets/" + path);
}

void AppPlatform_sdl::loadPNG(ImageData &data, const std::string &path,
                              bool_t t) {
  int32_t channels;
  AssetFile file = this->readAssetFile(path);
  if (file.bytes && file.length > 0) {
    uint8_t *pixels =
        stbi_load_from_memory(file.bytes, file.length, &data.width,
                              &data.height, &channels, STBI_rgb_alpha);
    delete[] file.bytes;
    if (pixels) {
      data.field_C = 0;
      data.pixels = pixels;
      return;
    }
  }

  uint8_t *pixels = stbi_load(path.c_str(), &data.width, &data.height,
                              &channels, STBI_rgb_alpha);
  if (!pixels) {
    printf("failed to load %s.\n", path.c_str());
    return;
  }

  printf("%d channels\n", channels);
  data.field_C = 0;
  data.pixels = pixels;
}

/*void AppPlatform_sdl::updateStatsUserData(const std::string&, const
std::string&){ printf("AppPlatform_sdl::updateStatsUserData: not
implemented\n");
}*/

SDL_Surface *AppPlatform_sdl::setSDLVideoMode() {
  return SDL_SetVideoMode(this->screenWidth, this->screenHeight, 32,
                          SDL_OPENGL | SDL_RESIZABLE);
}

bool_t AppPlatform_sdl::sdlCtxInit() {
  if (this->hasContext)
    return 1;

  SDL_Init(SDL_INIT_VIDEO);
  SDL_WM_SetCaption("ModifiedEight New Additions 1.6.1.1", 0);

  {
    int w, h, ch;
    unsigned char *pixels = stbi_load_from_memory(
        g_iconJpgData, (int)g_iconJpgSize, &w, &h, &ch, 4);
    if (!pixels) {
      pixels = stbi_load("icon.jpg", &w, &h, &ch, 4);
    }
    if (pixels) {
      SDL_Surface *icon =
          SDL_CreateRGBSurfaceFrom(pixels, w, h, 32, w * 4, 0x000000FF,
                                   0x0000FF00, 0x00FF0000, 0xFF000000);
      if (icon) {
        SDL_WM_SetIcon(icon, nullptr);
        SDL_FreeSurface(icon);
      }
      stbi_image_free(pixels);
    }
  }

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_EnableUNICODE(1);
  this->sdl_surface = this->setSDLVideoMode();
  if (!this->sdl_surface) {
    return 0;
  }
#ifndef USEGLES
  initGlFuncs();
#endif

#ifdef PCTWEAKS
  int maxTextureLevel, anisotropicFilter;

  const char_t *str = (const char_t *)glGetString(GL_EXTENSIONS);
  std::string exts = str ? str : "";
  // opengl (non es) doesnt seem to require ext for it
#ifdef USEGLES
  maxTextureLevel = GL_TEXTURE_MAX_LEVEL_APPLE;
#else
  maxTextureLevel = GL_TEXTURE_MAX_LEVEL;
#endif
  AppPlatform::TEXTURE_MAX_LEVEL = maxTextureLevel;
  if (exts.find("GL_EXT_texture_filter_anisotropic", 0) == -1)
    anisotropicFilter = 0;
  else
    anisotropicFilter = GL_EXT_texture_filter_anisotropic;
  AppPlatform::ANISOTROPIC_MAX_LEVEL = anisotropicFilter;
  if (anisotropicFilter)
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,
                &AppPlatform::ANISOTROPIC_MAX_LEVEL);
#endif

  return 1;
}

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <ctime>
#include <stb_image_write.h>
#include <sys/stat.h>
#include <unigl.h>
#if defined(_WIN32) || defined(WIN32)
#include <direct.h>
#else
#include <unistd.h>
#endif

static void takeScreenshot(Minecraft *mc, AppPlatform_sdl *platform, int sType = 0) {
  if (!mc || !platform)
    return;
  int w = platform->screenWidth;
  int h = platform->screenHeight;
  if (w <= 0 || h <= 0)
    return;

  std::vector<unsigned char> pixels(w * h * 4);
  glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

  std::vector<unsigned char> flipped(w * h * 4);
  for (int y = 0; y < h; ++y) {
    memcpy(&flipped[y * w * 4], &pixels[(h - 1 - y) * w * 4], w * 4);
  }

  std::string path;
  if (sType == 2 || (mc->options.panoramaAngle > 0 && sType == 0)) {
    int index = mc->options.panoramaAngle - 1;
    if (index < 0)
      index = 0;
#if defined(_WIN32) || defined(WIN32)
    _mkdir("assets");
    _mkdir("assets/images");
    _mkdir("assets/images/gui");
    _mkdir("assets/images/gui/background");
#else
    mkdir("assets", 0755);
    mkdir("assets/images", 0755);
    mkdir("assets/images/gui", 0755);
    mkdir("assets/images/gui/background", 0755);
#endif
    char pBuf[128];
    sprintf(pBuf, "assets/images/gui/background/panorama_%d.png", index);
    path = pBuf;
  } else {
#if defined(_WIN32) || defined(WIN32)
    _mkdir("screenshots");
#else
    mkdir("screenshots", 0755);
#endif
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    char filename[128];
    std::strftime(filename, sizeof(filename), "screenshots/%Y-%m-%d_%H.%M.%S.png",
                  &tm);
    path = filename;
  }

  if (stbi_write_png(path.c_str(), w, h, 4, flipped.data(), w * 4)) {
    std::string msg = "Saved screenshot as " + path;
    mc->gui.displayClientMessage(msg);
  }
}

bool fullscreen = 0;
int lastWidth = 0, lastHeight = 0;
void AppPlatform_sdl::onKeyPressed(Minecraft *mc, SDLKey key, uint8_t scancode,
                                   bool pressed) {
  if (!pressed) {
    SDL_Event next_event;
    if (SDL_PeepEvents(&next_event, 1, SDL_PEEKEVENT, SDL_ALLEVENTS) > 0) {
      if (next_event.type == SDL_KEYDOWN && next_event.key.keysym.sym == key) {
        SDL_PeepEvents(&next_event, 1, SDL_GETEVENT, SDL_ALLEVENTS);
        return;
      }
    }
  }
  int k = 0;
  if (key == SDLK_w || key == 1732 || key == 1764 || scancode == 25)
    k = mc->options.keyForward.keyCode;
  if (key == SDLK_s || key == 1754 || key == 1786 || scancode == 39)
    k = mc->options.keyBack.keyCode;
  if (key == SDLK_a || key == 1735 || key == 1767 || scancode == 38)
    k = mc->options.keyLeft.keyCode;
  if (key == SDLK_d || key == 1752 || key == 1784 || scancode == 40)
    k = mc->options.keyRight.keyCode;
  if (key == SDLK_SPACE || scancode == 65)
    k = mc->options.keyJump.keyCode;
  if (key == SDLK_F11) {
    if (pressed) {
      fullscreen = !fullscreen;
      if (fullscreen) {
        lastWidth = this->screenWidth;
        lastHeight = this->screenHeight;
        this->screenWidth = 0;
        this->screenHeight = 0;
        this->sdl_surface = this->setSDLVideoMode();
        this->screenWidth = SDL_GetVideoInfo()->current_w;
        this->screenHeight = SDL_GetVideoInfo()->current_h;
        mc->setSize(this->screenWidth, this->screenHeight);
      } else {
        this->screenWidth = lastWidth;
        this->screenHeight = lastHeight;
        lastWidth = 0;
        lastHeight = 0;
        mc->setSize(this->screenWidth, this->screenHeight);
      }
      SDL_WM_ToggleFullScreen(this->sdl_surface);
    }
  }
  if (key == SDLK_F1 && mc->player) {
    if (pressed) {
      mc->options.toggle(&Options::Option::HIDE_GUI, 0);
    }
  }
  if (key == SDLK_F2) {
    if (pressed) {
      takeScreenshot(mc, this);
    }
  }
  if ((key == SDLK_l || scancode == 46) && pressed && mc->player && !mc->currentScreen) {
    takeScreenshot(mc, this, 2);
  }
  if (key == SDLK_F5 && mc->player && !mc->currentScreen && mc->mouseGrabbed) {
    if (pressed) {
      mc->options.toggle(&Options::Option::THIRD_PERSON, 0);
    }
  }
  if (key == SDLK_F3 && mc->player && !mc->currentScreen && mc->mouseGrabbed) {
    if (pressed) {
      mc->options.toggle(&Options::Option::DEBUG_SCREEN, 0);
    }
  }
  if (key == SDLK_ESCAPE) {
    if (pressed)
      mc->handleBack(0);
    return;
  }
  if (key == SDLK_e || key == 1750 || key == 1782 || scancode == 26)
    k = 100;
  if ((key == SDLK_t || key == 1733 || key == 1765 || scancode == 28) &&
      mc->mouseGrabbed) {
    mc->screenChooser.setScreen(CHAT_SCREEN);
    return;
  }
  if ((key == SDLK_q || key == 1738 || key == 1770 || scancode == 24) &&
      mc->mouseGrabbed) {
    mc->player->inventory->dropSlot(mc->player->inventory->selectedSlot, 0, 0);
    return;
  }

  if (key == SDLK_BACKSPACE || scancode == 22)
    k = 8;
  if (key == SDLK_RETURN || scancode == 36)
    k = 13;
  if (key == SDLK_LSHIFT || key == SDLK_RSHIFT || key == 304 || key == 303 ||
      key == 1073742049 || key == 1073742053 || scancode == 225 ||
      scancode == 229 || scancode == 50 || scancode == 62)
    k = 10;

  if (key >= SDLK_1 && key <= SDLK_8) {
    if (mc->player && !mc->currentScreen) {
      mc->player->inventory->selectSlot(key - SDLK_1);
    }
  }

  if (k) {
    Keyboard::feed(k, pressed);
  }
}

bool AppPlatform_sdl::supportsTouchscreen() { return 0; }

#include <unistd.h>
#include <util/Util.hpp>
MeshBuffer cube;
void AppPlatform_sdl::init() {
  this->screenWidth = 854;
  this->screenHeight = 480;
  AppContext ctx;

  NinecraftApp *mc = new NinecraftApp();
  this->hasContext = this->sdlCtxInit();
#ifdef __WIN32__
  ((SoundSystemDirectSound *)mc->soundEngine)->init();
#endif
  ctx.platform = this;
  mc->context = ctx;
  char curdir[256];
  getcwd(curdir, 256);
  mc->dataPathMaybe = curdir;
  printf("dir: %s\n", curdir);
  bool_t running = this->hasContext;
  bool_t hasInit = 0;
  Tesselator &tess = Tesselator::instance;
  static const int lbkey = 102;
  static const int rbkey = 103;
  bool windowActive = 1;
  unsigned int lastBuiltTick = 0;
  uint16_t lastMX, lastMY;
  bool prevMGrabbed = 0;
  int discordUpdateCounter = 0;
  int lastDiscordState =
      -1; // -1=unset, 0=menu, 1=marketplace, 2=singleplayer, 3=multiplayer
  bool lastDiscordIntegration = true;
  while (running) {

    if (!hasInit) {
      hasInit = 1;
      mc->init();
      mc->setSize(this->screenWidth, this->screenHeight);
      if (!Options::instance || Options::instance->discordIntegration) {
        DiscordRPC::init("1516425667376451594");
        DiscordRPC::update(
            "Modified MCPE Alpha 0.8.1 client with new stuff", "icon",
            "ModifiedEight New Additions 1.6.1",
            {{"Get Client", "https://modifiedeight.github.io/"}});
      }
    }

    if (hasInit && ++discordUpdateCounter >= 300) {
      discordUpdateCounter = 0;
      int curState = 0;
      std::string details = "In Menu";

      if (mc->currentScreen) {
      }

      if (mc->level && mc->player) {
        if (mc->isOnlineClient()) {
          curState = 3;
          int online = (int)mc->level->playersMaybe.size();
          if (online < 1)
            online = 1;
          details = "Multiplayer";
        } else {
          curState = 2;
          details = "Singleplayer";
        }
      } else if (!mc->level) {
        curState = 0;
        details = "In Menu";
      }

      bool curDiscordIntegration =
          Options::instance ? Options::instance->discordIntegration : true;

      if (curState != lastDiscordState || curState == 3 ||
          curDiscordIntegration != lastDiscordIntegration) {
        lastDiscordState = curState;

        if (!curDiscordIntegration) {
          if (lastDiscordIntegration) {
            DiscordRPC::shutdown();
          }
        } else {
          if (!lastDiscordIntegration) {
            DiscordRPC::init("1516425667376451594");
          }
          int online = 0;
          if (curState == 3 && mc->level)
            online = (int)mc->level->playersMaybe.size();
          if (online < 1 && curState == 3)
            online = 1;
          DiscordRPC::update(
              details, "icon", "ModifiedEight New Additions 1.6.1.1",
              {{"Get Client", "https://modifiedeight.github.io/"}},
              curState == 3 ? online : 0, curState == 3 ? online : 0);
        }
        lastDiscordIntegration = curDiscordIntegration;
      }
    }

    if (mc->mouseGrabbed && !mc->useTouchscreen()) {
      SDL_WM_GrabInput(SDL_GRAB_ON);
      SDL_ShowCursor(SDL_DISABLE);
    } else {
      SDL_WM_GrabInput(SDL_GRAB_OFF);
      SDL_ShowCursor(SDL_ENABLE);
    }

    bool justGrabbed = mc->mouseGrabbed && !prevMGrabbed;
    if (windowActive && !mc->mouseGrabbed && prevMGrabbed) {
      SDL_WarpMouse(this->screenWidth / 2, this->screenHeight / 2);
    }

    prevMGrabbed = mc->mouseGrabbed;

    while (SDL_PollEvent(&appPlatform.sdl_event)) {
      uint16_t _mx, _my;
      switch (appPlatform.sdl_event.type) {
      case SDL_ACTIVEEVENT:
        // wine sends 6 when focus is received for some reason
        //(and 2(SDL_APPINPUTFOCUS) when lost)
        if (appPlatform.sdl_event.active.state == 6 ||
            appPlatform.sdl_event.active.state == SDL_APPINPUTFOCUS) {
          windowActive = appPlatform.sdl_event.active.gain;
          if (!windowActive && !mc->currentScreen) {
            mc->pauseGame(1);
          }
        }
        break;
      case SDL_QUIT:
        running = 0;
        break;
      case SDL_VIDEORESIZE:
        this->screenWidth = appPlatform.sdl_event.resize.w;
        this->screenHeight = appPlatform.sdl_event.resize.h;
        this->sdl_surface = this->setSDLVideoMode();
        if (!this->sdl_surface) {
          printf("setSDLVideoModel: returned 0 after resize! Force quitting\n");
          running = 0;
          break;
        }
        mc->setSize(this->screenWidth, this->screenHeight);
        break;
      case SDL_MOUSEMOTION:
#ifdef __WIN32__
        if (justGrabbed)
          break; // ignore first mouse motion even after grab
#endif
        if (!windowActive)
          break;
        _mx = appPlatform.sdl_event.motion.x;
        _my = appPlatform.sdl_event.motion.y;
        Mouse::feed(0, 0, _mx, _my, appPlatform.sdl_event.motion.xrel,
                    appPlatform.sdl_event.motion.yrel);
        break;
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP:
        if (!windowActive)
          break;
        _mx = appPlatform.sdl_event.button.x;
        _my = appPlatform.sdl_event.button.y;

        if (appPlatform.sdl_event.button.button == SDL_BUTTON_MIDDLE) {
          if (appPlatform.sdl_event.type == SDL_MOUSEBUTTONDOWN) {
            if (mc->player && !mc->currentScreen && mc->isCreativeMode()) {
              performPickBlock(mc);
            }
          }
        }

        if (mc->mouseGrabbed &&
            (appPlatform.sdl_event.button.button == SDL_BUTTON_LEFT ||
             appPlatform.sdl_event.button.button == SDL_BUTTON_RIGHT)) {
          int key = 0;
          if (appPlatform.sdl_event.button.button == SDL_BUTTON_LEFT) {
            key = lbkey;
          }
          if (appPlatform.sdl_event.button.button == SDL_BUTTON_RIGHT) {
            key = rbkey;
          }

          if (key && appPlatform.sdl_event.type == SDL_MOUSEBUTTONDOWN &&
              !Keyboard::_states[key]) {
            Keyboard::feed(key, 1);
          } else if (key && appPlatform.sdl_event.type == SDL_MOUSEBUTTONUP &&
                     Keyboard::_states[key]) {
            Keyboard::feed(key, 0);
          }
        } else if (appPlatform.sdl_event.type == SDL_MOUSEBUTTONDOWN &&
                   (appPlatform.sdl_event.button.button == SDL_BUTTON_WHEELUP ||
                    appPlatform.sdl_event.button.button ==
                        SDL_BUTTON_WHEELDOWN)) {
          int off = appPlatform.sdl_event.button.button == SDL_BUTTON_WHEELDOWN
                        ? 1
                        : -1;
          if (mc->player && !mc->currentScreen) {
            int newslot = mc->player->inventory->selectedSlot + off;
            if (newslot < 0)
              newslot = mc->gui.getNumSlots() -
                        2; // TODO one of the slots is used by inv button
            if (newslot >= mc->gui.getNumSlots() - 1)
              newslot = 0;
            mc->player->inventory->selectSlot(newslot);
          } else if (mc->currentScreen) {
            Mouse::feed(appPlatform.sdl_event.button.button, 1, _mx, _my);
          }
        } else if (appPlatform.sdl_event.button.button == SDL_BUTTON_LEFT ||
                   appPlatform.sdl_event.button.button == SDL_BUTTON_RIGHT) {
          Mouse::feed(
              appPlatform.sdl_event.button.button == SDL_BUTTON_LEFT ? 1 : 2,
              appPlatform.sdl_event.type == SDL_MOUSEBUTTONDOWN, _mx, _my);
        }
        break;
      case SDL_KEYDOWN:
      case SDL_KEYUP:
        if (!windowActive)
          break;
        if (this->keyboardShown && appPlatform.sdl_event.key.keysym.unicode &&
            appPlatform.sdl_event.key.keysym.sym != SDLK_BACKSPACE &&
            appPlatform.sdl_event.key.keysym.sym != SDLK_RETURN &&
            appPlatform.sdl_event.key.keysym.sym != SDLK_ESCAPE) {
          uint16_t c = appPlatform.sdl_event.key.keysym.unicode;
          char p_codepoint[5];
          if (c <= 0x7f) {
            p_codepoint[0] = (char)c;
            p_codepoint[1] = '\x00';
          } else if (c <= 0x7fff) {
            p_codepoint[0] = (char)(0xc0 | ((c >> 6) & 0x1f));
            p_codepoint[1] = (char)(0x80 | ((c & 0x3f)));
            p_codepoint[2] = '\x00';
          } else if (c <= 0xffff) {
            p_codepoint[0] = (char)(0xe0 | ((c >> 12) & 0x0f));
            p_codepoint[1] = (char)(0x80 | ((c >> 6) & 0x3f));
            p_codepoint[2] = (char)(0x80 | (c & 0x3f));
            p_codepoint[3] = '\x00';
          }
          Keyboard::feedText(p_codepoint, 0);
          break;
        }

        this->onKeyPressed(mc, appPlatform.sdl_event.key.keysym.sym,
                           appPlatform.sdl_event.key.keysym.scancode,
                           appPlatform.sdl_event.type == SDL_KEYDOWN);
        break;
      }
    }

    // TODO engine stuff

    /*Tesselator::instance.begin(4);
    Tesselator::instance.enableColor();
    Tesselator::instance.color(0xffff0000);
    Tesselator::instance.vertex(0, 0, 0);
    Tesselator::instance.vertex(0, 1, 0);
    Tesselator::instance.vertex(1, 1, 0);
    Tesselator::instance.vertex(1, 0, 0);
    Tesselator::instance.color(0xff0000ff);
    Tesselator::instance.vertex(0, 1, 0);
    Tesselator::instance.vertex(0, 1, 1);
    Tesselator::instance.vertex(1, 1, 1);
    Tesselator::instance.vertex(1, 1, 0);
    Tesselator::instance.color(0xff00ff00);
    Tesselator::instance.vertex(0, 0, 0);
    Tesselator::instance.vertex(0, 0, 1);
    Tesselator::instance.vertex(0, 1, 1);
    Tesselator::instance.vertex(0, 1, 0);
    Tesselator::instance.color(0xff00ffff);
    Tesselator::instance.vertex(0, 0, 0);
    Tesselator::instance.vertex(1, 0, 0);
    Tesselator::instance.vertex(1, 0, 1);
    Tesselator::instance.vertex(0, 0, 1);
    Tesselator::instance.color(0xffffff00);
    Tesselator::instance.vertex(1, 0, 0);
    Tesselator::instance.vertex(1, 1, 0);
    Tesselator::instance.vertex(1, 1, 1);
    Tesselator::instance.vertex(1, 0, 1);
    Tesselator::instance.color(0xffff00ff);
    Tesselator::instance.vertex(0, 0, 1);
    Tesselator::instance.vertex(1, 0, 1);
    Tesselator::instance.vertex(1, 1, 1);
    Tesselator::instance.vertex(0, 1, 1);
    cube = Tesselator::instance.end();*/
    // TODO contextWasLost
    // else{
    // printf("BUFSIZES %d+%d=%d\n", glBufferPool.unusedBuffers.size(),
    // glBufferPool.usedBuffers.size(),
    // glBufferPool.unusedBuffers.size()+glBufferPool.usedBuffers.size());
    mc->update();
    //}
    /*glViewport(0, 0, mc->field_1C, mc->field_20);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, mc->field_1C*Gui::InvGuiScale, mc->field_20*Gui::InvGuiScale, 0,
    0, 1); glMatrixMode(GL_MODELVIEW); glLoadIdentity();

    glEnable(GL_TEXTURE_2D);

    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnableClientState(GL_VERTEX_ARRAY);
    Tesselator::instance.enableColor();

    //TODO this thing happens in gamerenderer
    int32_t mx = Mouse::getX();
    int32_t my = Mouse::getY();
    mc->currentScreen->render(mx*Gui::InvGuiScale, my*Gui::InvGuiScale, 0);*/
    SDL_GL_SwapBuffers();
  }

  DiscordRPC::shutdown();
  if (mc)
    delete mc;
}
LoginInformation AppPlatform_sdl::getLoginInformation(void) {
  return LoginInformation();
  LoginInformation ret;
  ret.accessToken = "cliff";
  ret.clientId = "my_beloved";
  ret.profileId = "freehij_when_enchantile";
  ret.profileName = "wsndow1";
  return ret;
}
void AppPlatform_sdl::sdlCtxDestroy() {
  if (!this->hasContext)
    return;
  SDL_Quit();
  this->hasContext = 0;
}

AppPlatform_sdl::AppPlatform_sdl() { this->hasContext = 0; }

AppPlatform_sdl::~AppPlatform_sdl() { this->sdlCtxDestroy(); }
void AppPlatform_sdl::showKeyboard(std::string *a, int32_t b, bool_t c) {
  AppPlatform::showKeyboard(a, b, c);
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
}
void AppPlatform_sdl::hideKeyboard(void) {
  AppPlatform::hideKeyboard();
  SDL_EnableKeyRepeat(0, 0);
}
#endif
