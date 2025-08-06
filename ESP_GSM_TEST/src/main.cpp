#include <Arduino.h>

#include "config.h"

#include "webserver_util.h"
#include "gps_module.h"
#include "tft_sd_module.h"
#include "adc_module.h"
#include "sensors_module.h"
#include "led_module.h"

adc_module adc;
sensors_module sensors;
tft_module tft;
sd_module sd;
gsm_module gsm; // Create a GSM module instance
led_module led;

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

    led.begin();
    led.clear();
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
    // if (tft.draw_bmp_img(bmpFile, 0, 0) >= 0) {
    // }
    
    // webserverLoop();
    static unsigned long last_disp_time = 0;
    if (millis() - last_disp_time > 10000) {
        last_disp_time = millis();
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
            tft.draw_bmp_img(bmpFile, 1, 1);
        }
        current_page = (disp_page)(((int)current_page + 1) % disp_pages);

        //print esp FREE RAM
        Serial.printf("Free RAM: %d\n", ESP.getFreeHeap());
        //pritn esp FREE Flash
        Serial.printf("Free Flash: %d\n", ESP.getFreeSketchSpace());
        //print esp SKETCH SIZE
        Serial.printf("Sketch Size: %d\n", ESP.getSketchSize());

    }

    led.loop();

}