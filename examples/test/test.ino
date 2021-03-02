// include library
#include <SiguesAhi.h>

// include file for internet login
#include "secret.h"

// instance of class
SiguesAhi sigues;

// variables to be set by user
// 16779: KKK, 70101: NRA
int myPageID = 70101;

void setup() {

  // initialize serial and wait for port to open
  // argument are NETWORK_NAME and NETWORK_PASS, from secret.h
  // and page ID, provided by variable
  sigues.initialize(NETWORK_NAME, NETWORK_PASS, myPageID);

  // TODO: document
  sigues.connectingSSL();

  sigues.checkStatus();

  sigues.skipHeaders();

  sigues.parseJSON();

  if (sigues.wikiStillExists) {
    Serial.println("oh no it still exists");
  } else {
    Serial.println("oh great it does not exist anymore");
  }
}

void loop() {

  // if there are incoming bytes available
  // from the server, read them and print them:

  sigues.isClientAvailable();

  sigues.isClientConnected();
}
