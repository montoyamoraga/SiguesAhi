#ifndef SIGUES_AHI_OUTPUT_SCREEN
#define SIGUES_AHI_OUTPUT_SCREEN

// include library Output
#include "SiguesAhiOutput.h"

// include library SiguesAhi
#include "SiguesAhi.h"

// include library SPI for screen
#include <SPI.h>

// include library Wire for screen
#include <Wire.h>

// include library Adafruit_GFX for screen
#include <Adafruit_GFX.h>

// include library Adafruit_SSD1306 for screen
#include <Adafruit_SSD1306.h>

class OutputScreen : public SiguesAhiOutput {
  public:
    // constructor
    OutputScreen();

    // destructor
    ~OutputScreen();

    // instance of Adafruit_SSD1306
    Adafruit_SSD1306 *_screen = nullptr;
};

#endif
