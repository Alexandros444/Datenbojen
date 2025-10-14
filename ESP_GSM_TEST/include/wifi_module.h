#ifndef WIFI_MODULE_H
#define WIFI_MODULE_H

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "sensors_module.h"
#include "gsm_module.h"

class wifi_module {
private:
    const char* ssid = "Can Cervera";
    const char* password = "637457896";
    String serverName = "http://jserv.ddns.net:8080/update-sensor";
    sensors_module *sensors;
    gsm_module *gsm;
public:
    wifi_module(){};
    ~wifi_module(){};
    void begin(sensors_module* sensors, gsm_module* gsm);
    int sendData();
    void turn_off(){ WiFi.disconnect(true); WiFi.mode(WIFI_OFF); }
};


#endif