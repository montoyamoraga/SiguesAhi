// include library
#include <SiguesAhi.h>

// include file for internet login
#include "secret.h"

// instance of class
SiguesAhi sigues;

// variables to be set by user
// 16779: KKK
// 70101: NRA
int myPageID = 70101;

// read information from that file
char ssid[] = NETWORK_NAME;
// pass is network password (WPA)
char pass[] = NETWORK_PASS;

void setup() {

  // initialize serial and wait for port to open
  // argument is page ID
  sigues.initialize(myPageID);

  //  "https://en.wikipedia.org/w/api.php?format=json&action=query&prop=extracts&exintro&explaintext&redirects=1&titles=National_Rifle_Association&exchars=128"

  Serial.println("Starting connection to server");

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
