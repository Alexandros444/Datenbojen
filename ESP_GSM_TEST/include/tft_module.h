#ifndef TFT_MODULE_H
#define TFT_MODULE_H

#include <TFT_eSPI.h>
#include "adc_module.h"
#include "sensors_module.h"

class tft_module {
private:
    TFT_eSPI tft = TFT_eSPI(); // TFT display object
public:
    tft_module() {};
    ~tft_module() {};
    void begin();
    void print(String text, int row);
    void dot_test(); // Function to print dots in a loop
    void print_sensors(adc_module* adc, sensors_module* sensors); // Function to print sensor values on the TFT display
};

#endif