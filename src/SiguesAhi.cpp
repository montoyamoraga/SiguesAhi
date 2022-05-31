// SiguesAhi.cpp

/// @include local library
#include "SiguesAhi.h"

/// @include library for each output
#include "OutputPrinter.h"
#include "OutputScreen.h"
#include "OutputSerial.h"

char server[] = "en.wikipedia.org";

int status = WL_IDLE_STATUS;

SiguesAhi::SiguesAhi(Language language, OutputType outputType) {
  
  if (language == EN) {
    strcpy (server,"en.wikipedia.org");

    wikiYes[0] = "is";
    wikiYes[1] = "are";

    wikiNo[0] = "was";
    wikiNo[1] = "were";
  }
  else if (language == ES) {
    strcpy (server,"es.wikipedia.org");

    wikiYes[0] = "es";
    wikiYes[1] = "son";

    wikiNo[0] = "fue";
    wikiNo[1] = "fueron";
  }
  
  if (outputType == OUTPUT_SCREEN) {
    myOutput = new OutputScreen();
  }
  else if (outputType == OUTPUT_PRINTER) {
    myOutput = new OutputPrinter();
  }
  else if (outputType == OUTPUT_SERIAL) {
    myOutput = new OutputSerial();
  }
  else {
    myOutput = new Output();
  }

   // update pointer to output
  if (myOutput != nullptr) {
    myOutput->sigues = this;
  }
}

// destructor
SiguesAhi::~SiguesAhi() {
  if (myOutput != nullptr) {
    delete myOutput;
    myOutput = nullptr;
  }
}

void SiguesAhi::setupOutputScreen() {
  if (myOutput != nullptr) {
    myOutput->setupOutputScreen();
  }
}

void SiguesAhi::screenDrawWelcome() {
  if (myOutput != nullptr) {
    myOutput->screenDrawWelcome();
  }
}

void SiguesAhi::setNetwork(String newNetworkName, String newNetworkPass) {

  ssid = newNetworkName;
  pass = newNetworkPass;

  // hardware check
  checkWifiModule();

  // software check
  // checkFirmware();

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

void SiguesAhi::checkExistence() {
  SiguesAhi::connectSSL();
  SiguesAhi::checkStatus();
  SiguesAhi::skipHeaders();
  SiguesAhi::parseJSON();
}

void SiguesAhi::connectSSL() {

  // connect through port 443
  if (client.connectSSL(server, 443)) {
    
    if (debuggingMode) {
      Serial.println("connected to server");
    }

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

// this is a draft made with
// library ArduinoJSON
// needs to be updated
void SiguesAhi::parseJSON() {

  while (client.available()) {
    char c = client.read();
    // Serial.write(c);
    clientInput = clientInput + c;
  }

  Serial.println(clientInput);

  JSONVar inputParsedJSON = JSON.parse(clientInput);

  // retrieve info from query / pages / pageID / extract
  // and convert from JSON to String
  String inputExtract = JSON.stringify(inputParsedJSON["query"]["pages"][wikiPageID]["extract"]);

  if (debuggingMode) {
    Serial.println(inputExtract);
  }

  // TODO: check YES and check NO, adapt to 2d arrays of char
  for (int i = 0; i < wikiExtract.length() - sizeof(SiguesAhi::wikiYes[0]); i++) {
    if (wikiExtract.substring(i, i + sizeof(SiguesAhi::wikiYes[0])).equals(SiguesAhi::wikiYes[0]) || 
    wikiExtract.substring(i, i + sizeof(SiguesAhi::wikiYes[0])).equals(SiguesAhi::wikiYes[0])) {
      wikiStillExists = true;
    }
  }

  // check NO
  // for (int i = 0; i < wikiExtract.length() - wikiNoPlural.length(); i++) {

  //   if (wikiExtract.substring(i, i + wikiYesSingular.length())
  //           .equals(wikiYesSingular) ||
  //       wikiExtract.substring(i, i + wikiYesPlural.length())
  //           .equals(wikiYesPlural)) {
    // }

}

void SiguesAhi::isClientAvailable() {

  // while (client.available()) {
  //   char c = client.read();
  //   // Serial.write(c);
  //   clientInput = clientInput + c;
  // }

  // Serial.println(clientInput);
}

void SiguesAhi::isClientConnected() {
  // if the server is disconnected, stop the client
  if (!client.connected()) {
    Serial.println("disconnecting from server.");
    client.stop();

    // stay stuck forver
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
  // try to connect to wifi network:
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
  if (debuggingMode) {
    Serial.println("connected to wifi");
  }
}

void SiguesAhi::printExistence() {
  if (wikiStillExists) {
    Serial.println("oh no it still exists");
  } else {
    Serial.println("oh great it does not exist anymore");
  }
}
