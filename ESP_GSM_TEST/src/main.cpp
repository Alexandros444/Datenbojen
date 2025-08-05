#include <Arduino.h>

#include "config.h"

#include "webserver_util.h"
#include "gps_module.h"
#include "tft_sd_module.h"
#include "adc_module.h"
#include "sensors_module.h"

adc_module adc;
sensors_module sensors;
tft_module tft;
sd_module sd;
gsm_module gsm; // Create a GSM module instance

int disp_pages = 3;
enum disp_page {
    SENSORS,
    CONNECTION_STATUS,
    QR
};
disp_page current_page = SENSORS; // Default page to display


void setup() {

    Serial.begin(115200);
    Serial.println("Serial Initialzied.");

    delay(1000); // Wait for Serial Monitor to open
    
    tft.begin(); // Initialize TFT display
    sd.begin(tft);
    
    // webserverSetup();

    // gps_setup();
    gsm.begin();
    
    adc.begin();
    
    sensors.begin(&adc); // Initialize sensors with the ADC module

}

// char* filenames[] = {"/qr_24.bmp","/qr.bmp","/qr_full.bmp","/test.bmp","/testM1.bmp", "/qr_blue.bmp", "/qr2.bmp"};


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
    // static int idx = 0;
    // if (idx >= 7) idx = 0;
    // File bmpFile = sd.openFile(filenames[idx++]);
    // if (tft.bmpDraw(bmpFile, 0, 0) >= 0) {
    // }
    
    // webserverLoop();
    
    if (millis() % 10000 == 0) {
        adc.print_data();
        sensors.print();
        Serial.printf("Disp Page %d of %d\n", current_page, disp_pages);
        if (current_page == SENSORS) {
            tft.print_sensors(&adc, &sensors); // Print sensor values on the TFT display
        }
        else if (current_page == CONNECTION_STATUS) {
            tft.print_connection_status(&gsm); // Print connection status on the TFT display
        }
        else if (current_page == QR) {
            File bmpFile = sd.openFile("/qr.bmp");
            tft.bmpDraw(bmpFile, 0, 0);
        }
        current_page = (disp_page)(((int)current_page + 1) % disp_pages);
    }

}