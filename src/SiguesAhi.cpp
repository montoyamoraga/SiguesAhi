// SiguesAhi.cpp

// include local library
#include "SiguesAhi.h"

char server[] = "en.wikipedia.org";

// TODO: maybe this can be deleted?
int status = WL_IDLE_STATUS;

SiguesAhi::SiguesAhi() {}

void SiguesAhi::initialize(String newNetworkName, String newNetworkPass, int newPageID) {
  // open serial port
  Serial.begin(9600);
  while (!Serial)
    ;
  // set new page ID

  setPageID(newPageID);

  ssid = newNetworkName;
  pass = newNetworkPass;

  // hardware check
  checkWifiModule();

  // software check
  checkFirmware();

  // connect
  connectInternet();
}

void SiguesAhi::setPageID(int newPageID) {
  // set internal variable
  wikiPageID = String(newPageID);
}

String SiguesAhi::getPageID() {
  // return String
  return wikiPageID; 
}

void SiguesAhi::setDebuggingMode(bool newState) {
  // set debuggingMode
  debuggingMode = newState;
}

void SiguesAhi::connectingSSL() {
  if (client.connectSSL(server, 443)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("GET "
                   "/w/"
                   "api.php?format=json&action=query&prop=extracts&explaintext&"
                   "titles=National_Rifle_Association&exchars=128 HTTP/1.0");
    client.println("Host: en.wikipedia.org");
    client.println("Connection: close");
    client.println();
  }
}

void SiguesAhi::checkStatus() {
  //  check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
  if (strcmp(status + 9, "200 OK") != 0) {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    return;
  }
}

void SiguesAhi::skipHeaders() {
  // skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    Serial.println(F("Invalid response"));
    return;
  }
}

void SiguesAhi::parseJSON() {
  // allocate the json document
  const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 500;
  DynamicJsonDocument doc(capacity);

  // Parse JSON object
  DeserializationError error = deserializeJson(doc, client);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  // extract values
  Serial.println(F("Response:"));
  wikiExtract = doc["query"]["pages"][getPageID()]["extract"].as<char *>();
  Serial.println(wikiExtract);

  for (int i = 0; i < wikiExtract.length() - wikiNoPlural.length(); i++) {
    if (wikiExtract.substring(i, i + wikiYesSingular.length())
            .equals(wikiYesSingular) ||
        wikiExtract.substring(i, i + wikiYesPlural.length())
            .equals(wikiYesPlural)) {
    }

    if (wikiExtract.substring(i, i + wikiNoSingular.length())
            .equals(wikiNoSingular) ||
        wikiExtract.substring(i, i + wikiNoPlural.length())
            .equals(wikiNoPlural)) {
      wikiStillExists = true;
    }
  }
}

void SiguesAhi::isClientAvailable() {
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }
}

void SiguesAhi::isClientConnected() {
  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting from server.");
    client.stop();

    // do nothing forever more:
    while (true)
      ;
  }
}

void SiguesAhi::checkWifiModule() {
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  }
}

void SiguesAhi::checkFirmware() {
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }
}

void SiguesAhi::connectInternet() {
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("trying to connect to: ");
    Serial.println(ssid);
    
    // conversion from string to char array
    // first initialize the arrays with the length
    char ssidCharArray[ssid.length() + 1];
    char passCharArray[pass.length() + 1];

    // then use function to populate them
    ssid.toCharArray(ssidCharArray, ssid.length()+1);
    pass.toCharArray(passCharArray, pass.length()+1);

    // Connect to WPA/WPA2. Modify if using open or WEP
    status = WiFi.begin(ssidCharArray, passCharArray);

    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to wifi");
}
