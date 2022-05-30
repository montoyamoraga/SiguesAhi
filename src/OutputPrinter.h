#ifndef SIGUES_AHI_OUTPUT_PRINTER
#define SIGUES_AHI_OUTPUT_PRINTER

// include Arduino library
#include <Arduino.h>

// include library Output
#include "Output.h"

// include library SiguesAhi
#include "SiguesAhi.h"

// include library for output with Adafruit thermal printer
#include <Adafruit_Thermal.h>

class OutputPrinter : public Output {
  public:
    // constructor
    OutputPrinter();
    // destructor
   ~OutputPrinter();
   private:
    // instance of Adafruit Thermal Printer, connected to Serial1
    Adafruit_Thermal *_printer = nullptr;
    int _printerBaudRate = 9600;
};

#endif
