#ifndef SIGUES_AHI_OUTPUT_BUZZER
#define SIGUES_AHI_OUTPUT_BUZZER

// include Arduino library
#include <Arduino.h>

// include library Output
#include "Output.h"

// include library SiguesAhi
#include "SiguesAhi.h"

class OutputBuzzer : public Output {
  public:
    // constructor
    OutputBuzzer();
    // destructor
   ~OutputBuzzer();
   private:
    int _placeholder = 9600;
};

#endif
