#include <Arduino.h>

#include "config.h"

#include "webserver_util.h"
#include "gsm_modul_util.h"
#include "gps_module.h"
#include "tft_module.h"
#include "adc_module.h"
#include "sensors_module.h"


adc_module adc;
sensors_module sensors;
tft_module tft;


void setup() {
    Serial.begin(115200);

    delay(1000); // Wait for Serial Monitor to open
    
    Serial.println("Starting...");
    
    delay(1000); // Wait for Serial Monitor to open

    // webserverSetup();

    // gsm_setup();

    // gps_setup();

    
    adc.begin();
    
    sensors.begin(&adc); // Initialize sensors with the ADC module

    tft.begin(); // Initialize TFT display

}

void loop() {
    // while (SerialAT.available()) {
    //     Serial.write(SerialAT.read());
    // }
    // while (Serial.available()) {
    //     SerialAT.write(Serial.read());
    // }

    // gps_loop();

    // tft_loop();

    // tft.dot_test();
    
    
    // webserverLoop();
    
    if (millis() % 10000 == 0) {
        adc.print_data();
        sensors.print();
        
        tft.print_sensors(&adc, &sensors); // Print sensor values on the TFT display

    }

}