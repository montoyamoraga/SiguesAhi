// sigues-ahi

// a project by aaron montoya-moraga
// github.com/montoyamoraga/sigues-ahi

// started in september 2020
// alpha state
// it is working but it is messy

// conditional compilation
#ifndef SIGUES_AHI_H
#define SIGUES_AHI_H

// include libraries
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Arduino_JSON.h>
#include <SPI.h>
#include <WiFiNINA.h>

class SiguesAhi {
public:
  // constructor
  SiguesAhi();

  // other functions
  void initialize();

  void setPageID(int newID);
  String getPageID();

  void connectingSSL();

  void checkStatus();

  void skipHeaders();

  void parseJSON();

  void isClientAvailable();

  void isClientConnected();

  // TODO: add API so that this variable can be protected
  boolean wikiStillExists = true;

  // Initialize the Ethernet client library
  // with the IP address and port of the server
  // that you want to connect to (port 80 is default for HTTP):
  WiFiClient client;

  // char server[] = "en.wikipedia.org";
  // String server = "en.wikipedia.org";

  String wikiExtract = "";

  String wikiYesSingular = " is ";
  String wikiYesPlural = " are ";
  String wikiNoSingular = " was ";
  String wikiNoPlural = " were ";

protected:
  String wikiPageID;
};

// conditional compilation
#endif
