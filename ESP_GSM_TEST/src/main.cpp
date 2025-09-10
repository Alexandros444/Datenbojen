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

int disp_pages = 5;
enum disp_page {
    SENSORS,
    CONNECTION_STATUS,
    QR,
    URL,
    Quotes
};
disp_page current_page = SENSORS; // Default page to display

unsigned long disp_times_ms[] = {10000, 10000, 20000, 5000, 10000};

TaskHandle_t Task1;
TaskHandle_t Task2;

void led_loop( void * pvParameters );

void setup() {
    
    Serial.begin(115200);
    Serial.println("Serial Initialzied.");
    
    delay(1000); // Wait for Serial Monitor to open  
    
    tft.begin(); // Initialize TFT display
    sd.begin(tft);

    led.begin();
    // webserverSetup();

    // gps_setup();


    adc.begin();
    
    sensors.begin(&adc); // Initialize sensors with the ADC module

    Serial.println("Core in Use: "+ String(xPortGetCoreID()));
    
    xTaskCreatePinnedToCore(
          led_loop, /* Function to implement the task */
          "led_loop", /* Name of the task */
          5000,  /* Stack size in words */
          NULL,  /* Task input parameter */
          0,  /* Priority of the task */
          &Task1,  /* Task handle. */
          0); /* Core where the task should run */

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
        static unsigned long last_new_page_ms = 0;
        int wait_for_page = max((int) current_page - 1, 0);
        if (millis() > last_new_page_ms + disp_times_ms[wait_for_page]) {
            Serial.println("Core in Use: "+ String(xPortGetCoreID()));
            last_new_page_ms = millis();
            adc.print_data();
            sensors.print();
            Serial.printf("Disp Page %d of %d\n", current_page, disp_pages);
            if (current_page == SENSORS) {
                tft.print_sensors_example(); // Print sensor values on the TFT display
            }
            else if (current_page == CONNECTION_STATUS) {
                tft.print_connection_status_example(); // Print connection status on the TFT display
            }
            else if (current_page == QR) {
                File bmpFile = sd.openFile("/qr.bmp");
                tft.draw_bmp_img(bmpFile, 1, 1);
            }else if (current_page == URL) {
                tft.print_url();
            }else if (current_page == Quotes) {
                tft.print_quotes();
            }
            current_page = (disp_page)(((int)current_page + 1) % disp_pages);

            //print esp FREE RAM
            Serial.printf("Free RAM: %d\n", ESP.getFreeHeap());
            //pritn esp FREE Flash
            Serial.printf("Free Flash: %d\n", ESP.getFreeSketchSpace());
            //print esp SKETCH SIZE
            Serial.printf("Sketch Size: %d\n", ESP.getSketchSize());

        }

    // led.loop();
}




void led_loop( void * pvParameters ) {
    for(;;){
        led.loop();
        delay(1);
        led.rand_color_change();
    }
}