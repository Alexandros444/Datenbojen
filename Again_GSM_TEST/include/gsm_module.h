#ifndef GSM_MODULE_H
#define GSM_MODULE_H

#define TINY_GSM_MODEM_SIM800
// #define TINY_GSM_YIELD
#define TINY_GSM_YIELD_MS 10
#include <TinyGsmClient.h>
#include <StreamDebugger.h>

// Set serial for debug console (to the Serial Monitor, default speed 115200)
// #define SerialMon Serial
// Set serial for AT commands (to the module)
#define SerialAT  Serial1

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG Serial
// #define LOGGING  // <- Logging is for the HTTP library

// Baud rate for the GSM module
#define GSM_BAUD 9600


#define MODEM_RX 17
#define MODEM_TX 16

struct statusInfo
{
    bool isNetworkConnected;
    bool isGprsConnected;
    String batt;
    String signalQuality;
    String regStatus;
    String loc;
    String operatorName;
    String modemInfo;
    String networkTime;
};

void gsm_setup();
int perform_get_https(String url);
int perform_post_https(String url, String data);
statusInfo getStatusInfo();


class gsm_module {
private:
    StreamDebugger debugger; // Declare StreamDebugger
    TinyGsm modem;           // Declare TinyGsm
    bool set_gprs_bearer();
    bool http_close();
    int http_get_raw(String url);
    int http_post_raw(String url, String data);
    String getRegStatus(int regStatus);
    String getSignalQuality(int SignalQuality);
    const char* apn; // Most Sims work without configuration, search for "apn configuration <Your Provide Name>" if you need to set it manually
    const char* gprsUser; // https://www.lte-anbieter.info/ratgeber/apn/uebersicht.php
    const char* gprsPass;
    bool init = false;
public:
    gsm_module()
        : debugger(SerialAT, Serial), // Initialize StreamDebugger
        modem(debugger),              // Initialize TinyGsm with StreamDebugger
        apn(""),                   // Initialize APN
        gprsUser(""),
        gprsPass("")
    {
    };

    bool is_init() { return init; }

    ~gsm_module() {};

    void begin();
    int get_req_http(String url);
    int post_req_http(String url, String data);
    statusInfo getStatusInfo();

};

#endif