////////
// about
////////

// are you still here?
// a project by aaron montoya-moraga
// available at
// github.com/montoyamoraga/are-you-still-here

// this is the microcontroller Arduino flavor
// started in september 2020
// modified in october 2020
// v0.0.1

///////////////
// instructions
///////////////

// 0. install on your computer the Arduino IDE + libraries
// 1. check if your Arduino needs a firmware upgrade
// 2. modify in this script the user defined variables
// 3. upload this script to an Arduino (Nano 33 IoT) with internet connection

///////////
// comments
///////////

// 0. this currently works only on WAP networks, not WEP yet
// 1. programmed to use HTTPS protocol, not HTTP

///////////////////
// import libraries
///////////////////

#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoJson.h>

////////////////////////
// import external files
////////////////////////

// include file for internet login
#include "internet_login.h"

// read information from that file
// ssid is network name
char ssid[] = NETWORK_NAME;
// pass is network password (WPA)
char pass[] = NETWORK_PASS;

/////////////////////////
// user defined variables
/////////////////////////

String userOrganization = "National_Rifle_Association";
String userWikiPageId = "70101";

// messages of existence
String userExistPositive = "it is still here";
String userExistNegative = "it is no more";


//////////////////////
// setup initial state
//////////////////////

WiFiClient client;

int status = WL_IDLE_STATUS;
char server[] = "en.wikipedia.org";

String wikiExtract = "";

char wordsExistYes[][] = {" is ", " are "};
char wordsExistNo[][] = {" was ", " were "};

boolean wikiStillExists = true;

// setup() happens once at the beginning
void setup() {

  // initialize serial communication
  Serial.begin(9600);

  // wait for port to open
  while (!Serial);

  // check for the WiFi module
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  // attempt to connect to internet
  while (status != WL_CONNECTED) {
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection
    delay(10000);
  }
  Serial.println("Connected to wifi");

  // if you get a connection, report back via serial:
  if (client.connectSSL(server, 443)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("GET /w/api.php?format=json&action=query&prop=extracts&explaintext&exchars=128&titles=" + userOrganization + " HTTP/1.0");
    client.println("Host: en.wikipedia.org");
    client.println("Connection: close");
    client.println();
  }

  // skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    Serial.println(F("Invalid response"));
    return;
  }

  // allocate the json document
  // calculated from https://arduinojson.org/v6/assistant/
  const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) + 3 * JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + 292;
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
  wikiExtract = doc["query"]["pages"][userWikiPageId]["extract"].as<char*>();
  Serial.println(wikiExtract);

  for (int index; index < sizeof(wordsExistNo); index++) {
    Serial.println(String(sizeof(wordsExistNo)));
    //    Serial.println(wordsExistNo[index]);
  }


  //  for (int i = 0; i < wikiExtract.length() - wikiNoPlural.length(); i ++) {
  //    if (wikiExtract.substring(i, i + wikiYesSingular.length()).equals(wikiYesSingular) || wikiExtract.substring(i, i + wikiYesPlural.length()).equals(wikiYesPlural)) {
  //    }
  //
  //    if (wikiExtract.substring(i, i + wikiNoSingular.length()).equals(wikiNoSingular) || wikiExtract.substring(i, i + wikiNoPlural.length()).equals(wikiNoPlural)) {
  //      wikiStillExists = true;
  //    }
  //  }

  if (wikiStillExists) {
    Serial.println(userExistPositive);
  }
  else {
    Serial.println(userExistNegative);
  }
}

// loop() happens after setup(), and it repeats
void loop() {

  // check for availability of incoming bytes
  while (client.available()) {
    // read the byte
    char newChar = client.read();
    // print the byte
    Serial.write(newChar);
  }

  // if the server is disconnected
  if (!client.connected()) {

    // print a warning
    Serial.println("disconnecting from server.");

    // stop the client
    client.stop();

    // infinite loop to stop execution
    while (true);

  }
}
