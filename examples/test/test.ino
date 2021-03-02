#include <SiguesAhi.h>

// include file for internet login
#include "secret.h"

// instance of class
SiguesAhi sigues;

// read information from that file
char ssid[] = NETWORK_NAME;
// pass is network password (WPA)
char pass[] = NETWORK_PASS;

int status = WL_IDLE_STATUS;

void setup() {
  
  //Initialize serial and wait for port to open:
  sigues.initialize();

  // TODO: add explanation of example
  sigues.setPageID(70101);

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to wifi");

  //  "https://en.wikipedia.org/w/api.php?format=json&action=query&prop=extracts&exintro&explaintext&redirects=1&titles=National_Rifle_Association&exchars=128"

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:

  sigues.connectingSSL();

  sigues.checkStatus();

  sigues.skipHeaders();

  sigues.parseJSON();

  if (sigues.wikiStillExists) {
    Serial.println("oh no it still exists");
  }
  else {
    Serial.println("oh great it does not exist anymore");
  }
}

void loop() {

  // if there are incoming bytes available
  // from the server, read them and print them:

  sigues.isClientAvailable();

  sigues.isClientConnected();

}
