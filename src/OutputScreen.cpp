#include "OutputScreen.h"

OutputScreen::OutputScreen() {}

OutputScreen::~OutputScreen() {
  if (_screen != nullptr) {
    delete _screen;
    _screen = nullptr;
  }
}

void OutputScreen::setupOutputScreen() {
    if (!_screen) {
    _screen =
        new Adafruit_SSD1306(_screenWidth, _screenHeight, &Wire, _oledReset);
  }
  if (!_screen->begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  _screen->clearDisplay();
  _screen->display();
  delay(2000);
}

void OutputScreen::screenDrawWelcome() {
  _screen->clearDisplay();
  _screen->setTextSize(1);
  _screen->setTextColor(SSD1306_WHITE);
  _screen->setCursor(0, 0);
  _screen->println(F("SiguesAhi"));
  _screen->display();
  delay(2000);
}
