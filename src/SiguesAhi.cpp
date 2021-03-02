// include local library
#include "SiguesAhi.h"

char server[] = "en.wikipedia.org";

SiguesAhi::SiguesAhi() {
  // TODO: add here
}

void SiguesAhi::initialize() {
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("hey what up");
}

void SiguesAhi::setPageID(int newID) { wikiPageID = String(newID); }

String SiguesAhi::getPageID() { return wikiPageID; }

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
