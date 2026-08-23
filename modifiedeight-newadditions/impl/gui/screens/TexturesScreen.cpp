#include <gui/screens/TexturesScreen.hpp>
#include <gui/screens/Touch_StartMenuScreen.hpp>
#include <NinecraftApp.hpp>
#include <rendering/Font.hpp>
#include <rendering/Tesselator.hpp>
#include <rendering/Textures.hpp>
#include <AppPlatform.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <sys/stat.h>
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#include <winsock2.h>
#include <windows.h>
#include <direct.h>
#define platform_mkdir(p) _mkdir(p)
#else
#include <dirent.h>
#include <unistd.h>
#define platform_mkdir(p) mkdir(p, 0777)
#endif
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cstdlib>

TexturesScreen::TexturesScreen(Screen *previous)
    : lastScreen(previous),
      btnBack(nullptr),
      btnImport(nullptr),
      scrollOffset(0),
      prevMouseY(0),
      isDragging(false),
      installRunning(false),
      installDone(false),
      needsRefresh(false),
      shouldClose(false) {
  this->alive = std::make_shared<bool>(true);
}

TexturesScreen::~TexturesScreen() {
  if (this->alive) {
    *this->alive = false;
  }
}

void TexturesScreen::init() {
  this->buttons.clear();
  this->managedActionButtons.clear();
  this->managedDeleteButtons.clear();

  this->btnBack = new Touch::TButton(1, "Back", this->minecraft);
  this->btnImport = new Touch::TButton(2, "Import .zip", this->minecraft);

  this->buttons.emplace_back(this->btnBack);
  this->buttons.emplace_back(this->btnImport);

  this->refreshSavedTextures();
  this->setupPositions();
}

void TexturesScreen::setupPositions() {
  int topY = 8;
  int btnW = 100;
  int btnH = 24;

  if (this->btnBack) {
    this->btnBack->width = 50;
    this->btnBack->height = btnH;
    this->btnBack->posX = 10;
    this->btnBack->posY = topY;
  }

  if (this->btnImport) {
    this->btnImport->width = btnW;
    this->btnImport->height = btnH;
    this->btnImport->posX = this->width - btnW - 10;
    this->btnImport->posY = topY;
  }
}

std::string TexturesScreen::pickZipFile() {
  char buffer[1024];
  std::string result = "";
  FILE *f = popen("zenity --file-selection --title=\"Select Texture Pack\" --file-filter=\"Texture Pack (*.zip) | *.zip\" 2>/dev/null", "r");
  if (!f) {
    f = popen("kdialog --getopenfilename . \"*.zip\" 2>/dev/null", "r");
  }
  if (f) {
    if (fgets(buffer, sizeof(buffer), f) != nullptr) {
      result = buffer;
      while (!result.empty() && (result.back() == '\n' || result.back() == '\r')) {
        result.pop_back();
      }
    }
    pclose(f);
  }
  return result;
}

void TexturesScreen::refreshSavedTextures() {
  this->savedTextures.clear();
  this->activeTexturePack = "";

  for (auto *b : this->managedActionButtons) {
    auto it = std::find(this->buttons.begin(), this->buttons.end(), b);
    if (it != this->buttons.end()) this->buttons.erase(it);
    delete b;
  }
  this->managedActionButtons.clear();

  for (auto *b : this->managedDeleteButtons) {
    auto it = std::find(this->buttons.begin(), this->buttons.end(), b);
    if (it != this->buttons.end()) this->buttons.erase(it);
    delete b;
  }
  this->managedDeleteButtons.clear();

  std::string activeFile = "texture_packs/active.txt";
  std::ifstream af(activeFile);
  if (af.is_open()) {
    std::getline(af, this->activeTexturePack);
    af.close();
  }

  platform_mkdir("texture_packs");
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
  WIN32_FIND_DATAA fd;
  HANDLE hFind = FindFirstFileA("texture_packs\\*.zip", &fd);
  if (hFind != INVALID_HANDLE_VALUE) {
    do {
      std::string name = fd.cFileName;
      std::string fullPath = "texture_packs/" + name;
      std::string title = name.substr(0, name.length() - 4);
      this->savedTextures.push_back({fullPath, title});
    } while (FindNextFileA(hFind, &fd));
    FindClose(hFind);
  }
#else
  DIR *dir = opendir("texture_packs");
  if (dir) {
    struct dirent *ent;
    while ((ent = readdir(dir)) != nullptr) {
      std::string name = ent->d_name;
      if (name.length() > 4 && name.substr(name.length() - 4) == ".zip") {
        std::string fullPath = "texture_packs/" + name;
        std::string title = name.substr(0, name.length() - 4);
        this->savedTextures.push_back({fullPath, title});
      }
    }
    closedir(dir);
  }
#endif


  std::sort(this->savedTextures.begin(), this->savedTextures.end(),
            [](const SavedTexture &a, const SavedTexture &b) { return a.title < b.title; });

  int btnId = 1000;
  for (size_t i = 0; i < this->savedTextures.size(); ++i) {
    bool isActive = (this->activeTexturePack == this->savedTextures[i].path);
    Touch::TButton *btnAction = new Touch::TButton(btnId++, isActive ? "Deactivate" : "Activate", this->minecraft);
    Touch::TButton *btnDel = new Touch::TButton(btnId++, "Del", this->minecraft);
    btnAction->width = 75;
    btnAction->height = 20;
    btnDel->width = 30;
    btnDel->height = 20;
    this->managedActionButtons.push_back(btnAction);
    this->managedDeleteButtons.push_back(btnDel);
    this->buttons.emplace_back(btnAction);
    this->buttons.emplace_back(btnDel);
  }
}

void TexturesScreen::importTexturePack() {
  if (this->installRunning) return;
  std::string picked = TexturesScreen::pickZipFile();
  if (picked.empty()) return;

  size_t lastSlash = picked.find_last_of("/\\");
  std::string fname = (lastSlash == std::string::npos) ? picked : picked.substr(lastSlash + 1);

  platform_mkdir("texture_packs");
  std::string dest = "texture_packs/" + fname;
  char cmd[2048];
  snprintf(cmd, sizeof(cmd), "cp \"%s\" \"%s\"", picked.c_str(), dest.c_str());
  (void)system(cmd);

  this->statusMsg = "Imported " + fname;
  this->needsRefresh = true;
}

void TexturesScreen::applySavedTexture(const std::string &path, const std::string &title) {
  if (this->installRunning) return;
  this->installRunning = true;
  this->installDone = false;
  this->installMsg = "Activating " + title + "...";
  this->statusMsg = "";

  auto isAlive = this->alive;
  std::thread([this, isAlive, path, title]() {
    platform_mkdir("texture_packs");
    char cmd[4096];
    snprintf(cmd, sizeof(cmd),
             "rm -rf /tmp/mcpe_tex_tmp && mkdir -p /tmp/mcpe_tex_tmp && "
             "unzip -o -q \"%s\" -d /tmp/mcpe_tex_tmp && "
             "if [ -d /tmp/mcpe_tex_tmp/assets ]; then "
             "  cp -rf /tmp/mcpe_tex_tmp/assets/* assets/ ; "
             "else "
             "  cp -rf /tmp/mcpe_tex_tmp/* assets/ 2>/dev/null || true ; "
             "fi && "
             "rm -rf /tmp/mcpe_tex_tmp",
             path.c_str());
    int res = system(cmd);

    std::ofstream af("texture_packs/active.txt");
    if (af.is_open()) {
      af << path;
      af.close();
    }

    if (*isAlive) {
      this->installRunning = false;
      this->installDone = true;
      if (res == 0) {
        this->statusMsg = "Activated " + title + "!";
      } else {
        this->statusMsg = "Failed to extract texture pack.";
      }
      this->needsRefresh = true;
    }
  }).detach();
}

void TexturesScreen::deleteSavedTexture(const std::string &path) {
  char cmd[2048];
  snprintf(cmd, sizeof(cmd), "rm -f \"%s\"", path.c_str());
  (void)system(cmd);
  if (this->activeTexturePack == path) {
    deactivateTextures();
  }
}

void TexturesScreen::deactivateTextures() {
  if (this->installRunning) return;
  this->installRunning = true;
  this->installDone = false;
  this->installMsg = "Deactivating texture pack...";
  this->statusMsg = "";

  auto isAlive = this->alive;
  std::thread([this, isAlive]() {
    (void)system("rm -rf assets && cp -rf platforms/android/app/src/newadditions/assets . && rm -f texture_packs/active.txt");

    if (*isAlive) {
      this->installRunning = false;
      this->installDone = true;
      this->statusMsg = "Default textures restored!";
      this->needsRefresh = true;
    }
  }).detach();
}

void TexturesScreen::render(int32_t mx, int32_t my, float pt) {
  this->renderDirtBackground(0);

  Font *font = this->minecraft->font;
  std::string title = "Texture Packs";
  this->drawCenteredString(font, title, this->width / 2, 14, 0xFFFFFFFF);

  int listYStart = 40;
  int listYEnd = this->height - 15;
  int itemHeight = 32;

  this->fill(10, listYStart - 2, this->width - 10, listYEnd + 2, 0x88000000);

  int curY = listYStart + this->scrollOffset;

  for (size_t i = 0; i < this->savedTextures.size(); ++i) {
    const auto &tex = this->savedTextures[i];
    Touch::TButton *btnAction = (i < this->managedActionButtons.size()) ? this->managedActionButtons[i] : nullptr;
    Touch::TButton *btnDel = (i < this->managedDeleteButtons.size()) ? this->managedDeleteButtons[i] : nullptr;

    bool isVisible = (curY + itemHeight >= listYStart && curY <= listYEnd);

    if (isVisible) {
      bool isActive = (this->activeTexturePack == tex.path);
      uint32_t bgCol = isActive ? 0x4422AA22 : ((i % 2 == 0) ? 0x22FFFFFF : 0x11FFFFFF);
      this->fill(12, curY, this->width - 12, curY + itemHeight - 2, bgCol);

      uint32_t textCol = isActive ? 0xFF55FF55 : 0xFFFFFFFF;
      this->drawString(font, tex.title + (isActive ? " (Active)" : ""), 18, curY + 8, textCol);

      if (btnAction) {
        btnAction->posX = this->width - 120;
        btnAction->posY = curY + 4;
        btnAction->visible = true;
      }
      if (btnDel) {
        btnDel->posX = this->width - 40;
        btnDel->posY = curY + 4;
        btnDel->visible = true;
      }
    } else {
      if (btnAction) btnAction->visible = false;
      if (btnDel) btnDel->visible = false;
    }

    curY += itemHeight;
  }

  if (this->savedTextures.empty()) {
    this->drawCenteredString(font, "No texture packs found. Click 'Import .zip' to add.", this->width / 2, listYStart + 30, 0xFFAAAAAA);
  }

  if (this->installRunning) {
    this->fill(this->width / 2 - 120, this->height / 2 - 30, this->width / 2 + 120, this->height / 2 + 30, 0xDD000000);
    this->drawCenteredString(font, this->installMsg, this->width / 2, this->height / 2 - 6, 0xFFFFFF55);
  } else if (!this->statusMsg.empty()) {
    this->drawCenteredString(font, this->statusMsg, this->width / 2, this->height - 20, 0xFF55FF55);
  }

  Screen::render(mx, my, pt);
}

bool_t TexturesScreen::handleBackEvent(bool_t a2) {
  this->minecraft->setScreen(this->lastScreen ? this->lastScreen : new Touch::StartMenuScreen());
  return 1;
}

void TexturesScreen::tick() {
  if (!this->pendingDeletePath.empty()) {
    std::string p = this->pendingDeletePath;
    this->pendingDeletePath = "";
    this->deleteSavedTexture(p);
    this->needsRefresh = true;
  }
  if (this->needsRefresh) {
    this->needsRefresh = false;
    this->refreshSavedTextures();
    if (this->minecraft && this->minecraft->texturesPtr) {
      this->minecraft->texturesPtr->reloadAll();
    }
  }
  if (this->shouldClose) {
    this->shouldClose = false;
    this->minecraft->setScreen(this->lastScreen ? this->lastScreen : new Touch::StartMenuScreen());
    return;
  }
  Screen::tick();
}

bool_t TexturesScreen::isInGameScreen() { return 0; }

void TexturesScreen::buttonClicked(Button *btn) {
  if (!btn) return;
  if (btn->buttonID == 1) { // Back
    this->handleBackEvent(false);
    return;
  }
  if (btn->buttonID == 2) { // Import .zip
    this->importTexturePack();
    return;
  }

  for (size_t i = 0; i < this->managedActionButtons.size(); ++i) {
    if (btn->buttonID == this->managedActionButtons[i]->buttonID) {
      if (i < this->savedTextures.size()) {
        if (this->activeTexturePack == this->savedTextures[i].path) {
          this->deactivateTextures();
        } else {
          this->applySavedTexture(this->savedTextures[i].path, this->savedTextures[i].title);
        }
      }
      return;
    }
  }

  for (size_t i = 0; i < this->managedDeleteButtons.size(); ++i) {
    if (btn->buttonID == this->managedDeleteButtons[i]->buttonID) {
      if (i < this->savedTextures.size()) {
        this->pendingDeletePath = this->savedTextures[i].path;
      }
      return;
    }
  }
}

void TexturesScreen::mouseClicked(int32_t mx, int32_t my, int32_t btn) {
  if (btn == 1) {
    this->isDragging = true;
    this->prevMouseY = my;
  }
  Screen::mouseClicked(mx, my, btn);
}

void TexturesScreen::mouseReleased(int32_t mx, int32_t my, int32_t btn) {
  if (btn == 1) {
    this->isDragging = false;
  }
  Screen::mouseReleased(mx, my, btn);
}
