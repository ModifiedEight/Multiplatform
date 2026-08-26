#include <Minecraft.hpp>
#include <cpputils.hpp>
#include <gui/NinePatchFactory.hpp>
#include <gui/buttons/ImageButton.hpp>
#include <gui/elements/Label.hpp>
#include <gui/elements/TextBox.hpp>
#include <gui/screens/CreateWorldScreen.hpp>
#include <gui/screens/ManageMCOServerScreen.hpp>
#include <gui/screens/PlayScreen.hpp>
#include <gui/screens/ProgressScreen.hpp>
#include <level/LevelSettings.hpp>
#include <level/storage/LevelStorageSource.hpp>
#include <rendering/Tesselator.hpp>
#include <util/IntRectangle.hpp>
#include <util/ParameterStringify.hpp>
#include <util/Util.hpp>
#include <utils.h>

static bool CreateWorldScreen_useAdvancedGen = false;

CreateWorldScreen::CreateWorldScreen(CreateWorldScreenType a2,
                                     const MCOServerListItem &a3)
    : SelectWorldScreen() {
  this->field_12C = 0;
  this->field_130 = 0;
  this->field_134 = 0;
  this->field_138 = 0;
  this->field_13C = 0;
  this->field_140 = 0;
  this->field_144 = 0;
  this->field_148 = 0;
  this->field_14C = 0;
  this->field_150 = 0;
  this->field_158 = 0;
  this->field_15C = 0;
  this->field_160 = 0;
  this->field_164 = 0;
  this->field_168 = 0;
  this->field_16C = a2;
  this->field_170 = a3;
  this->worldTypeOldButton = 0;
  this->worldTypeInfButton = 0;
  this->worldTypeFlatButton = 0;
  this->selectedWorldType = 0;
  this->worldTypeLabel = 0;
  this->btnAdvancedSettings = 0;
  this->btnMoreOptions = 0;
  this->btnCaves = 0;
  this->btnMonsters = 0;
  this->btnAnimals = 0;
  this->btnTimeFreeze = 0;
  this->currentTab = 0;
  this->optCaves = 1;
  this->optMonsters = 1;
  this->optAnimals = 1;
  this->optTimeFreeze = 0;
}
void CreateWorldScreen::closeScreen() {
  CreateWorldScreenType v2; // r6
  Minecraft *minecraft;     // r5

  v2 = this->field_16C;
  minecraft = this->minecraft;
  if (v2 == WST_MCOGAME_RECREATE) {
    minecraft->setScreen(new ManageMCOServerScreen(this->field_170));
  } else {
    minecraft->setScreen(new PlayScreen(v2 == WST_LOCALGAME));
  }
}
void CreateWorldScreen::generateLocalGame() {
  this->minecraft->getLevelSource()->getLevelList(this->field_50);
  std::string text(*this->field_144->getText());
  if (text == "")
    text = "Level";
  std::string ret = this->getUniqueLevelName(text);
  int32_t genType = 1;
  if (this->selectedWorldType == this->worldTypeInfButton)
    genType = 1;
  else if (this->selectedWorldType == this->worldTypeFlatButton)
    genType = 2;
  else if (this->selectedWorldType == this->worldTypeOldButton)
    genType = 0;
  this->minecraft->selectLevel(
      ret, text, LevelSettings{this->getSeed(), this->isCreative(), genType, this->optCaves, this->optMonsters, this->optAnimals, this->optTimeFreeze});
  this->minecraft->hostMultiplayer(19132);
  this->minecraft->setScreen(new ProgressScreen());
  std::string v16;
  if (this->field_148->text == "") {
    std::string v21 = "{\"%\": \"%\", \"%\": \"%\"}";
    const char *v11 = this->isCreative() ? "creative" : "survival";
    std::string v18 = this->field_148->text;
    std::vector<std::string> v24;
    v24.push_back("game_type");
    v24.push_back(v11);
    v24.push_back("seed");
    v24.push_back(v18);
    v16 = Util::simpleFormat(v21, v24);
  } else {
    std::string v21 = "{\"%\": \"%\"}";
    const char *v11 = this->isCreative() ? "creative" : "survival";
    std::vector<std::string> v24;
    v24.push_back("game_type");
    v24.push_back(v11);
    v16 = Util::simpleFormat(v21, v24);
  }
  this->minecraft->platform()->statsTrackData("create_world", v16);
}
void CreateWorldScreen::generateMCOGame(bool_t) {
  printf("CreateWorldScreen::generateMCOGame - not implemented\n");
  // TODO
}
std::string CreateWorldScreen::getLevelName() {
  if (*this->field_144->getText() == "") {
    return *this->field_144->getText();
  } else {
    return "Level";
  }
}
int32_t CreateWorldScreen::getSeed() {
  std::string text = *this->field_148->getText();
  if (text.size() <= 1) {
    return getEpochTimeS();
  }
  std::string s = Util::stringTrim(text);
  if (s.size() == 0) {
    return getEpochTimeS();
  }
  int32_t v7;
  if (sscanf(s.c_str(), "%d", &v7) <= 0) {
    return Util::hashCode(s);
  } else {
    return v7;
  }
}
bool_t CreateWorldScreen::isCreative() {
  return this->field_134 == this->field_12C;
}
void CreateWorldScreen::setGameType(bool_t a2) {
  if (a2)
    this->field_154->setText("Easily destroy and place blocks. No damage, "
                             "flying and other cool stuff.");
  else
    this->field_154->setText("Limited resources, you'll need tools. You may "
                             "get hurt. Watch out for Monsters.");
}
void CreateWorldScreen::waitForMCO() {
  this->field_12C->visible = 0;
  this->field_130->visible = 0;
  this->field_138->visible = 0;
  this->field_168 = 1;
}

void CreateWorldScreen::updateTabVisibility() {
  bool isMain = (this->currentTab == 0);

  if (this->field_12C) {
    this->field_12C->visible = isMain;
    this->field_12C->active = isMain;
  }
  if (this->field_130) {
    this->field_130->visible = isMain;
    this->field_130->active = isMain;
  }
  if (this->worldTypeOldButton) {
    this->worldTypeOldButton->visible = isMain;
    this->worldTypeOldButton->active = isMain;
  }
  if (this->worldTypeInfButton) {
    this->worldTypeInfButton->visible = isMain;
    this->worldTypeInfButton->active = isMain;
  }
  if (this->worldTypeFlatButton) {
    this->worldTypeFlatButton->visible = isMain;
    this->worldTypeFlatButton->active = isMain;
  }

  if (this->btnAdvancedSettings) {
    this->btnAdvancedSettings->visible = !isMain;
    this->btnAdvancedSettings->active = !isMain;
  }
  if (this->btnCaves) {
    this->btnCaves->visible = !isMain;
    this->btnCaves->active = !isMain;
  }
  if (this->btnMonsters) {
    this->btnMonsters->visible = !isMain;
    this->btnMonsters->active = !isMain;
  }
  if (this->btnAnimals) {
    this->btnAnimals->visible = !isMain;
    this->btnAnimals->active = !isMain;
  }
  if (this->btnTimeFreeze) {
    this->btnTimeFreeze->visible = !isMain;
    this->btnTimeFreeze->active = !isMain;
  }

  if (this->btnMoreOptions) {
    ((Touch::TButton*)this->btnMoreOptions)->setMsg(isMain ? "More options" : "< Basic options");
  }
  if (this->field_140) {
    ((Touch::THeader*)this->field_140)->setMsg(isMain ? "Create a Local Game" : "More Options");
  }
}

CreateWorldScreen::~CreateWorldScreen() {
  safeRemove<Button>(this->field_138);
  safeRemove<Button>(this->field_13C);
  safeRemove<TextBox>(this->field_144);
  safeRemove<TextBox>(this->field_148);
  safeRemove<Button>(this->field_140);
  safeRemove<Label>(this->field_14C);
  safeRemove<Label>(this->field_150);
  safeRemove<Label>(this->field_154);
  safeRemove<ImageButton>(this->field_12C);
  safeRemove<ImageButton>(this->field_130);
  safeRemove<NinePatchLayer>(this->field_15C);
  safeRemove<NinePatchLayer>(this->field_160);
  safeRemove<NinePatchLayer>(this->field_158);
  safeRemove<Label>(this->field_164);
  safeRemove<ImageButton>(this->worldTypeOldButton);
  safeRemove<ImageButton>(this->worldTypeInfButton);
  safeRemove<ImageButton>(this->worldTypeFlatButton);
  safeRemove<Button>(this->btnAdvancedSettings);
  safeRemove<Button>(this->btnMoreOptions);
  safeRemove<Button>(this->btnCaves);
  safeRemove<Button>(this->btnMonsters);
  safeRemove<Button>(this->btnAnimals);
  safeRemove<Button>(this->btnTimeFreeze);
  safeRemove<Label>(this->worldTypeLabel);
}
void CreateWorldScreen::render(int32_t a2, int32_t a3, float a4) {
  Label *v8; // r0

  if (this->currentTab == 0 && (this->field_144->suppressOtherGUI() ||
      this->field_148->suppressOtherGUI())) {
    this->renderBackground(0);
    this->field_144->topRender(this->minecraft, a2, a3);
    this->field_148->topRender(this->minecraft, a2, a3);
  } else {
    this->renderMenuBackground(a4);
    this->field_158->draw(Tesselator::instance, 5.0,
                          (float)this->field_14C->posY - 5.0);
    if (this->field_168) {
      v8 = this->field_164;
      v8->render(this->minecraft, a2, a3);
    } else if (this->currentTab == 0) {
      this->field_14C->render(this->minecraft, a2, a3);
      this->field_144->render(this->minecraft, a2, a3);
      this->field_150->render(this->minecraft, a2, a3);
      this->field_148->render(this->minecraft, a2, a3);
      this->field_154->render(this->minecraft, a2, a3);
    }
    Screen::render(a2, a3, a4);
  }
}
void CreateWorldScreen::init() {
  const char *v3;
  if (this->field_16C == WST_MCOGAME_NEW)
    v3 = "Create a Realms Server";
  else if (this->field_16C == WST_LOCALGAME)
    v3 = "Create a Local Game";
  else
    v3 = "Create a Local Game";

  std::string v47 = v3;
  this->field_140 = new Touch::THeader(0, v47);
  std::string v48 = this->field_16C == WST_MCOGAME_RECREATE ? "Reset Realm"
                                                            : "Generate World";
  this->field_138 = new Touch::TButton(2, v48, 0);
  ((Touch::TButton *)this->field_138)
      ->init(this->minecraft, "gui/spritesheet.png", {8, 32, 8, 8},
             {0, 32, 8, 8}, 2, 2, this->field_138->width,
             this->field_138->height);
  const char *extAscii = TextBox::extendedAcsii
                             ? TextBox::extendedAcsii
                             : " !\"#$%&\'()*+,-./"
                               "0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]"
                               "^_`abcdefghijklmnopqrstuvwxyz{|}~";
  this->field_144 = new TextBox(this->minecraft, "Name", 16, extAscii,
                                strlen(extAscii), 0, 0, 0, 0);
  this->field_144->text = this->field_170.worldName;
  this->field_148 = new TextBox(this->minecraft, "Seed", 32, extAscii,
                                strlen(extAscii), 0, 0, 0, 0);
  this->field_13C = new Touch::TButton(3, "Back", 0);
  this->field_13C->width = 38;
  this->field_13C->height = 18;
  this->field_14C = new Label("Name", this->minecraft, -1, 0, 0, 0, 1);
  this->field_150 = new Label("Seed", this->minecraft, -1, 0, 0, 0, 1);
  this->field_154 =
      new Label("", this->minecraft, -1, 0, 0, this->width / 2 - 17, 1);
  std::string v49 = this->field_16C == WST_MCOGAME_NEW
                        ? "Creating new world..."
                        : "Resetting your realm...";
  this->field_164 = new Label(v49, this->minecraft, -1, 0, 0, 0, 1);
  ((Touch::TButton *)this->field_13C)->init(this->minecraft);
  NinePatchFactory a1(this->minecraft->texturesPtr, "gui/spritesheet.png");
  this->field_158 = a1.createSymmetrical({34, 43, 14, 14}, 3, 3, 32, 32);
  this->field_15C = a1.createSymmetrical({8, 32, 8, 8}, 2, 2, 38, 26);
  this->field_160 = a1.createSymmetrical({0, 32, 8, 8}, 2, 2, 38, 26);
  this->field_12C =
      new CategoryButton("Creative", 4, this->field_15C, this->field_160,
                         (Button **)&this->field_134);
  this->field_12C->width = 38;
  this->field_12C->height = 26;
  this->field_12C->setYOffset(this->field_12C->height / 2 - 4);
  this->field_130 =
      new CategoryButton("Survival", 5, this->field_15C, this->field_160,
                         (Button **)&this->field_134);
  this->field_130->width = 38;
  this->field_130->height = 26;
  this->field_130->setYOffset(this->field_130->height / 2 - 4);
  bool v36 = this->field_170.gamemodeName.compare("creative") == 0;
  if (v36) {
    this->field_134 = this->field_12C;
  } else {
    this->field_134 = this->field_130;
  }
  this->setGameType(v36);
  this->worldTypeOldButton =
      new CategoryButton("Old", 6, this->field_15C, this->field_160,
                         (Button **)&this->selectedWorldType);
  this->worldTypeOldButton->width = 38;
  this->worldTypeOldButton->height = 26;
  this->worldTypeOldButton->setYOffset(this->worldTypeOldButton->height / 2 -
                                       4);
  this->worldTypeInfButton =
      new CategoryButton("Infinite", 7, this->field_15C, this->field_160,
                         (Button **)&this->selectedWorldType);
  this->worldTypeInfButton->width = 38;
  this->worldTypeInfButton->height = 26;
  this->worldTypeInfButton->setYOffset(this->worldTypeInfButton->height / 2 -
                                       4);
  this->worldTypeFlatButton =
      new CategoryButton("Flat", 8, this->field_15C, this->field_160,
                         (Button **)&this->selectedWorldType);
  this->worldTypeFlatButton->width = 38;
  this->worldTypeFlatButton->height = 26;
  this->worldTypeFlatButton->setYOffset(this->worldTypeFlatButton->height / 2 -
                                        4);
  this->selectedWorldType = this->worldTypeInfButton;
  
  this->btnAdvancedSettings = new Touch::TButton(10, CreateWorldScreen_useAdvancedGen ? "Terrain: New" : "Terrain: Old (0.8.1)", 0);
  ((Touch::TButton *)this->btnAdvancedSettings)->init(this->minecraft);
  this->btnAdvancedSettings->width = 100;
  this->btnAdvancedSettings->height = 26;

  this->btnMoreOptions = new Touch::TButton(11, "More options", 0);
  ((Touch::TButton *)this->btnMoreOptions)
      ->init(this->minecraft, "gui/spritesheet.png", {8, 32, 8, 8},
             {0, 32, 8, 8}, 2, 2, this->btnMoreOptions->width,
             this->btnMoreOptions->height);

  this->btnCaves = new Touch::TButton(12, this->optCaves ? "Caves: ON" : "Caves: OFF", 0);
  ((Touch::TButton *)this->btnCaves)->init(this->minecraft);

  this->btnMonsters = new Touch::TButton(13, this->optMonsters ? "Monsters: ON" : "Monsters: OFF", 0);
  ((Touch::TButton *)this->btnMonsters)->init(this->minecraft);

  this->btnAnimals = new Touch::TButton(14, this->optAnimals ? "Animals: ON" : "Animals: OFF", 0);
  ((Touch::TButton *)this->btnAnimals)->init(this->minecraft);

  this->btnTimeFreeze = new Touch::TButton(15, this->optTimeFreeze ? "Time Freeze: ON" : "Time Freeze: OFF", 0);
  ((Touch::TButton *)this->btnTimeFreeze)->init(this->minecraft);

  this->worldTypeInfButton->visible = 1;
  this->worldTypeInfButton->active = 1;
  this->worldTypeLabel = nullptr;
  this->buttons.push_back(this->field_140);
  this->buttons.push_back(this->field_138);
  this->buttons.push_back(this->btnMoreOptions);
  this->buttons.push_back(this->field_13C);
  this->buttons.emplace_back(this->field_12C);
  this->buttons.emplace_back(this->field_130);
  this->buttons.emplace_back(this->worldTypeOldButton);
  this->buttons.emplace_back(this->worldTypeInfButton);
  this->buttons.emplace_back(this->worldTypeFlatButton);
  this->buttons.push_back(this->btnAdvancedSettings);
  this->buttons.push_back(this->btnCaves);
  this->buttons.push_back(this->btnMonsters);
  this->buttons.push_back(this->btnAnimals);
  this->buttons.push_back(this->btnTimeFreeze);

  this->field_2C.emplace_back(this->field_12C);
  this->field_2C.emplace_back(this->field_130);
  this->field_2C.push_back(this->field_13C);
  this->field_2C.push_back(this->field_138);
  this->field_2C.push_back(this->btnMoreOptions);
  this->field_2C.emplace_back(this->worldTypeOldButton);
  this->field_2C.emplace_back(this->worldTypeInfButton);
  this->field_2C.emplace_back(this->worldTypeFlatButton);
  this->field_2C.push_back(this->btnAdvancedSettings);
  this->field_2C.push_back(this->btnCaves);
  this->field_2C.push_back(this->btnMonsters);
  this->field_2C.push_back(this->btnAnimals);
  this->field_2C.push_back(this->btnTimeFreeze);

  this->updateTabVisibility();
}
void CreateWorldScreen::setupPositions() {
  int32_t v2;  // r5
  Label *v7;   // r1
  TextBox *v8; // r1
  TextBox *v9; // r1

  this->field_13C->posX = 4;
  this->field_13C->posY = 4;
  this->field_140->posX = 0;
  this->field_140->posY = 0;
  this->field_140->width = this->width;
  this->field_140->height = this->field_13C->height + 8;
  v2 = this->width / 2;
  this->field_154->setWidth(v2 - 12);
  this->field_154->posX = v2 + v2 / 2 - this->field_154->width / 2 - 3;

  v7 = this->field_14C;
  this->field_150->posX = 11;
  v7->posX = 11;
  v8 = this->field_144;
  this->field_148->width = 100;
  v8->width = 100;
  v9 = this->field_144;
  this->field_148->posX = 10;
  v9->posX = 10;
  this->field_14C->posY = this->field_140->height + 15;
  this->field_144->posY = this->field_14C->posY + 10;
  this->field_150->posY = this->field_144->posY + this->field_144->height + 13;
  this->field_148->posY = this->field_150->posY + 10;

  int32_t gameModeY = this->field_140->height + 15;
  int32_t gameModeW = (this->field_154->width - 4) / 2;
  this->field_12C->width = gameModeW;
  this->field_130->width = gameModeW;
  this->field_12C->posY = gameModeY;
  this->field_130->posY = gameModeY;
  this->field_12C->posX = this->field_154->posX;
  this->field_130->posX = this->field_154->posX + gameModeW + 4;

  this->field_154->posY = gameModeY + this->field_12C->height + 4;

  int32_t worldTypeRowY = this->field_154->posY + 26;
  if (this->worldTypeOldButton) {
    if (this->minecraft->supportNonTouchscreen()) {
      int32_t btnW = this->field_154->width / 2;
      this->worldTypeOldButton->width = btnW;
      this->worldTypeFlatButton->width = btnW;
      this->worldTypeOldButton->posY = worldTypeRowY;
      this->worldTypeFlatButton->posY = worldTypeRowY;
      this->worldTypeOldButton->posX = this->field_154->posX;
      this->worldTypeFlatButton->posX = this->field_154->posX + btnW;

      this->worldTypeInfButton->width = btnW;
      this->worldTypeInfButton->posY =
          worldTypeRowY + this->worldTypeOldButton->height + 4;
      this->worldTypeInfButton->posX = this->field_154->posX;
    } else {
      int32_t startX = this->field_154->posX - 35;
      int32_t totalW = this->field_154->width + 35;
      int32_t btnW = totalW / 3;
      this->worldTypeOldButton->width = btnW;
      this->worldTypeInfButton->width = btnW;
      this->worldTypeFlatButton->width = btnW;
      this->worldTypeOldButton->posY = worldTypeRowY;
      this->worldTypeInfButton->posY = worldTypeRowY;
      this->worldTypeFlatButton->posY = worldTypeRowY;
      this->worldTypeOldButton->posX = startX;
      this->worldTypeInfButton->posX = startX + btnW;
      this->worldTypeFlatButton->posX = startX + 2 * btnW;
    }
  }

  int32_t worldTypeHeight =
      this->worldTypeOldButton ? this->worldTypeOldButton->height : 26;
  if (this->worldTypeOldButton && this->minecraft->supportNonTouchscreen()) {
    worldTypeHeight = 2 * this->worldTypeOldButton->height + 4;
  }

  int32_t optBtnW = (this->width - 40) / 2;
  if (optBtnW > 140) optBtnW = 140;
  int32_t optBtnH = (this->height >= 260) ? 24 : 20;
  int32_t optSpacing = (this->height >= 260) ? 6 : 4;
  int32_t optTopY = this->field_14C->posY + 6;

  this->btnAdvancedSettings->width = optBtnW;
  this->btnAdvancedSettings->height = optBtnH;
  this->btnAdvancedSettings->posX = this->width / 2 - optBtnW - 4;
  this->btnAdvancedSettings->posY = optTopY;

  this->btnCaves->width = optBtnW;
  this->btnCaves->height = optBtnH;
  this->btnCaves->posX = this->width / 2 + 4;
  this->btnCaves->posY = optTopY;

  this->btnMonsters->width = optBtnW;
  this->btnMonsters->height = optBtnH;
  this->btnMonsters->posX = this->width / 2 - optBtnW - 4;
  this->btnMonsters->posY = optTopY + optBtnH + optSpacing;

  this->btnAnimals->width = optBtnW;
  this->btnAnimals->height = optBtnH;
  this->btnAnimals->posX = this->width / 2 + 4;
  this->btnAnimals->posY = this->btnMonsters->posY;

  this->btnTimeFreeze->width = optBtnW * 2 + 8;
  if (this->btnTimeFreeze->width > this->width - 24) this->btnTimeFreeze->width = this->width - 24;
  this->btnTimeFreeze->height = optBtnH;
  this->btnTimeFreeze->posX = this->width / 2 - this->btnTimeFreeze->width / 2;
  this->btnTimeFreeze->posY = this->btnMonsters->posY + optBtnH + optSpacing;

  int32_t bottomBtnW = (this->width - 40) / 2;
  if (bottomBtnW > 140) bottomBtnW = 140;
  int32_t bottomBtnH = (this->height >= 260) ? 24 : 20;
  int32_t bottomBtnY = this->height - bottomBtnH - 6;
  if (this->currentTab == 0) {
    int32_t calcY = worldTypeRowY + worldTypeHeight + 8;
    if (calcY < bottomBtnY) bottomBtnY = calcY;
  } else {
    int32_t optBottom = this->btnTimeFreeze->posY + this->btnTimeFreeze->height + 8;
    if (bottomBtnY < optBottom) bottomBtnY = optBottom;
  }

  this->field_138->width = bottomBtnW;
  this->field_138->height = bottomBtnH;
  this->field_138->posX = this->width / 2 - bottomBtnW - 4;
  this->field_138->posY = bottomBtnY;

  this->btnMoreOptions->width = bottomBtnW;
  this->btnMoreOptions->height = bottomBtnH;
  this->btnMoreOptions->posX = this->width / 2 + 4;
  this->btnMoreOptions->posY = bottomBtnY;

  this->field_15C->setSize((float)this->field_12C->width,
                           (float)this->field_12C->height);
  this->field_160->setSize((float)this->field_12C->width,
                           (float)this->field_12C->height);

  int32_t boxTopY = this->field_14C->posY - 5;
  int32_t boxBottomY = bottomBtnY + bottomBtnH + 5;
  this->field_158->setSize((float)this->width - 10.0f, (float)(boxBottomY - boxTopY));
  this->field_164->posX = this->width / 2 - this->field_164->width / 2;
  this->field_164->posY = boxTopY + this->field_158->height / 2;

  this->setGameType(this->isCreative());
  this->updateTabVisibility();
}
bool_t CreateWorldScreen::handleBackEvent(bool_t a2) {
  if (this->currentTab != 0) {
    this->currentTab = 0;
    this->updateTabVisibility();
    return 1;
  }
  if (!a2 && (!this->field_148->suppressOtherGUI() &&
                  !this->field_144->suppressOtherGUI() ||
              !this->field_148->backPressed(this->minecraft, 0) &&
                  !this->field_144->backPressed(this->minecraft, 0))) {
    this->closeScreen();
  }
  return 1;
}
void CreateWorldScreen::tick() {}
void CreateWorldScreen::feedMCOEvent(MCOEvent) {}
void CreateWorldScreen::setTextboxText(const std::string &a2) {
  TextBox *v4; // r0

  if (this->field_144->suppressOtherGUI()) {
    v4 = this->field_144;
  } else {
    if (!this->field_148->suppressOtherGUI()) {
      return;
    }
    v4 = this->field_148;
  }
  v4->setText(a2);
}
void CreateWorldScreen::buttonClicked(Button *a2) {
  bool_t isCreative;        // r1
  CreateWorldScreenType v4; // r3
  int32_t v5;               // r1

  if (a2 == this->field_13C) {
    if (this->currentTab != 0) {
      this->currentTab = 0;
      this->updateTabVisibility();
      return;
    }
    this->closeScreen();
    return;
  }
  if (a2 == this->btnMoreOptions) {
    this->currentTab = (this->currentTab == 0) ? 1 : 0;
    this->updateTabVisibility();
    return;
  }
  if (a2 == this->btnCaves) {
    this->optCaves = !this->optCaves;
    ((Touch::TButton*)this->btnCaves)->setMsg(this->optCaves ? "Caves: ON" : "Caves: OFF");
    return;
  }
  if (a2 == this->btnMonsters) {
    this->optMonsters = !this->optMonsters;
    ((Touch::TButton*)this->btnMonsters)->setMsg(this->optMonsters ? "Monsters: ON" : "Monsters: OFF");
    return;
  }
  if (a2 == this->btnAnimals) {
    this->optAnimals = !this->optAnimals;
    ((Touch::TButton*)this->btnAnimals)->setMsg(this->optAnimals ? "Animals: ON" : "Animals: OFF");
    return;
  }
  if (a2 == this->btnTimeFreeze) {
    this->optTimeFreeze = !this->optTimeFreeze;
    ((Touch::TButton*)this->btnTimeFreeze)->setMsg(this->optTimeFreeze ? "Time Freeze: ON" : "Time Freeze: OFF");
    return;
  }
  if (a2 == this->field_12C || a2 == this->field_130) {
    this->field_134 = (ImageButton *)a2;
    isCreative = this->isCreative();
    this->setGameType(isCreative);
    return;
  }
  if (a2 == this->worldTypeOldButton || a2 == this->worldTypeInfButton ||
      a2 == this->worldTypeFlatButton) {
    this->selectedWorldType = a2;
    return;
  }
  if (a2 == this->btnAdvancedSettings) {
    CreateWorldScreen_useAdvancedGen = !CreateWorldScreen_useAdvancedGen;
    ((Touch::TButton*)this->btnAdvancedSettings)->setMsg(CreateWorldScreen_useAdvancedGen ? "Terrain: New" : "Terrain: Old (0.8.1)");
    return;
  }
  if (a2 == this->field_138) {
    v4 = this->field_16C;
    switch (v4) {
    case WST_LOCALGAME:
      this->generateLocalGame();
      return;
    case WST_MCOGAME_NEW:
      v5 = 0;
      break;
    case WST_MCOGAME_RECREATE:
      v5 = 1;
      break;
    default:
      return;
    }
    this->generateMCOGame(v5);
  }
}
void CreateWorldScreen::mouseClicked(int32_t a2, int32_t a3, int32_t a4) {
  TextBox *v8; // r0

  if (this->currentTab == 0) {
    if (this->field_144->suppressOtherGUI()) {
      v8 = this->field_144;
    LABEL_5:
      v8->focusuedMouseClicked(this->minecraft, a2, a3, a4);
      return;
    }
    if (this->field_148->suppressOtherGUI()) {
      v8 = this->field_148;
      goto LABEL_5;
    }
    this->field_144->mouseClicked(this->minecraft, a2, a3, a4);
    this->field_148->mouseClicked(this->minecraft, a2, a3, a4);
  }
  Screen::mouseClicked(a2, a3, a4);
}
void CreateWorldScreen::mouseReleased(int32_t a2, int32_t a3, int32_t a4) {
  TextBox *v8; // r0

  if (this->currentTab == 0) {
    if (this->field_144->suppressOtherGUI()) {
      v8 = this->field_144;
    LABEL_5:
      v8->focusuedMouseReleased(this->minecraft, a2, a3, a4);
      return;
    }
    if (this->field_148->suppressOtherGUI()) {
      v8 = this->field_148;
      goto LABEL_5;
    }
    this->field_144->mouseReleased(this->minecraft, a2, a3, a4);
    this->field_148->mouseReleased(this->minecraft, a2, a3, a4);
  }
  Screen::mouseReleased(a2, a3, a4);
}
void CreateWorldScreen::keyPressed(int32_t a2) {
  TextBox *v4; // r0

  if (this->currentTab == 0) {
    if (this->field_144->suppressOtherGUI()) {
      v4 = this->field_144;
    LABEL_5:
      v4->keyPressed(this->minecraft, a2);
      return;
    }
    if (this->field_148->suppressOtherGUI()) {
      v4 = this->field_148;
      goto LABEL_5;
    }
    this->field_144->keyPressed(this->minecraft, a2);
    this->field_148->keyPressed(this->minecraft, a2);
  }
  Screen::keyPressed(a2);
}
void CreateWorldScreen::keyboardNewChar(const std::string &a2, bool_t a3) {
  if (this->currentTab == 0) {
    if (this->field_144->suppressOtherGUI()) {
      this->field_144->keyboardNewChar(this->minecraft, a2, a3);
      return;
    }
    if (this->field_148->suppressOtherGUI()) {
      this->field_148->keyboardNewChar(this->minecraft, a2, a3);
      return;
    }
    this->field_144->keyboardNewChar(this->minecraft, a2, a3);
    this->field_148->keyboardNewChar(this->minecraft, a2, a3);
  }
  Screen::keyboardNewChar(a2, a3);
}
