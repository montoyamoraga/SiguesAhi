#include "SiguesAhiOutputScreen.h"

OutputScreen::OutputScreen() {}

OutputScreen::~OutputScreen() {
  if (_screen != nullptr) {
    delete _screen;
    _screen = nullptr;
  }
}
