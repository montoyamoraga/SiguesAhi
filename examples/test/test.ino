// include library
#include <SiguesAhi.h>

// include file for internet login
#include "secret.h"

// instance of class
SiguesAhi sigues;

// variables to be set by user
String myWikiPageTitle = "National_Rifle_Association";
// 70101: NRA
int myWikiPageID = 70101;

void setup() {

  // initialize serial and wait for port to open
  // argument are NETWORK_NAME and NETWORK_PASS, from secret.h
  sigues.setNetwork(NETWORK_NAME, NETWORK_PASS);

  sigues.setWiki(myWikiPageTitle, myWikiPageID);

  // TODO: document
  sigues.connectingSSL();

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
