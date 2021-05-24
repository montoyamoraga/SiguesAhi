// SiguesAhi.cpp

// include local library
#include "SiguesAhi.h"

char server[] = "en.wikipedia.org";

// TODO: maybe this can be deleted?
int status = WL_IDLE_STATUS;

SiguesAhi::SiguesAhi() {}

void SiguesAhi::setNetwork(String newNetworkName, String newNetworkPass) {

  ssid = newNetworkName;
  pass = newNetworkPass;

  // hardware check
  checkWifiModule();

  // software check
  checkFirmware();

  // connect
  connectInternet();
}

void SiguesAhi::setWiki(String newWikiPageTitle, int newWikiPageID) {
  setWikiPageTitle(newWikiPageTitle);
  setWikiPageID(newWikiPageID);
}

void SiguesAhi::setWikiPageID(int newWikiPageID) {
  // set internal variable
  wikiPageID = String(newWikiPageID);
}

String SiguesAhi::getWikiPageID() {
  // return String
  return wikiPageID;
}

void SiguesAhi::setDebugging(bool newState) {
  // set debuggingMode
  debuggingMode = newState;
  if (debuggingMode) {
    // open serial port
    Serial.begin(9600);
    while (!Serial)
      ;
  }
}

String SiguesAhi::getWikiPageTitle() {
  // return String
  return wikiPageTitle;
}

void SiguesAhi::setWikiPageTitle(String newWikiPageTitle) {
  wikiPageTitle = newWikiPageTitle;
  updateWikiRequest();
}

void SiguesAhi::updateWikiRequest() {

  String wikiRequestPrefix = "GET "
                             "/w/"
                             "api.php?format=json&action=query&prop=extracts&"
                             "explaintext&exchars=128&titles=";
  String wikiRequestSuffix = " HTTP/1.0";

  wikiRequest = wikiRequestPrefix + wikiPageTitle + wikiRequestSuffix;
}

void SiguesAhi::connectingSSL() {

  // connect through port 443
  if (client.connectSSL(server, 443)) {

    Serial.println("connected to server");

    client.println(wikiRequest);
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
  // const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 500;
  // DynamicJsonDocument doc(capacity);

  // Parse JSON object
  // DeserializationError error = deserializeJson(doc, client);

  // if (error) {
  //   Serial.print(F("deserializeJson() failed: "));
  //   Serial.println(error.c_str());
  //   return;
  // }

  JSONVar myArray = JSON.parse(client);

  // extract values

  Serial.println(F("Response:"));

  // wikiExtract = doc["query"]["pages"][getWikiPageID()]["extract"].as<char
  // *>(); Serial.println(wikiExtract);

  // for (int i = 0; i < wikiExtract.length() - wikiNoPlural.length(); i++) {
  //   if (wikiExtract.substring(i, i + wikiYesSingular.length())
  //           .equals(wikiYesSingular) ||
  //       wikiExtract.substring(i, i + wikiYesPlural.length())
  //           .equals(wikiYesPlural)) {
  //   }

  //   if (wikiExtract.substring(i, i + wikiNoSingular.length())
  //           .equals(wikiNoSingular) ||
  //       wikiExtract.substring(i, i + wikiNoPlural.length())
  //           .equals(wikiNoPlural)) {
  //     wikiStillExists = true;
  //   }
  // }
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
    Serial.println("disconnecting from server.");
    client.stop();

    // do nothing forever more:
    while (true)
      ;
  }
}

void SiguesAhi::checkWifiModule() {
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("communication with wifi module failed!");
    // don't continue
    while (true)
      ;
  }
}

void SiguesAhi::checkFirmware() {
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("optional: upgrade firmware with WiFiNINA library");
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
    ssid.toCharArray(ssidCharArray, ssid.length() + 1);
    pass.toCharArray(passCharArray, pass.length() + 1);

    // Connect to WPA/WPA2. Modify if using open or WEP
    status = WiFi.begin(ssidCharArray, passCharArray);

    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("connected to wifi");
}

void SiguesAhi::printExistence() {
  if (wikiStillExists) {
    Serial.println("oh no it still exists");
  } else {
    Serial.println("oh great it does not exist anymore");
  }
}
