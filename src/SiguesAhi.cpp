// include local library
#include "SiguesAhi.h"

SiguesAhi::SiguesAhi() {
  // TODO: add here
}

void SiguesAhi::initialize() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("hey what up");
}

void SiguesAhi::setPageID(int newID) {
  wikiPageID = String(newID);
}

String SiguesAhi::getPageID() {
  return wikiPageID;
}
