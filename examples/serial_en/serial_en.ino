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
SiguesAhi sigues(OUTPUT_SERIAL);

// variables to be set by user
String myWikiPageTitle = "National_Rifle_Association";
// 70101: NRA
int myWikiPageID = 70101;
// true for printing in console, false for
bool myDebugging = true;

void setup() {

  sigues.setDebugging(myDebugging);

  // initialize serial and wait for port to open
  // argument are NETWORK_NAME and NETWORK_PASS, from secret.h
  sigues.setNetwork(NETWORK_NAME, NETWORK_PASS);

  sigues.setWiki(myWikiPageTitle, myWikiPageID);

  // TODO: document
  sigues.connectSSL();

  sigues.checkStatus();

  sigues.skipHeaders();

  sigues.parseJSON();

  sigues.printExistence();
}

void loop() {

  // if there are incoming bytes available
  // from the server, read them and print them:

  sigues.isClientAvailable();

  sigues.isClientConnected();
}
