// SiguesAhi.h

// a project by aaron montoya-moraga
// github.com/montoyamoraga/SiguesAhi
// started in september 2020

// conditional compilation
#ifndef SIGUES_AHI_H
#define SIGUES_AHI_H

/// @include library Arduino
#include <Arduino.h>

/// @include libraries for commmunication
#include <SPI.h>
#include <WiFiNINA.h>

/// @include libraries for parsing data
#include <Arduino_JSON.h>

/// @include libraries for inputting data
#include <RotaryEncoder.h>

/// @include library Output
#include "Output.h"

/// @enum to set the output type
enum OutputType {
  OUTPUT_PRINTER = 0,
  OUTPUT_SCREEN = 1,
  OUTPUT_SERIAL = 2
};

class SiguesAhi {
public:
  /// @brief constructor
  SiguesAhi(OutputType outputType);

  // @brief destructor
  ~SiguesAhi();

  // pointer to output
  Output *myOutput = nullptr;

  void setupOutputScreen();
  void screenDrawWelcome();

  // other functions
  void setNetwork(String newNetworkName, String newNetworkPass);

  void setWiki(String newWikiPageTitle, int newWikiPageID);

  void setWikiPageTitle(String newWikiPageTitle);
  String getWikiPageTitle();

  void setWikiPageID(int newWikiPageID);
  String getWikiPageID();

  void updateWikiRequest();

  void setDebugging(bool newState);

  void checkExistence();

  void connectSSL();

  void checkStatus();

  void skipHeaders();

  void parseJSON();

  void isClientAvailable();

  void isClientConnected();

  void checkWifiModule();

  void checkFirmware();

  void connectInternet();

  void printExistence();

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
  bool debuggingMode = true;
  
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
