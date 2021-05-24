#ifndef SIGUES_AHI_OUTPUT_H
#define SIGUES_AHI_OUTPUT_H

// include Arduino library
#include <Arduino.h>

// forward declaration of class SiguesAhi
class SiguesAhi;

class SiguesAhiOutput {
  public:

  // virtual output destructor
  virtual ~SiguesAhiOutput();

    // pointer to the SiguesAhi instance
    SiguesAhi *sigues = nullptr;
};

#endif
