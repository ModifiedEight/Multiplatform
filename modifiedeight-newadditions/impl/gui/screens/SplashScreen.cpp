#include <Minecraft.hpp>
#include <gui/screens/SplashScreen.hpp>
#include <math.h>
#include <rendering/Font.hpp>
#include <unigl.h>

SplashScreen::SplashScreen() : Screen() {
  this->timer = 0.0f;
  this->duration = 1.5f;
  this->fadeDuration = 0.0f;
}

SplashScreen::~SplashScreen() {}

bool_t SplashScreen::isPauseScreen() { return 1; }

bool_t SplashScreen::renderGameBehind() { return 0; }

bool_t SplashScreen::handleBackEvent(bool_t) { return 0; }

void SplashScreen::tick() {
  this->timer += 0.05f;
  if (this->timer >= this->duration) {
    this->minecraft->screenChooser.setScreen(START_MENU_SCREEN);
  }
}

void SplashScreen::render(int32_t mx, int32_t my, float delta) {
  this->fill(0, 0, this->width, this->height, 0xFFFFFFFF);

  Font *font = this->minecraft->font;
  if (font) {
    std::string title = "ModifiedEight";
    float scale = 2.0f;
    float textWidth = (float)font->width(title) * scale;
    float textHeight = 8.0f * scale;
    float x = ((float)this->width - textWidth) * 0.5f;
    float y = ((float)this->height - textHeight) * 0.5f;

    glPushMatrix();
    glTranslatef(floorf(x), floorf(y), 0.0f);
    glScalef(scale, scale, 1.0f);
    font->draw(title, 0.0f, 0.0f, 0x000000);
    glPopMatrix();
  }
}
