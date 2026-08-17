#ifdef _WIN32
  #include <direct.h>
  #define mkdir(path, mode) _mkdir(path)
#endif
#include <Minecraft.hpp>
#include <NinecraftApp.hpp>
#include <algorithm>
#include <cpputils.hpp>
#include <cstring>
#include <dirent.h>
#include <gui/NinePatchFactory.hpp>
#include <gui/buttons/ImageButton.hpp>
#include <gui/elements/Label.hpp>
#include <gui/elements/TextBox.hpp>
#include <gui/screens/TexturesScreen.hpp>
#include <item/Item.hpp>
#include <level/storage/LevelStorageSource.hpp>
#include <rendering/LevelRenderer.hpp>
#include <rendering/Tesselator.hpp>
#include <rendering/Textures.hpp>
#include <tile/Tile.hpp>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <util/CrossPlatformWeb.hpp>
#include <util/FileUtil.hpp>
#include <util/IntRectangle.hpp>
#include <util/Util.hpp>
#include <utils.h>

static std::mutex g_texture_mutex;

static inline void makeDirs(const std::string &path) {
  char tmp[1024];
  snprintf(tmp, sizeof(tmp), "%s", path.c_str());
  size_t len = strlen(tmp);
  if (len == 0)
    return;
  if (tmp[len - 1] == '/')
    tmp[len - 1] = 0;
  for (char *p = tmp + 1; *p; p++) {
    if (*p == '/') {
      *p = 0;
      mkdir(tmp, 0755);
      *p = '/';
    }
  }
  mkdir(tmp, 0755);
}

static inline bool isDirExists(const std::string &path) {
  struct stat st;
  return (stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode));
}

TexturesScreen::TexturesScreen(Screen *previous)
    : Screen(), lastScreen(previous), scrollOffset(0), prevMouseY(0),
      isDragging(false), installRunning(false), installDone(false),
      needsRefresh(false), shouldClose(false), btnBack(nullptr),
      btnImport(nullptr), btnDefault(nullptr) {
  alive = std::make_shared<bool>(true);
}

TexturesScreen::~TexturesScreen() {
  std::lock_guard<std::mutex> lock(g_texture_mutex);
  *alive = false;
}

void TexturesScreen::init() {
  buttons.clear();
  field_2C.clear();

  btnImport = new Touch::TButton(2, "Import Pack (.zip)...", nullptr);
  btnImport->init(this->minecraft);
  btnImport->width = 150;
  btnImport->height = 22;
  buttons.push_back(btnImport);
  field_2C.push_back(btnImport);

  btnDefault = new Touch::TButton(3, "Restore Default", nullptr);
  btnDefault->init(this->minecraft);
  btnDefault->width = 150;
  btnDefault->height = 22;
  buttons.push_back(btnDefault);
  field_2C.push_back(btnDefault);

  btnBack = new Touch::TButton(1, "Done", nullptr);
  btnBack->init(this->minecraft);
  btnBack->width = 150;
  btnBack->height = 24;
  buttons.push_back(btnBack);
  field_2C.push_back(btnBack);

  refreshSavedTextures();
}

void TexturesScreen::setupPositions() {
  if (btnImport) {
    btnImport->posX = width / 2 - 154;
    btnImport->posY = 40;
  }
  if (btnDefault) {
    btnDefault->posX = width / 2 + 4;
    btnDefault->posY = 40;
  }
  if (btnBack) {
    btnBack->posX = width / 2 - 75;
    btnBack->posY = height - 30;
  }
}

void TexturesScreen::renderMenuBg32() {
  if (this->minecraft && this->minecraft->texturesPtr) {
    this->minecraft->texturesPtr->loadAndBindTexture("gui/bg32.png");
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    Tesselator::instance.begin(4);
    Tesselator::instance.color(0xFFFFFFFF);
    Tesselator::instance.vertexUV(0, this->height, 0, 0,
                                  (float)this->height * 0.03125f);
    Tesselator::instance.vertexUV(this->width, this->height, 0,
                                  (float)this->width * 0.03125f,
                                  (float)this->height * 0.03125f);
    Tesselator::instance.vertexUV(this->width, 0, 0,
                                  (float)this->width * 0.03125f, 0);
    Tesselator::instance.vertexUV(0, 0, 0, 0, 0);
    Tesselator::instance.draw(1);
  } else {
    renderBackground(0);
  }
}

std::string TexturesScreen::pickZipFile() {
#if defined(ANDROID) || defined(__ANDROID__) || defined(TARGET_OS_IPHONE) || defined(IPHONE)
  return "";
#else
  char path[1024] = {0};

  FILE *f = popen("zenity --file-selection --title=\"Select Texture Pack "
                  "(.zip)\" --file-filter=\"*.zip\" 2>/dev/null",
                  "r");
  if (f) {
    if (fgets(path, sizeof(path), f)) {
      path[strcspn(path, "\r\n")] = 0;
    }
    pclose(f);
  }
  if (strlen(path) > 0)
    return std::string(path);

  f = popen("kdialog --getopenfilename . \"*.zip\" 2>/dev/null", "r");
  if (f) {
    if (fgets(path, sizeof(path), f)) {
      path[strcspn(path, "\r\n")] = 0;
    }
    pclose(f);
  }
  if (strlen(path) > 0)
    return std::string(path);

  f = popen(
      "powershell -Command \"Add-Type -AssemblyName System.Windows.Forms; $f = "
      "New-Object System.Windows.Forms.OpenFileDialog; $f.Filter = 'Zip files "
      "(*.zip)|*.zip'; if($f.ShowDialog() -eq 'OK'){ Write-Host $f.FileName "
      "}\" 2>/dev/null",
      "r");
  if (f) {
    if (fgets(path, sizeof(path), f)) {
      path[strcspn(path, "\r\n")] = 0;
    }
    pclose(f);
  }
  if (strlen(path) > 0)
    return std::string(path);

  f = popen("osascript -e 'POSIX path of (choose file with prompt \"Select "
            "Texture Pack (.zip)\" of type {\"zip\"})' 2>/dev/null",
            "r");
  if (f) {
    if (fgets(path, sizeof(path), f)) {
      path[strcspn(path, "\r\n")] = 0;
    }
    pclose(f);
  }
  if (strlen(path) > 0)
    return std::string(path);

  return "";
#endif
}

void TexturesScreen::refreshSavedTextures() {
  for (auto btn : managedApplyButtons) {
    auto it = std::find(buttons.begin(), buttons.end(), btn);
    if (it != buttons.end()) buttons.erase(it);
    auto it2 = std::find(field_2C.begin(), field_2C.end(), btn);
    if (it2 != field_2C.end()) field_2C.erase(it2);
    delete btn;
  }
  managedApplyButtons.clear();

  for (auto btn : managedDeleteButtons) {
    auto it = std::find(buttons.begin(), buttons.end(), btn);
    if (it != buttons.end()) buttons.erase(it);
    auto it2 = std::find(field_2C.begin(), field_2C.end(), btn);
    if (it2 != field_2C.end()) field_2C.erase(it2);
    delete btn;
  }
  managedDeleteButtons.clear();

  savedTextures.clear();
  activeTexturePack = "";

  std::string dataPath = this->minecraft ? this->minecraft->dataPathMaybe : "";
  if (dataPath.empty()) {
    const char* home = getenv("HOME");
    if (home) dataPath = std::string(home) + "/.minecraftpe";
    else dataPath = ".";
  }

  std::string path = dataPath + "/saved_textures";
  makeDirs(path);

  std::string activeFile = path + "/_active.txt";
  FILE *af = fopen(activeFile.c_str(), "r");
  if (af) {
    char name[256] = {0};
    if (fgets(name, sizeof(name), af)) {
      name[strcspn(name, "\r\n")] = 0;
      activeTexturePack = name;
    }
    fclose(af);
  }

  std::vector<std::string> packDirs = {
      dataPath + "/resource_packs",
      dataPath + "/games/com.mojang/resource_packs"
  };

  for (const auto &pdir : packDirs) {
    if (!isDirExists(pdir)) continue;
    DIR *pd = opendir(pdir.c_str());
    if (!pd) continue;
    struct dirent *pe;
    while ((pe = readdir(pd))) {
      if (!pe) break;
      std::string zname = pe->d_name;
      if (zname == "." || zname == "..") continue;
      std::string fullItem = pdir + "/" + zname;
      if (isDirExists(fullItem)) {
        std::string targetExtDir = path + "/" + zname;
        if (!isDirExists(targetExtDir)) {
          FileUtil::CopyDirectory(fullItem, targetExtDir);
        }
      } else if (zname.size() > 4 && zname.substr(zname.size() - 4) == ".zip") {
        std::string title = zname.substr(0, zname.size() - 4);
        std::string targetExtDir = path + "/" + title;
        if (!isDirExists(targetExtDir)) {
          std::string tmpExt = dataPath + "/_texture_ext";
          FileUtil::RemoveDirectory(tmpExt);
          makeDirs(tmpExt);
          makeDirs(targetExtDir);
          if (CrossPlatform_ExtractZip(fullItem, tmpExt)) {
            std::string imagesDir = FileUtil::FindDirRecursively(tmpExt, "images");
            if (imagesDir.empty()) {
              imagesDir = FileUtil::FindDirRecursively(tmpExt, "gui");
            }
            std::string srcDir = imagesDir.empty() ? tmpExt : imagesDir.substr(0, imagesDir.find_last_of('/'));
            FileUtil::CopyDirectory(srcDir, targetExtDir);
          }
          FileUtil::RemoveDirectory(tmpExt);
        }
      }
    }
    closedir(pd);
  }

  DIR *d = opendir(path.c_str());
  if (d) {
    struct dirent *ep;
    int idx = 0;
    while ((ep = readdir(d))) {
      if (!ep) break;
      std::string name = ep->d_name;
      if (name == "." || name == ".." || name == "_active.txt" ||
          name[0] == '.')
        continue;

      std::string packFullPath = path + "/" + name;
      if (!isDirExists(packFullPath)) continue;

      SavedTexture st;
      st.path = packFullPath;
      st.title = name;
      savedTextures.push_back(st);

      Touch::TButton *btnApply = new Touch::TButton(
          100 + idx, (name == activeTexturePack) ? "Active" : "Apply", nullptr);
      btnApply->init(this->minecraft);
      btnApply->active = false;
      btnApply->visible = false;
      btnApply->width = 60;
      btnApply->height = 22;
      buttons.push_back(btnApply);
      field_2C.push_back(btnApply);
      managedApplyButtons.push_back(btnApply);

      Touch::TButton *btnDel = new Touch::TButton(200 + idx, "Delete", nullptr);
      btnDel->init(this->minecraft);
      btnDel->active = false;
      btnDel->visible = false;
      btnDel->width = 60;
      btnDel->height = 22;
      buttons.push_back(btnDel);
      field_2C.push_back(btnDel);
      managedDeleteButtons.push_back(btnDel);

      idx++;
    }
    closedir(d);
  }
}

void TexturesScreen::importTexturePack() {
  installRunning = true;
  installDone = false;
  installMsg = "";
  std::string dataPath = this->minecraft ? this->minecraft->dataPathMaybe : "";
  if (dataPath.empty()) {
    const char* home = getenv("HOME");
    if (home) dataPath = std::string(home) + "/.minecraftpe";
    else dataPath = ".";
  }
  auto aliveFlag = alive;

  std::thread([this, dataPath, aliveFlag]() {
    std::string zipPath = pickZipFile();
    if (zipPath.empty()) {
      std::vector<std::string> searchDirs;
      searchDirs.push_back(dataPath + "/resource_packs");
      searchDirs.push_back(dataPath + "/texture_packs");
      searchDirs.push_back(dataPath + "/games/com.mojang/resource_packs");
      searchDirs.push_back("/sdcard/Download");
      searchDirs.push_back("/sdcard/Download/Minecraft");
      searchDirs.push_back("/storage/emulated/0/Download");
      searchDirs.push_back("/storage/emulated/0/Download/Minecraft");
      searchDirs.push_back("/storage/emulated/0/games/com.mojang/resource_packs");
      searchDirs.push_back("/storage/emulated/0/Android/data/com.mojang.minecraftpe/files/resource_packs");
      const char* home = getenv("HOME");
      if (home) {
        searchDirs.push_back(std::string(home) + "/Downloads");
        searchDirs.push_back(std::string(home) + "/Documents");
      }
      searchDirs.push_back(".");

      for (const auto& dir : searchDirs) {
        if (!isDirExists(dir)) continue;
        DIR *pd = opendir(dir.c_str());
        if (pd) {
          struct dirent *pe;
          while ((pe = readdir(pd))) {
            if (!pe) break;
            std::string zname = pe->d_name;
            if (zname.size() > 4 && zname.substr(zname.size() - 4) == ".zip") {
              zipPath = dir + "/" + zname;
              break;
            }
          }
          closedir(pd);
        }
        if (!zipPath.empty()) break;
      }
    }

    if (zipPath.empty()) {
      std::lock_guard<std::mutex> lock(g_texture_mutex);
      if (!*aliveFlag)
        return;
      installMsg = "Put .zip in Download or resource_packs folder!";
      installRunning = false;
      installDone = true;
      return;
    }

    std::string filename = zipPath;
    size_t lastSlash = filename.find_last_of("/\\");
    if (lastSlash != std::string::npos)
      filename = filename.substr(lastSlash + 1);
    size_t dotPos = filename.find_last_of('.');
    if (dotPos != std::string::npos)
      filename = filename.substr(0, dotPos);

    std::string titleStr = filename;
    std::string outPath = dataPath + "/saved_textures/";
    std::string sanitized = titleStr;
    for (char &c : sanitized) {
      if (c == '/' || c == '\\' || c == ':')
        c = '_';
    }
    outPath += sanitized;

    std::string texExt = dataPath + "/_texture_ext";
    FileUtil::RemoveDirectory(texExt);
    makeDirs(texExt);
    makeDirs(outPath);

    bool extractOk = CrossPlatform_ExtractZip(zipPath, texExt);
    int ret = 1;
    if (extractOk) {
      std::string imagesDir = FileUtil::FindDirRecursively(texExt, "images");
      if (imagesDir.empty()) {
        imagesDir = FileUtil::FindDirRecursively(texExt, "gui");
      }
      std::string srcDir =
          imagesDir.empty() ? texExt
                            : imagesDir.substr(0, imagesDir.find_last_of('/'));
      FileUtil::CopyDirectory(srcDir, outPath);
      ret = 0;
    }

    FileUtil::RemoveDirectory(texExt);

    std::lock_guard<std::mutex> lock(g_texture_mutex);
    if (!*aliveFlag)
      return;
    installMsg =
        (ret == 0) ? "Imported '" + titleStr + "'!" : "Extraction failed";
    installRunning = false;
    installDone = true;
  }).detach();
}

void TexturesScreen::applySavedTexture(const std::string &path,
                                       const std::string &title) {
  std::string dataPath = this->minecraft ? this->minecraft->dataPathMaybe : "";
  if (dataPath.empty()) {
    const char* home = getenv("HOME");
    if (home) dataPath = std::string(home) + "/.minecraftpe";
    else dataPath = ".";
  }
  std::string savedDir = dataPath + "/saved_textures";
  makeDirs(savedDir);

  std::string activeFile = savedDir + "/_active.txt";
  FILE *af = fopen(activeFile.c_str(), "w");
  if (af) {
    fputs(title.c_str(), af);
    fclose(af);
  }

  activeTexturePack = title;

  extern uint8_t* g_terrainAtlasPixels;
  extern uint8_t* g_itemsAtlasPixels;
  if (g_terrainAtlasPixels) {
    free(g_terrainAtlasPixels);
    g_terrainAtlasPixels = nullptr;
  }
  if (g_itemsAtlasPixels) {
    free(g_itemsAtlasPixels);
    g_itemsAtlasPixels = nullptr;
  }

  if (this->minecraft) {
    if (this->minecraft->texturesPtr) {
      this->minecraft->texturesPtr->reloadAll();
    }
    if (NinecraftApp::_terrainTextureAtlas) {
      NinecraftApp::_terrainTextureAtlas->load((NinecraftApp *)this->minecraft);
      Tile::initTiles(NinecraftApp::_terrainTextureAtlas);
    }
    if (NinecraftApp::_itemsTextureAtlas) {
      NinecraftApp::_itemsTextureAtlas->load((NinecraftApp *)this->minecraft);
      Item::initItems(NinecraftApp::_itemsTextureAtlas);
    }
    if (this->minecraft->levelRenderer) {
      this->minecraft->levelRenderer->allChanged();
    }
  }

  statusMsg = "Texture Pack Applied Successfully!";
  needsRefresh = true;
}

void TexturesScreen::deleteSavedTexture(const std::string &path) {
  if (!path.empty()) {
    size_t slash = path.find_last_of("/\\");
    std::string title = (slash != std::string::npos) ? path.substr(slash + 1) : path;
    if (title == activeTexturePack) {
      restoreDefaultTextures();
    }
  }
  FileUtil::RemoveDirectory(path);
  statusMsg = "Texture Pack deleted.";
  needsRefresh = true;
}

void TexturesScreen::restoreDefaultTextures() {
  std::string dataPath = this->minecraft ? this->minecraft->dataPathMaybe : "";
  if (dataPath.empty()) {
    const char* home = getenv("HOME");
    if (home) dataPath = std::string(home) + "/.minecraftpe";
    else dataPath = ".";
  }
  std::string activeFile = dataPath + "/saved_textures/_active.txt";
  remove(activeFile.c_str());
  activeTexturePack = "";

  extern uint8_t* g_terrainAtlasPixels;
  extern uint8_t* g_itemsAtlasPixels;
  if (g_terrainAtlasPixels) {
    free(g_terrainAtlasPixels);
    g_terrainAtlasPixels = nullptr;
  }
  if (g_itemsAtlasPixels) {
    free(g_itemsAtlasPixels);
    g_itemsAtlasPixels = nullptr;
  }

  if (this->minecraft) {
    if (this->minecraft->texturesPtr) {
      this->minecraft->texturesPtr->reloadAll();
    }
    if (NinecraftApp::_terrainTextureAtlas) {
      NinecraftApp::_terrainTextureAtlas->load((NinecraftApp *)this->minecraft);
      Tile::initTiles(NinecraftApp::_terrainTextureAtlas);
    }
    if (NinecraftApp::_itemsTextureAtlas) {
      NinecraftApp::_itemsTextureAtlas->load((NinecraftApp *)this->minecraft);
      Item::initItems(NinecraftApp::_itemsTextureAtlas);
    }
    if (this->minecraft->levelRenderer) {
      this->minecraft->levelRenderer->allChanged();
    }
  }

  statusMsg = "Default Textures Restored.";
  needsRefresh = true;
}

void TexturesScreen::render(int32_t mx, int32_t my, float pt) {
  renderMenuBg32();
  setupPositions();

  drawCenteredString(font, "Texture Packs", width / 2, 12, 0xFFFFFFFF);
  drawCenteredString(font, "Select or import custom textures", width / 2, 26,
                     0xFFA0A0A0);

  if (!statusMsg.empty()) {
    drawCenteredString(font, statusMsg, width / 2, height - 46, 0xFF88FF88);
  }
  if (installRunning) {
    drawCenteredString(font, "Importing Texture Pack...", width / 2,
                       height - 46, 0xFFFFAA00);
  } else if (installDone && !installMsg.empty()) {
    drawCenteredString(font, installMsg, width / 2, height - 46, 0xFF88FF88);
  }

  int startY = 70;
  int endY = height - 52;
  int y = startY - scrollOffset;

  if (savedTextures.empty()) {
    drawCenteredString(font, "No custom texture packs saved.", width / 2,
                       startY + 20, 0xFF888888);
    drawCenteredString(font, "Click 'Import Pack (.zip)...' to add one!",
                       width / 2, startY + 36, 0xFFA0A0A0);
  } else {
    for (int i = 0; i < (int)savedTextures.size(); ++i) {
      if (i >= (int)managedApplyButtons.size() ||
          i >= (int)managedDeleteButtons.size())
        break;

      bool visible = (y + 36 > startY && y < endY);
      bool isActive = (savedTextures[i].title == activeTexturePack);

      managedApplyButtons[i]->visible = visible;
      managedApplyButtons[i]->active = visible && !isActive;
      managedDeleteButtons[i]->visible = visible;
      managedDeleteButtons[i]->active = visible;

      if (visible) {
        fill(width / 2 - 160, y, width / 2 + 160, y + 36,
             isActive ? 0x99004400 : 0x88000000);
        drawString(font, savedTextures[i].title, width / 2 - 150, y + 6,
                   isActive ? 0xFF55FF55 : 0xFFFFFFFF);
        drawString(font, isActive ? "[Active Texture Pack]" : "[Available]",
                   width / 2 - 150, y + 20, isActive ? 0xFF55FF55 : 0xFF888888);

        managedApplyButtons[i]->posX = width / 2 + 25;
        managedApplyButtons[i]->posY = y + 7;
        managedDeleteButtons[i]->posX = width / 2 + 92;
        managedDeleteButtons[i]->posY = y + 7;
      }

      y += 40;
    }
  }

  int contentHeight = (int)savedTextures.size() * 40;
  int maxScroll = std::max(0, contentHeight - (endY - startY));
  if (scrollOffset < 0)
    scrollOffset = 0;
  if (scrollOffset > maxScroll)
    scrollOffset = maxScroll;

  Screen::render(mx, my, pt);
}

bool_t TexturesScreen::handleBackEvent(bool_t a2) {
  shouldClose = true;
  return 1;
}

void TexturesScreen::tick() {
  if (shouldClose) {
    shouldClose = false;
    if (this->minecraft) {
      if (this->lastScreen) {
        this->minecraft->setScreen(this->lastScreen);
      } else {
        this->minecraft->screenChooser.setScreen(START_MENU_SCREEN);
      }
    }
    return;
  }
  if (installDone) {
    installDone = false;
    needsRefresh = true;
  }
  if (needsRefresh) {
    needsRefresh = false;
    refreshSavedTextures();
  }
}

bool_t TexturesScreen::isInGameScreen() { return 0; }

void TexturesScreen::buttonClicked(Button *btn) {
  if (!btn)
    return;
  if (btn == btnBack) {
    shouldClose = true;
  } else if (btn == btnImport) {
    importTexturePack();
  } else if (btn == btnDefault) {
    restoreDefaultTextures();
  } else {
    int id = btn->buttonID;
    if (id >= 100 && id < 200) {
      int idx = id - 100;
      if (idx >= 0 && idx < (int)savedTextures.size()) {
        applySavedTexture(savedTextures[idx].path, savedTextures[idx].title);
      }
    } else if (id >= 200 && id < 300) {
      int idx = id - 200;
      if (idx >= 0 && idx < (int)savedTextures.size()) {
        deleteSavedTexture(savedTextures[idx].path);
      }
    }
  }
}

void TexturesScreen::mouseClicked(int32_t mx, int32_t my, int32_t btn) {
  if (btn == 1) {
    isDragging = true;
    prevMouseY = my;
  }
  Screen::mouseClicked(mx, my, btn);
}

void TexturesScreen::mouseReleased(int32_t mx, int32_t my, int32_t btn) {
  if (btn == 1) {
    isDragging = false;
  }
  Screen::mouseReleased(mx, my, btn);
}
