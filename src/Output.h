#ifndef SIGUES_AHI_OUTPUT_H
#define SIGUES_AHI_OUTPUT_H

// include Arduino library
#include <Arduino.h>

// forward declaration of class SiguesAhi
class SiguesAhi;

class Output {
  public:

  // virtual output destructor
  virtual ~Output() {}

  virtual void setupOutputScreen() {}
  virtual void screenDrawWelcome() {}

    // pointer to the SiguesAhi instance
    SiguesAhi *sigues = nullptr;
};

#endif
