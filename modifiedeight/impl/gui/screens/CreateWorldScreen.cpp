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
  // 0 = Old, 1 = Infinite, 2 = Flat
  int32_t genType = 0;
  if (this->selectedWorldType == this->worldTypeInfButton)
    genType = CreateWorldScreen_useAdvancedGen ? 3 : 1;
  else if (this->selectedWorldType == this->worldTypeFlatButton)
    genType = 2;
  else if (this->selectedWorldType == this->worldTypeOldButton)
    genType = CreateWorldScreen_useAdvancedGen ? 4 : 0;
  this->minecraft->selectLevel(
      ret, text, LevelSettings{this->getSeed(), this->isCreative(), genType});
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
  safeRemove<Label>(this->worldTypeLabel);
}
void CreateWorldScreen::render(int32_t a2, int32_t a3, float a4) {
  Label *v8; // r0

  if (this->field_144->suppressOtherGUI() ||
      this->field_148->suppressOtherGUI()) {
    this->renderBackground(0);
    this->field_144->topRender(this->minecraft, a2, a3);
    this->field_148->topRender(this->minecraft, a2, a3);
  } else {
    this->renderMenuBackground(a4);
    this->field_158->draw(Tesselator::instance, 5.0,
                          (float)this->field_14C->posY - 5.0);
    if (this->field_168) {
      v8 = this->field_164;
    } else {
      this->field_14C->render(this->minecraft, a2, a3);
      this->field_144->render(this->minecraft, a2, a3);
      this->field_150->render(this->minecraft, a2, a3);
      this->field_148->render(this->minecraft, a2, a3);
      v8 = this->field_154;
    }
    v8->render(this->minecraft, a2, a3);
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
  this->selectedWorldType = this->worldTypeOldButton;

  this->btnAdvancedSettings = new Touch::TButton(
      10,
      CreateWorldScreen_useAdvancedGen ? "Terrain: New"
                                       : "Terrain: Old (0.8.1)",
      0);
  ((Touch::TButton *)this->btnAdvancedSettings)->init(this->minecraft);
  this->btnAdvancedSettings->width = 100;
  this->btnAdvancedSettings->height = 26;
  this->worldTypeInfButton->visible = 1;
  this->worldTypeInfButton->active = 1;
  this->worldTypeLabel =
      new Label("World Type", this->minecraft, -1, 0, 0, 0, 1);
  this->buttons.push_back(this->field_140);
  this->buttons.push_back(this->field_138);
  this->buttons.push_back(this->field_13C);
  this->buttons.emplace_back(this->field_12C);
  this->buttons.emplace_back(this->field_130);
  this->buttons.emplace_back(this->worldTypeOldButton);
  this->buttons.emplace_back(this->worldTypeInfButton);
  this->buttons.emplace_back(this->worldTypeFlatButton);
  this->buttons.push_back(this->btnAdvancedSettings);
  this->field_2C.emplace_back(this->field_12C);
  this->field_2C.emplace_back(this->field_130);
  this->field_2C.push_back(this->field_13C);
  this->field_2C.push_back(this->field_138);
  this->field_2C.emplace_back(this->worldTypeOldButton);
  this->field_2C.emplace_back(this->worldTypeInfButton);
  this->field_2C.emplace_back(this->worldTypeFlatButton);
  this->field_2C.push_back(this->btnAdvancedSettings);
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
  this->field_138->width = (int)(float)((float)this->width / 2.3);
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
  if (this->worldTypeLabel) {
    this->worldTypeLabel->posX = 11;
    this->worldTypeLabel->posY =
        worldTypeRowY + (this->worldTypeOldButton
                             ? this->worldTypeOldButton->height / 2 - 4
                             : 0);
  }

  int32_t worldTypeHeight =
      this->worldTypeOldButton ? this->worldTypeOldButton->height : 26;
  if (this->worldTypeOldButton && this->minecraft->supportNonTouchscreen()) {
    worldTypeHeight = 2 * this->worldTypeOldButton->height + 4;
  }
  this->btnAdvancedSettings->posX = 11;
  this->btnAdvancedSettings->posY =
      this->field_148->posY + this->field_148->height + 10;

  this->field_138->posX = v2 - this->field_138->width / 2;
  this->field_138->posY = worldTypeRowY + worldTypeHeight + 8;

  this->field_15C->setSize((float)this->field_12C->width,
                           (float)this->field_12C->height);
  this->field_160->setSize((float)this->field_12C->width,
                           (float)this->field_12C->height);

  this->field_164->posX = this->width / 2 - this->field_164->width / 2;
  this->field_164->posY =
      this->field_14C->posY - 5 + this->field_158->height / 2;
  this->field_158->setSize(
      (float)this->width - 10.0,
      (float)((float)(this->field_138->posY + this->field_138->height) -
              (float)this->field_14C->posY) +
          10.0);
  this->setGameType(this->isCreative());
}
bool_t CreateWorldScreen::handleBackEvent(bool_t a2) {
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
    this->closeScreen();
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
    ((Touch::TButton *)this->btnAdvancedSettings)
        ->setMsg(CreateWorldScreen_useAdvancedGen ? "Terrain: New"
                                                  : "Terrain: Old (0.8.1)");
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
  Screen::mouseClicked(a2, a3, a4);
}
void CreateWorldScreen::mouseReleased(int32_t a2, int32_t a3, int32_t a4) {
  TextBox *v8; // r0

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
  Screen::mouseReleased(a2, a3, a4);
}
void CreateWorldScreen::keyPressed(int32_t a2) {
  TextBox *v4; // r0

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
  Screen::keyPressed(a2);
}
void CreateWorldScreen::keyboardNewChar(const std::string &a2, bool_t a3) {
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
  Screen::keyboardNewChar(a2, a3);
}
