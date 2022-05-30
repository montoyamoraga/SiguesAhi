#ifndef SIGUES_AHI_OUTPUT_SERIAL
#define SIGUES_AHI_OUTPUT_SERIAL

// include Arduino library
#include <Arduino.h>

// include library Output
#include "Output.h"

// include library SiguesAhi
#include "SiguesAhi.h"

class OutputSerial : public Output {
  public:
    // constructor
    OutputSerial();
    // destructor
   ~OutputSerial();
   private:
    // 
    int _serialBaudRate = 9600;
};

#endif
