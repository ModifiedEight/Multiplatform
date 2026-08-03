#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
#else
  #include <arpa/inet.h>
  #include <netdb.h>
#endif

#include <ExternalServer.hpp>
#include <ExternalServerFile.hpp>
#include <Minecraft.hpp>
#include <gui/Gui.hpp>
#include <gui/buttons/ImageWithBackground.hpp>
#include <gui/buttons/Touch_TButton.hpp>
#include <gui/elements/LocalServerListItemElement.hpp>
#include <gui/screens/DisconnectionScreen.hpp>
#include <gui/screens/EditExternalServerScreen.hpp>
#include <gui/screens/PlayScreen.hpp>
#include <gui/screens/ProgressScreen.hpp>
#include <gui/screens/RenameMPLevelScreen.hpp>
#include <gui/screens/Touch_DeleteWorldScreen.hpp>
#include <input/Mouse.hpp>
#include <level/LevelSettings.hpp>
#include <level/LevelSummary.hpp>
#include <math.h>
#include <network/PingedCompatibleServer.hpp>
#include <network/RakNetInstance.hpp>
#include <rendering/Font.hpp>
#include <rendering/Textures.hpp>
#include <sstream>
#include <util/IntRectangle.hpp>
#include <util/ParameterStringify.hpp>
#include <util/Util.hpp>
#include <utils.h>

static std::string resolveHost(const std::string &host) {
  if (host.empty())
    return "";
  struct hostent *he = gethostbyname(host.c_str());
  if (he && he->h_addr_list && he->h_addr_list[0]) {
    return inet_ntoa(*(struct in_addr *)he->h_addr_list[0]);
  }
  return host;
}

static std::string extractPlayerCount(const std::string &str) {
  size_t start = str.find('[');
  if (start != std::string::npos) {
    size_t end = str.find(']', start);
    if (end != std::string::npos && end > start + 1) {
      std::string sub = str.substr(start + 1, end - start - 1);
      if (sub.find('/') != std::string::npos) {
        return sub;
      }
    }
  }
  return "0/20";
}

LocalServerListItemElement::LocalServerListItemElement(Minecraft *a2,
                                                       ExternalServer a3,
                                                       bool_t editing,
                                                       PlayScreen *a5)
    : GuiElement(1, 1, 0, 0, 24, 24) {
  this->field_24 = this->field_28 = 0; // XXX doesnt seem to be in mcpe
  this->field_2C = 0;
  this->field_30 = 0;
  this->deleteElementButton = 0;
  this->editElementButton = 0;
  this->field_3C = 0;
  this->levelSummary = 0;
  this->isEditing = editing;
  this->field_50 = 0;
  this->field_54 = a5;
  this->height = 32;
  this->server = new ExternalServer(a3);
  if (editing) {
    this->deleteElementButton = new ImageWithBackground(-1);
    this->deleteElementButton->init(a2->texturesPtr, 32, 32, {112, 0, 8, 67},
                                    {120, 0, 8, 67}, 2, 2,
                                    "gui/spritesheet.png");
    ImageDef v16;
    v16.field_0 = "gui/gui.png";
    v16.field_24 = 1;
    v16.width = 11.0;
    v16.height = 11.0;
    v16.field_14 = 182;
    v16.field_18 = 10;
    v16.field_1C = 11;
    v16.field_20 = 11;
    this->deleteElementButton->setImageDef(v16, 0);
    this->deleteElementButton->width = 32;
    this->deleteElementButton->height = 32;
    this->deleteElementButton->setupPositions();

    this->editElementButton = new ImageWithBackground(-1);
    this->editElementButton->init(a2->texturesPtr, 32, 32, {112, 0, 8, 67},
                                  {120, 0, 8, 67}, 2, 2, "gui/spritesheet.png");
    ImageDef v21;
    v21.field_0 = "gui/touchgui.png";
    v21.field_24 = 1;
    v21.width = 22.0;
    v21.field_18 = 0;
    v21.height = 21.0;
    v21.field_14 = 218;
    v21.field_1C = 22;
    v21.field_20 = 21;
    this->editElementButton->setImageDef(v21, 0);
    this->editElementButton->width = 32;
    this->editElementButton->height = 32;
    this->editElementButton->setupPositions();
  }
}
LocalServerListItemElement::LocalServerListItemElement(Minecraft *a2,
                                                       const LevelSummary &a3,
                                                       bool_t a4)
    : GuiElement(1, 1, 0, 0, 24, 24) {
  this->field_24 = this->field_28 = 0; // XXX doesnt seem to be in mcpe
  this->field_2C = 0;
  this->field_30 = 0;
  this->deleteElementButton = 0;
  this->editElementButton = 0;
  this->server = 0;
  this->isEditing = a4;
  this->field_50 = 0;
  this->field_54 = 0;
  this->height = 32;
  this->field_3C = 0;
  this->levelSummary = new LevelSummary(a3);
  if (a4) {
    this->deleteElementButton = new ImageWithBackground(-1);
    this->deleteElementButton->init(a2->texturesPtr, 32, 32, {112, 0, 8, 67},
                                    {120, 0, 8, 67}, 2, 2,
                                    "gui/spritesheet.png");
    ImageDef v15;
    v15.field_0 = "gui/gui.png";
    v15.field_24 = 1;
    v15.width = 11.0;
    v15.height = 11.0;
    v15.field_14 = 182;
    v15.field_18 = 10;
    v15.field_1C = 11;
    v15.field_20 = 11;
    this->deleteElementButton->setImageDef(v15, 0);
    this->deleteElementButton->width = 32;
    this->deleteElementButton->height = 32;
    this->deleteElementButton->setupPositions();

    this->editElementButton = new ImageWithBackground(-1);
    this->editElementButton->init(a2->texturesPtr, 32, 32, {112, 0, 8, 67},
                                  {120, 0, 8, 67}, 2, 2, "gui/spritesheet.png");
    ImageDef v21;
    v21.field_0 = "gui/touchgui.png";
    v21.field_24 = 1;
    v21.width = 22.0;
    v21.field_18 = 0;
    v21.height = 21.0;
    v21.field_14 = 218;
    v21.field_1C = 22;
    v21.field_20 = 21;
    this->editElementButton->setImageDef(v21, 0);
    this->editElementButton->width = 32;
    this->editElementButton->height = 32;
    this->editElementButton->setupPositions();

    std::stringstream v16;
    v16 << this->levelSummary->field_10;
    this->field_44 = v16.str();
    this->field_48 = a2->font->width(this->field_44);
  }
}
LocalServerListItemElement::LocalServerListItemElement(
    const PingedCompatibleServer &a2)
    : GuiElement(1, 1, 0, 0, 24, 24) {
  this->field_24 = this->field_28 = 0; // XXX doesnt seem to be in mcpe
  this->field_2C = 0;
  this->field_30 = 0;
  this->deleteElementButton = 0;
  this->editElementButton = 0;
  this->server = 0;
  this->field_50 = 0;
  this->field_54 = 0;
  this->height = 32;
  this->field_3C = new PingedCompatibleServer(a2);
  this->levelSummary = 0;
}
std::string LocalServerListItemElement::getLastPlayedString() {
  int32_t v3;   // r0
  char_t s[32]; // [sp+4h] [bp-34h] BYREF

  if (!this->levelSummary) {
    return "";
  }
  v3 = getEpochTimeS() - this->levelSummary->field_8;
  if (v3 > 86399) {
    if (v3 > 172799) {
      if (v3 > 604799) {
        if (v3 > 2419199) {
          return "long ago";
        }
        sprintf(s, "%d weeks ago", v3 / 604800);
      } else {
        sprintf(s, "%d days ago", v3 / 86400);
      }
      return s;
    } else {
      return "Yesterday";
    }
  } else {
    return "Today";
  }
}
void LocalServerListItemElement::init(Minecraft *a2) {
  Touch::TButton *v4 = new Touch::TButton(1, "", 0);
  v4->init(a2, "gui/spritesheet.png", {8, 32, 8, 8}, {0, 32, 8, 8}, 2, 2, 120,
           32);
  this->field_2C = v4;
}

LocalServerListItemElement::~LocalServerListItemElement() {
  if (this->server) {
    delete this->server;
    this->server = 0;
  }

  if (this->field_3C) {
    delete this->field_3C;
    this->field_3C = 0;
  }
  if (this->levelSummary) {
    delete this->levelSummary;
    this->levelSummary = 0;
  }

  if (this->deleteElementButton) {
    delete this->deleteElementButton;
    this->deleteElementButton = 0;
  }

  if (this->editElementButton) {
    delete this->editElementButton;
    this->editElementButton = 0;
  }

  if (this->field_2C) {
    delete this->field_2C;
    this->field_2C = 0;
  }
}
void LocalServerListItemElement::tick(Minecraft *a2) {
  float x; // s16
  float y; // s15

  x = (float)Mouse::getX() * Gui::InvGuiScale;
  y = (float)Mouse::getY() * Gui::InvGuiScale;
  if (fabsf(x - this->field_24) > 20.0 || fabsf(y - this->field_28) > 20.0) {
    Button *pressed = this->field_30;
    if (pressed) {
      pressed->released((int32_t)x, (int32_t)y);
    }
    this->field_30 = 0;
  }
  ++this->field_50;
}
void LocalServerListItemElement::render(Minecraft *a2, int32_t a3, int32_t a4) {
  int32_t width;               // r8
  bool_t isPressed;            // r0
  LevelSummary *levelSummary;  // r9
  int32_t v12;                 // r0
  int32_t v14;                 // r11
  Font *font;                  // r6
  int32_t v17;                 // r11
  PingedCompatibleServer *v20; // r3
  ExternalServer *server;      // r3
  ExternalServer *v23;         // r3
  char_t v29[128];             // [sp+2Ch] [bp-B4h] BYREF

  if (this->isEditing && this->deleteElementButton && this->editElementButton) {
    width = this->width - this->deleteElementButton->width -
            this->editElementButton->width;
    this->deleteElementButton->posX =
        this->posX + this->width - this->deleteElementButton->width;
    this->deleteElementButton->posY = this->posY;
    this->editElementButton->posX = this->posX + this->width -
                                    this->deleteElementButton->width -
                                    this->editElementButton->width;
    this->editElementButton->posY = this->posY;
  } else {
    width = this->width;
  }
  this->field_2C->posX = this->posX;
  this->field_2C->posY = this->posY;
  this->field_2C->width = width;
  this->field_2C->render(a2, a3, a4);
  isPressed = this->field_2C->isPressed(a3, a4);
  levelSummary = this->levelSummary;
  if (isPressed) {
    v12 = 0xFFFFA0;
  } else {
    v12 = 0xFFFFFFFF;
  }
  if (levelSummary) {
    a2->font->drawShadow(levelSummary->field_4, (float)this->posX + 5.0,
                         (float)this->posY + 5.0, v12);
    std::string ss = this->levelSummary->field_C == 1 ? "Creative" : "Survival";
    v14 = a2->font->width(ss);
    a2->font->drawShadow(ss, (float)this->posX + 5.0, (float)this->posY + 16.0,
                         0xFFBBBBBB);
    a2->font->drawShadow(
        this->getLastPlayedString(),
        (float)((float)((float)this->posX + 5.0) + (float)v14) + 10.0,
        (float)this->posY + 16.0, 0xFFBBBBBB);
    if (this->isEditing) {
      if (this->deleteElementButton)
        this->deleteElementButton->render(a2, a3, a4);
      if (this->editElementButton)
        this->editElementButton->render(a2, a3, a4);
      font = a2->font;
      v17 = width + this->posX;
      font->drawShadow("Seed:", (float)(v17 - font->width("Seed:")) - 5.0,
                       (float)this->posY + 5.0, 0xFFBBBBBB);
      a2->font->drawShadow(this->field_44,
                           (float)(width + this->posX - this->field_48) - 5.0,
                           (float)this->posY + 16.0, 0xFFBBBBBB);
    }
  } else {
    v20 = this->field_3C;
    if (v20) {
      a2->font->drawShadow(v20->field_0, (float)this->posX + 5.0,
                           (float)this->posY + 5.0, -4473857);
      this->field_3C->field_4.ToString(0, v29, '|');
      a2->font->drawShadow(std::string("World on wifi: ").append(v29),
                           (float)this->posX + 5.0, (float)this->posY + 16.0,
                           0xFFBBBBBB);
      a2->texturesPtr->loadAndBindTexture("gui/spritesheet.png");
      glColor4f(1.0, 1.0, 1.0, 1.0);
      this->blit((float)this->width - 14.0f, (float)this->posY + 9.0f, 192,
                 -24 * (this->field_50 / 4 % 3) + 48, 12.0, 12.0, 24, 24);
      std::string statusStr = "Online";
      std::string playersStr = "0/20";
      int32_t rightX = this->posX + width - 20;
      a2->font->drawShadow(statusStr,
                           (float)(rightX - a2->font->width(statusStr)),
                           (float)this->posY + 5.0, 0xFF55FF55);
      a2->font->drawShadow(playersStr,
                           (float)(rightX - a2->font->width(playersStr)),
                           (float)this->posY + 16.0, 0xFFBBBBBB);
    } else {
      server = this->server;
      if (!server) {
        return;
      }
      a2->font->drawShadow(server->field_4, (float)this->posX + 5.0,
                           (float)this->posY + 5.0, v12);
      v23 = this->server;
      if (v23->field_8.size() <= 0x80u) {
        sprintf(v29, "%s:%d", v23->field_8.c_str(), v23->field_C);
      } else {
        sprintf(v29, "...:%d", v23->field_C);
      }
      a2->font->drawShadow(v29, (float)this->posX + 5.0,
                           (float)this->posY + 16.0, -4473925);

      bool isOnline = false;
      std::string playersStr = "0/20";
      if (a2->rakNetInstance && a2->rakNetInstance->getServerList()) {
        auto *sList = a2->rakNetInstance->getServerList();
        for (auto &s : *sList) {
          char addrBuf[128];
          s.field_4.ToString(0, addrBuf, 0);
          int sPort = s.field_4.GetPort();
          if ((server->field_8 == addrBuf ||
               resolveHost(server->field_8) == addrBuf ||
               server->field_8 == "localhost" ||
               server->field_8 == "127.0.0.1") &&
              (server->field_C == sPort || server->field_C == 0)) {
            isOnline = true;
            std::string pStr = s.field_0.C_String();
            playersStr = extractPlayerCount(pStr);
            break;
          }
        }
      }
      std::string loadingDots = "";
      int dotCount = (this->field_50 / 8) % 4;
      for (int d = 0; d < dotCount; ++d)
        loadingDots += ".";

      std::string statusStr = isOnline ? "Online" : ("Loading" + loadingDots);
      if (!isOnline)
        playersStr = "";
      int32_t statusColor = isOnline ? 0xFF55FF55 : 0xFFFFFF55;
      int32_t playersColor = isOnline ? 0xFFBBBBBB : 0xFF777777;

      if (this->isEditing) {
        if (this->deleteElementButton)
          this->deleteElementButton->render(a2, a3, a4);
        if (this->editElementButton)
          this->editElementButton->render(a2, a3, a4);
      } else {
        int32_t rightX = this->posX + width - 5;
        a2->font->drawShadow(statusStr,
                             (float)(rightX - a2->font->width(statusStr)),
                             (float)this->posY + 5.0, statusColor);
        a2->font->drawShadow(playersStr,
                             (float)(rightX - a2->font->width(playersStr)),
                             (float)this->posY + 16.0, playersColor);
      }
    }
  }
}
void LocalServerListItemElement::mouseClicked(Minecraft *a2, int32_t a3,
                                              int32_t a4, int32_t a5) {
  float v9;                                 // s17
  int32_t v10;                              // s15
  ImageWithBackground *deleteElementButton; // r0
  float v12;                                // s16
  Button *v13;                              // r0

  v9 = (float)Mouse::getX() * Gui::InvGuiScale;
  v10 = Mouse::getY();
  deleteElementButton = this->deleteElementButton;
  v12 = (float)v10 * Gui::InvGuiScale;
  if (deleteElementButton && deleteElementButton->clicked(a2, a3, a4)) {
    v13 = this->deleteElementButton;
    this->field_24 = v9;
    this->field_28 = v12;
  } else if (this->editElementButton &&
             this->editElementButton->clicked(a2, a3, a4)) {
    v13 = this->editElementButton;
    this->field_24 = v9;
    this->field_28 = v12;
  } else {
    if (!this->field_2C->clicked(a2, a3, a4)) {
      return;
    }
    this->field_24 = v9;
    this->field_28 = v12;
    v13 = this->field_2C;
  }
  this->field_30 = v13;
  v13->setPressed();
}
void LocalServerListItemElement::mouseReleased(Minecraft *a2, int32_t a3,
                                               int32_t a4, int32_t a5) {
  if (this->deleteElementButton &&
      this->deleteElementButton == this->field_30) {
    if (this->deleteElementButton->clicked(a2, a3, a4)) {
      if (this->server) {
        a2->externalServerFile->removeServer(this->server->field_0);
        this->field_54->field_50 = 1;
      } else {
        a2->setScreen(new Touch::DeleteWorldScreen(*this->levelSummary));
      }
      return;
    }
    this->deleteElementButton->released(a3, a4);
    this->field_30 = 0;
    return;
  }
  if (this->editElementButton && this->editElementButton == this->field_30) {
    if (this->editElementButton->clicked(a2, a3, a4)) {
      if (this->server) {
        a2->setScreen(new EditExternalServerScreen(*this->server));
      } else {
        a2->setScreen(new RenameMPLevelScreen(this->levelSummary->field_0,
                                              this->levelSummary->field_4));
      }
      return;
    }
    this->editElementButton->released(a3, a4);
    this->field_30 = 0;
    return;
  }
  if (this->field_30 != this->field_2C) {
    return;
  }

  if (!this->field_30->clicked(a2, a3, a4)) {
    this->field_2C->released(a3, a4);
    this->field_30 = 0;
    return;
  }

  if (this->field_3C || this->server) {
    if (this->server) {
      if (a2->platform()->isNetworkEnabled(1)) {
        PingedCompatibleServer v43;
        v43.field_4.FromStringExplicitPort(this->server->field_8.c_str(),
                                           this->server->field_C, 0);
        v43.field_0 = this->server->field_4.c_str();
        a2->joinMultiplayer(PingedCompatibleServer(v43), 0);
        a2->setScreen(new ProgressScreen());
        std::string v38_;
        { // TODO probably some inlined function - ParameterStringify::something
          // ???
          std::string v33 = "{\"%\": \"%\"}";
          std::vector<std::string> v37;
          {
            std::stringstream v38;
            v38 << "server_type";
            v37.emplace_back(v38.str());
            std::stringstream v40;
            v40 << "External";
            v37.emplace_back(v40.str());
          }
          v38_ = Util::simpleFormat(v33, v37);
        }
        a2->platform()->statsTrackData("start_game", v38_);
        return;
      }
      a2->setScreen(new DisconnectionScreen(
          "You need to be connected through Wifi to play on External servers"));
    } else {
      a2->joinMultiplayer(*this->field_3C, 0);
      a2->setScreen(new ProgressScreen());
      // TODO ParameterStringify::stringifyNext()
      printf("LocalServerListItemElement::mouseReleased - connect to local "
             "server - statsTrackData - not implemented\n");
    }
  } else {
    a2->selectLevel(this->levelSummary->field_0, this->levelSummary->field_4,
                    LevelSettings{-1, -1});
    a2->hostMultiplayer(19132);
    a2->setScreen(new ProgressScreen());

    printf("LocalServerListItemElement::mouseReleased - join world - "
           "statsTrackData - not implemented\n");
  }
}
