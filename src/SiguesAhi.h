// SiguesAhi.h

// a project by aaron montoya-moraga
// github.com/montoyamoraga/SiguesAhi
// started in september 2020

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
  void setNetwork(String newNetworkName, String newNetworkPass);

  void setWiki(String newWikiPageTitle, int newWikiPageID);

  void setWikiPageTitle(String newWikiPageTitle);
  String getWikiPageTitle();

  void setWikiPageID(int newWikiPageID);
  String getWikiPageID();

  void setWikiRequest(String newWikiTitle);

  void setDebuggingMode(bool newState);

  void connectingSSL();

  void checkStatus();

  void skipHeaders();

  void parseJSON();

  void isClientAvailable();

  void isClientConnected();

  void checkWifiModule();

  void checkFirmware();

  void connectInternet();

  // TODO: add API so that this variable can be protected
  boolean wikiStillExists = true;

  // Initialize the Ethernet client library
  // with the IP address and port of the server
  // that you want to connect to (port 80 is default for HTTP):
  WiFiClient client;

  String wikiExtract = "";

  String wikiYesSingular = " is ";
  String wikiYesPlural = " are ";
  String wikiNoSingular = " was ";
  String wikiNoPlural = " were ";

protected:
  // debugging variables
  bool debuggingMode = false;
  // network variables
  String ssid;
  String pass;
  // wikipedia variables
  String wikiPageTitle;
  String wikiPageID;
  String wikiRequest;
};

// conditional compilation
#endif
