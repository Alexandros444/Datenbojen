#include "tft_module.h"

// #define TFT_LED 25
// #define TFT_SCLK 15
// #define TFT_SDA 2
// #define TFT_DC 0
// #define TFT_RST 18
// #define TFT_CS 19



void tft_module::begin() {
    Serial.println("TFT setup");
    tft.begin();

    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_ORANGE, TFT_BLACK);
    tft.setTextSize(1);
    tft.setCursor(0, 0);
    tft.println("TFT setup done");
    Serial.println("TFT setup done");
}

void tft_module::print(String text, int row) {
    int y_pos = row * 15; // Assuming each row is 15 pixels high
    tft.fillRect(0, y_pos, tft.width(), 15, TFT_BLACK);
    tft.setCursor(0, y_pos);
    tft.println(text.c_str());
}

void tft_module::print_sensors(adc_module* adc, sensors_module* sensors) {
    if (sensors == nullptr || adc == nullptr) {
        Serial.println("Error: Null pointer detected for sensors or adc");
        return;
    }
    print("TDS Value: " + String((float)sensors->tdsValue, 2) + " ppm", 0);
    print("Temperature: " + String((float)sensors->temperature, 2) + " °C", 1);
    print("Turbidity: " + String((float)sensors->turbidity, 2) + " NTU", 2);
    print("DO Value: " + String((float)sensors->doValue, 2) + " %", 3);
    print("pH Value: " + String((float)sensors->phValue, 2), 4);

    String adc_data = "ADC: 0: " + String((float)adc->readVoltage(0), 2) + "V, " +
                      "1: " + String((float)adc->readVoltage(1), 2) + " V, " +
                      "2: " + String((float)adc->readVoltage(2), 2) + " V, " +
                      "3: " + String((float)adc->readVoltage(3), 2) + " V";
    print(adc_data, 5);
}

void tft_module::dot_test() {
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