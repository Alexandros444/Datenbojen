#include <Arduino.h>
#include "webserver_util.h"
#include "gsm_modul_util.h"
#include "gps_module.h"

void setup() {
    Serial.begin(115200);

    webserverSetup();

    gsm_setup();

    gps_setup();
}

void loop() {
    while (SerialAT.available()) {
        SerialMon.write(SerialAT.read());
    }
    while (SerialMon.available()) {
        SerialAT.write(SerialMon.read());
    }

    gps_loop();

    webserverLoop();
}