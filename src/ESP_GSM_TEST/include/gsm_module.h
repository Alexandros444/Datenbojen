#ifndef GSM_MODULE_H
#define GSM_MODULE_H

#define TINY_GSM_MODEM_SIM800
// #define TINY_GSM_YIELD
#define TINY_GSM_YIELD_MS 10
#include <TinyGsmClient.h>
// #define DEBUG_MODEM
#ifdef DEBUG_MODEM
#include <StreamDebugger.h>
#endif

#include <ArduinoJson.h>

#include "gpio_module.h"
#include "sensors_module.h"

// Set serial for debug console (to the Serial Monitor, default speed 115200)
// #define SerialMon Serial
// Set serial for AT commands (to the module)
#define SerialAT  Serial1

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG Serial
// #define LOGGING  // <- Logging is for the HTTP library

// Baud rate for the GSM module
#define GSM_BAUD 115200

#define GSM_POWER_PIN PCF6
#define GSM_RST_PIN PCF5
#define GSM_DTR_PIN PCF7

#define MODEM_RX 16
#define MODEM_TX 17

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

enum sim_status{
    GSM_STATUS_OK,
    GSM_MODULE_ERROR,
    GSM_NETWORK_ERROR,
    GSM_AUTH_ERROR
};

void gsm_setup();
int perform_get_https(String url);
int perform_post_https(String url, String data);
statusInfo getStatusInfo();


class gsm_module {
private:
#ifdef DEBUG_MODEM
    StreamDebugger debugger; // Declare StreamDebugger
#endif
    TinyGsm modem;           // Declare TinyGsm
    bool set_gprs_bearer();
    bool http_close();
    int http_get_raw(String url);
    int http_post_raw(String url, String data);
    String getRegStatus(int regStatus);
    String getSignalQuality(int SignalQuality);
    const char* apn; // Most Sims work without configuration, search for "apn configuration <Your Provide Name>" if you need to set it manually
    const char* usr; // https://www.lte-anbieter.info/ratgeber/apn/uebersicht.php
    const char* pwd;
    bool init = false;
    bool debug_mode = false;
    sim_status last_status;
    void ask_debug_mode();
public:

    gsm_module()
#ifdef DEBUG_MODEM
    : debugger(SerialAT, Serial), // Initialize StreamDebugger
    modem(debugger),              // Initialize TinyGsm with StreamDebugger
#else
    : modem(SerialAT),            // Initialize TinyGsm with SerialAT
#endif
    apn("internet.telekom"),                   // Initialize APN
    usr("telekom"),
    pwd("tm")
    {
    };
    
    bool is_init() { return init; }
    bool is_debug_mode() { return debug_mode; }
    
    ~gsm_module() {};
    
    sim_status begin();
    int get_req_http(String url);
    int post_req_http(String url, String data);
    statusInfo getStatusInfo();
    void relay_serial();
    void loop();
    void turn_off();
    int sendData(sensors_module* sensors);
};

#endif