#ifndef SIGUES_AHI_OUTPUT_SCREEN
#define SIGUES_AHI_OUTPUT_SCREEN

// include library Arduino
#include <Arduino.h>

// include library Output
#include "Output.h"

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

// constants for OLED screen
#define OLED_RESET 4
#define SCREEN_ADDRESS 0x3C

class OutputScreen : public Output {
  public:
    // constructor
    OutputScreen();

    // destructor
    ~OutputScreen();

    virtual void setupOutputScreen() override;
    virtual void screenDrawWelcome() override;

    int _screenWidth = 128;
    int _screenHeight = 32;
    int _oledReset = 4;

    // instance of Adafruit_SSD1306
    Adafruit_SSD1306 *_screen = nullptr;
};

#endif
