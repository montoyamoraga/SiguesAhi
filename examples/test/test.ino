#include <SiguesAhi.h>

// include file for internet login
#include "secret.h"

// instance of class
SiguesAhi sigues;

// read information from that file
// ssid is network name
char ssid[] = NETWORK_NAME;
// pass is network password (WPA)
char pass[] = NETWORK_PASS;

int status = WL_IDLE_STATUS;
char server[] = "en.wikipedia.org";

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;

String wikiPageId = "70101";
String wikiExtract = "";

String wikiYesSingular = " is ";
String wikiYesPlural = " are ";
String wikiNoSingular = " was ";
String wikiNoPlural = " were ";

boolean wikiStillExists = true;

void setup() {
  
  //Initialize serial and wait for port to open:
  serial.setup();

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
    Serial.print("Attempting to connect to SSID: ");
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
  if (client.connectSSL(server, 443)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("GET /w/api.php?format=json&action=query&prop=extracts&explaintext&titles=National_Rifle_Association&exchars=128 HTTP/1.0");
    client.println("Host: en.wikipedia.org");
    client.println("Connection: close");
    client.println();
  }

  //  check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
  if (strcmp(status + 9, "200 OK") != 0) {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    return;
  }

  // skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    Serial.println(F("Invalid response"));
    return;
  }

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
  wikiExtract = doc["query"]["pages"][wikiPageId]["extract"].as<char*>();
  Serial.println(wikiExtract);

  for (int i = 0; i < wikiExtract.length() - wikiNoPlural.length(); i ++) {
    if (wikiExtract.substring(i, i + wikiYesSingular.length()).equals(wikiYesSingular) || wikiExtract.substring(i, i + wikiYesPlural.length()).equals(wikiYesPlural)) {
    }

    if (wikiExtract.substring(i, i + wikiNoSingular.length()).equals(wikiNoSingular) || wikiExtract.substring(i, i + wikiNoPlural.length()).equals(wikiNoPlural)) {
      wikiStillExists = true;
    }
  }

  if (wikiStillExists) {
    Serial.println("damn it still exists");
  }
  else {
    Serial.println("oh great it does not exist anymore");
  }
}

void loop() {
  // if there are incoming bytes available
  // from the server, read them and print them:
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting from server.");
    client.stop();

    // do nothing forevermore:
    while (true);
  }
}
