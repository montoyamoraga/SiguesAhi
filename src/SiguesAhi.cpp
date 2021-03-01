// include local library
#include "SiguesAhi.h"

SiguesAhi::SiguesAhi() {
  wikiPageID = 7001;
}

void SiguesAhi::setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("hey what up");
}
