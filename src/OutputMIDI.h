#ifndef SIGUES_AHI_OUTPUT_MIDI
#define SIGUES_AHI_OUTPUT_MIDI

// include Arduino library
#include <Arduino.h>

// include library Output
#include "Output.h"

// include library SiguesAhi
#include "SiguesAhi.h"


class OutputMIDI : public Output {
  public:
    // constructor
    OutputMIDI();
    // destructor
   ~OutputMIDI();
   private:
    int _midiBaudRate = 9600;
};

#endif
