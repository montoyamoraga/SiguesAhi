// include library
#include <SiguesAhi.h>

// include file for internet login
// #include "secret.h"

// or put your info here
#ifndef NETWORK_NAME
#define NETWORK_NAME "replaceHere"
#endif

#ifndef NETWORK_PASS
#define NETWORK_PASS "replaceHere"
#endif

// instance of class
SiguesAhi sigues(ES, OUTPUT_SCREEN);

// variables to be set by user
String wikiPageTitle = "National_Rifle_Association";
// 70101: NRA
int wikiPageID = 70101;
// true for printing in console, false for
bool debugging = true;

void setup() {

  sigues.setDebugging(debugging);

  // initialize serial and wait for port to open
  // argument are NETWORK_NAME and NETWORK_PASS, from secret.h
  sigues.setNetwork(NETWORK_NAME, NETWORK_PASS);

  // set wikipedia
  sigues.setWiki(wikiPageTitle, wikiPageID);

  sigues.checkExistence();

  sigues.printExistence();

  }

void loop() {

  // if there are incoming bytes available
  // from the server, read them and print them:

  sigues.isClientAvailable();

  sigues.isClientConnected();
}
