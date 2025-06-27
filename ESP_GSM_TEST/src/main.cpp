#include <Arduino.h>
#include "webserver_util.h"
#include "gsm_modul_util.h"
#include "gps_module.h"
#include "tft_module.h"
#include "adc_module.h"

adc_module adc;

void setup() {
    Serial.begin(115200);

    delay(1000); // Wait for Serial Monitor to open
    
    Serial.println("Starting...");
    
    delay(1000); // Wait for Serial Monitor to open

    webserverSetup();

    gsm_setup();

    gps_setup();

    tft_setup();

    adc.begin();
}

void loop() {
    while (SerialAT.available()) {
        Serial.write(SerialAT.read());
    }
    while (Serial.available()) {
        SerialAT.write(Serial.read());
    }

    gps_loop();

    tft_loop();

    webserverLoop();

    if (millis() % 10000 == 0) {
        adc.print_data();
    }
}