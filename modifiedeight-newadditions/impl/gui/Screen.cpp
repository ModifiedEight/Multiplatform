#include <Minecraft.hpp>
#include <Options.hpp>
#include <cmath>
#include <gui/GuiElement.hpp>
#include <gui/Screen.hpp>
#include <gui/buttons/Button.hpp>
#include <input/Keyboard.hpp>
#include <input/Mouse.hpp>
#include <math/Mth.hpp>
#include <rendering/Tesselator.hpp>
#include <rendering/Textures.hpp>
#include <rendering/states/DisableState.hpp>
#include <sound/SoundEngine.hpp>
#include <unigl.h>
#include <util/Color4.hpp>

char_t *panorama_images[] = {
    "gui/background/panorama_0.png", "gui/background/panorama_1.png",
    "gui/background/panorama_2.png", "gui/background/panorama_3.png",
    "gui/background/panorama_4.png", "gui/background/panorama_5.png"};

Screen::Screen() {
  this->width = 1;
  this->height = 1;
  this->field_C = 0;
  this->minecraft = 0;
  this->field_20 = 0;
  this->field_24 = 0;
  this->field_28 = 0;
  this->field_44 = 0;
  this->font = 0;
  this->lastPressedButton = 0;
}

void Screen::updateTabButtonSelection() {
  if (!this->minecraft->useTouchscreen()) {
    for (int32_t i = 0; i < this->field_2C.size(); ++i) {
      Button *b = this->field_2C[i];
      b->text = (i - this->field_44) == 0;
    }
  }
}
void Screen::init(struct Minecraft *mc, int32_t w, int32_t h) {
  this->minecraft = mc;
  this->height = h;
  this->font = mc->font;
  this->width = w;
  this->init();
  this->setupPositions();
  this->updateTabButtonSelection();
}
void Screen::setSize(int32_t w, int32_t h) {
  this->width = w;
  this->height = h;
  this->setupPositions();
}

void Screen::render(int32_t x, int32_t y, float) {
  if (this->supppressedBySubWindow()) {
    for (GuiElement **start = this->elements.data();
         start != (this->elements.data() + this->elements.size()); ++start) {
      (*start)->topRender(this->minecraft, x, y);
    }
  } else {
    for (GuiElement **start = this->elements.data();
         start != (this->elements.data() + this->elements.size()); ++start) {
      (*start)->render(this->minecraft, x, y);
    }

    for (int i = 0; i < this->buttons.size(); ++i) {
      Button *b = this->buttons[i];
      if (!b->isOverrideScreenRendering()) {
        b->render(this->minecraft, x, y);
      }
    }
  }
}
void Screen::init() {}
void Screen::setupPositions(void) {
  for (GuiElement **start = this->elements.data();
       start != (this->elements.data() + this->elements.size()); ++start) {
    (*start)->setupPositions();
  }
}
void Screen::updateEvents() {
  if (!this->field_C) {
    while (Mouse::next()) {
      this->mouseEvent();
    }

    while (Keyboard::_index + 1 < Keyboard::_inputs.size()) {
      ++Keyboard::_index;
      this->keyboardEvent();
    }
    while (Keyboard::_textIndex + 1 < Keyboard::_inputText.size()) {
      ++Keyboard::_textIndex;
      this->keyboardTextEvent();
    }
  }
}
void Screen::mouseEvent(void) {
  MouseAction *ma = Mouse::getEvent();
  if (ma->isButton()) {
    bool_t b = Mouse::getEventButtonState();
    int32_t v5 = this->width * ma->field_0 / this->minecraft->field_1C;
    int32_t v6 = ma->field_2 * this->height / this->minecraft->field_20;
    int32_t evb = Mouse::getEventButton();
    if (b) {
      this->mouseClicked(v5, v6 - 1, evb);
    } else {
      this->mouseReleased(v5, v6 - 1, evb);
    }
  }
}
void Screen::keyboardEvent() {
  if (Keyboard::_inputs[Keyboard::_index].field_0) {
    this->keyPressed(Keyboard::_inputs[Keyboard::_index].field_4);
  }
}
void Screen::keyboardTextEvent() {
  this->keyboardNewChar(Keyboard::_inputText[Keyboard::_textIndex].field_0,
                        Keyboard::_inputText[Keyboard::_textIndex].field_4);
}
bool_t Screen::handleBackEvent(bool_t a2) {
  GuiElement *v5;
  GuiElement **start = this->elements.data();
  do {
    if (start == (this->elements.data() + this->elements.size()))
      return 0;
    v5 = *(start++);
  } while (!v5->backPressed(this->minecraft, a2));
  return 1;
}
void Screen::tick() {
  for (GuiElement **start = this->elements.data();
       start != (this->elements.data() + this->elements.size()); ++start) {
    (*start)->tick(this->minecraft);
  }
}
void Screen::removed() {}
void Screen::renderBackground(int32_t a2) {
  if (this->renderGameBehind()) {
    this->fill(0, 0, this->width, this->height, 0x7f000000);
  } else {
    this->renderDirtBackground(a2);
  }
}
void Screen::renderDirtBackground(int32_t a2) {
  this->minecraft->texturesPtr->loadAndBindTexture("gui/background.png");
  glColor4f(1, 1, 1, 1);
  float v5 = a2;
  Tesselator::instance.begin(4);
  Tesselator::instance.color(0x404040);
  Tesselator::instance.vertexUV(0, this->height, 0, 0,
                                v5 + (float)this->height * 0.03125);
  Tesselator::instance.vertexUV(this->width, this->height, 0,
                                (float)this->width * 0.03125,
                                v5 + (float)this->height * 0.03125);
  float v6 = v5 + 0;
  Tesselator::instance.vertexUV(this->width, 0, 0, (float)this->width * 0.03125,
                                v6);
  Tesselator::instance.vertexUV(0, 0, 0, 0, v6);
  Tesselator::instance.draw(1);
}

float dword_D6E05C20 = 0; // TODO check what value it has

void Screen::renderMenuBackground(float a2) {
  this->renderDirtBackground(0);
}

bool_t Screen::renderGameBehind() { return this->minecraft->options.graphics; }
bool_t Screen::hasClippingArea(struct IntRectangle &) { return 0; }
bool_t Screen::isPauseScreen() { return 1; }
bool_t Screen::isErrorScreen() { return 0; }
bool_t Screen::isInGameScreen() { return 1; }
bool_t Screen::closeOnPlayerHurt() { return 0; }
void Screen::confirmResult(bool_t, int32_t) {}
void Screen::lostFocus() {
  // TODO lostFocus
  // field_20 -> field_24: vector?
  printf("Screen::lostFocus - not implemented\n");
}
void Screen::toGUICoordinate(int32_t &x, int32_t &y) {
  x = this->width * x / this->minecraft->field_1C;
  y = this->height * y / this->minecraft->field_20 - 1;
}
void Screen::feedMCOEvent(MCOEvent) {}
bool_t Screen::supppressedBySubWindow() {
  int32_t v3 = 0;
  for (GuiElement **start = this->elements.data();
       start != (this->elements.data() + this->elements.size()); ++start) {
    if ((*start)->suppressOtherGUI())
      v3 = 1;
  }
  return v3;
}
void Screen::onTextBoxUpdated(int32_t) {}
void Screen::onMojangConnectorStatus(MojangConnectionStatus) {}
void Screen::setTextboxText(const std::string &a2) {
  for (auto &&e : this->elements) {
    if (e->suppressOtherGUI()) {
      e->setTextboxText(a2);
    }
  }
}
void Screen::onInternetUpdate() {}
void Screen::buttonClicked(struct Button *) {}
void Screen::mouseClicked(int32_t a2, int32_t a3, int32_t a4) {
  GuiElement **elements = this->elements.data();
  if (this->supppressedBySubWindow()) {
    while (elements != &this->elements.back()) {
      GuiElement *el = *elements++;
      if (el->suppressOtherGUI()) {
        el->focusuedMouseClicked(this->minecraft, a2, a3, a4);
      }
    }
  } else {
    while (elements != (this->elements.data() + this->elements.size())) {
      GuiElement *el = *elements++;
      el->mouseClicked(this->minecraft, a2, a3, a4);
    }

    if (a4 == 1) {
      for (int32_t i = 0;; ++i) {
        if (i >= this->buttons.size())
          break;
        Button *b = this->buttons[i];
        if (b->active) {
          if (b->clicked(this->minecraft, a2, a3)) {
            b->setPressed();
            this->lastPressedButton = b;
          }
        }
      }
    }
  }
}
void Screen::mouseReleased(int32_t a2, int32_t a3, int32_t a4) {
  GuiElement **elements = this->elements.data();
  if (this->supppressedBySubWindow()) {
    while (elements != &this->elements.back()) {
      GuiElement *el = *elements++;
      if (el->suppressOtherGUI()) {
        el->focusuedMouseReleased(this->minecraft, a2, a3, a4);
      }
    }
  } else {
    while (elements != (this->elements.data() + this->elements.size())) {
      GuiElement *el = *elements++;
      el->mouseReleased(this->minecraft, a2, a3, a4);
    }

    if (this->lastPressedButton) {
      if (a4 == 1) {
        for (int32_t i = 0;; ++i) {
          if (i >= this->buttons.size())
            break;

          Button *b = this->buttons[i];
          if (this->lastPressedButton == b) {
            if (this->lastPressedButton->clicked(this->minecraft, a2, a3)) {
              this->buttonClicked(this->lastPressedButton);
              this->minecraft->soundEngine->playUI("random.click", 1, 1);
              this->lastPressedButton->released(a2, a3);
            }
          }
        }
        this->lastPressedButton = 0;
      }
    }
  }
}
void Screen::keyPressed(int32_t a2) {
  for (auto &&e : this->elements) {
    e->keyPressed(this->minecraft, a2);
  }
  if (!this->minecraft->useTouchscreen()) {
    int v7 = this->field_2C.size();
    if (v7) {
      if (a2 == this->minecraft->options.keyMenuNext.keyCode) {
        int v9 = this->field_44 + 1;
        if (v9 == v7)
          v9 = 0;
        this->field_44 = v9;
      }
      if (a2 == this->minecraft->options.keyMenuPrevious.keyCode) {
        int v10 = this->field_44 - 1;
        this->field_44 = v10;
        if (v10 == -1) {
          this->field_44 = v7 - 1;
        }
      }

      if (a2 == this->minecraft->options.keyMenuOk.keyCode) {
        Button *b = this->field_2C[this->field_44];
        if (b->active) {
          this->minecraft->soundEngine->playUI("random.click", 1, 1);
          this->buttonClicked(b);
        }
      }
      this->updateTabButtonSelection();
    }
  }
}
void Screen::keyboardNewChar(const std::string &a2, bool_t a3) {
  for (auto &&e : this->elements) {
    if (e->suppressOtherGUI()) {
      e->keyboardNewChar(this->minecraft, a2, a3);
    }
  }
}
Screen::~Screen() {}
