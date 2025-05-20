#include "tft_module.h"

// #define TFT_LED 25
// #define TFT_SCLK 15
// #define TFT_SDA 2
// #define TFT_DC 0
// #define TFT_RST 18
// #define TFT_CS 19


TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

void tft_setup() {
    Serial.println("TFT setup");
    tft.begin();

    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(1);
    tft.setCursor(0, 0);
    tft.println("TFT setup done");
}


void tft_loop() {
    // reapeat print dots 
    static unsigned long lastTime = 0;
    static unsigned long dot_pos_x = 0;
    unsigned long currentTime = millis();
    if (currentTime - lastTime > 1000) {
        // Serial.println("TFT loop");
        lastTime = currentTime;
        tft.setCursor(dot_pos_x, 20);
        tft.println(".");
        dot_pos_x += 6;
    }
    if (dot_pos_x > tft.width()) {
        // clear one line
        tft.fillRect(0, 20, tft.width(), 20, TFT_BLACK);
        dot_pos_x = 0;
    }
}