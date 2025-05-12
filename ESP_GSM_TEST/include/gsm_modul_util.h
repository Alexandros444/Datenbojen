#ifndef GSM_MODUL_UTIL_H
#define GSM_MODUL_UTIL_H

#include <Arduino.h>

// #define SIM800L_IP5306_VERSION_20200811
#include "utilities.h"

#define TINY_GSM_MODEM_SIM800
#include <TinyGsmClient.h>

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial
// Set serial for AT commands (to the module)
#define SerialAT  Serial1
// See all AT commands, if wanted
#define DUMP_AT_COMMANDS

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon
// #define LOGGING  // <- Logging is for the HTTP library

// Baud rate for the GSM module
#define GSM_BAUD 115200

// Add a reception delay, if needed.
// This may be needed for a fast processor at a slow baud rate.
// #define TINY_GSM_YIELD() { delay(2); }

// flag to force SSL client authentication, if needed
// #define TINY_GSM_SSL_CLIENT_AUTHENTICATION

// Your GPRS credentials, if any
const char apn[]      = "";
const char gprsUser[] = "";
const char gprsPass[] = "";


struct statusInfo
{
    bool isNetworkConnected;
    bool isGprsConnected;
    int16_t batt;
    String signalQuality;
    String regStatus;
    String loc;
    String operatorName;
    String modemInfo;

};

void gsm_setup();
void perform_get_https(String url);
statusInfo getStatusInfo();



#endif