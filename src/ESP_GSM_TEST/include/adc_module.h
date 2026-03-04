#ifndef ADC_MODULE_H
#define ADC_MODULE_H

#include <Adafruit_ADS1X15.h>
#include "config.h"

struct adc_data_t {
    int16_t adc0;
    float adc0_volts;
    int16_t adc1;
    float adc1_volts;
    int16_t adc2;
    float adc2_volts;
    int16_t adc3;
    float adc3_volts;
} typedef adc_data;


class adc_module {
private:
    Adafruit_ADS1115 ads;
    bool initialized = false;

public:
    adc_module() {};
    ~adc_module() {};
    int begin();
    void print_data();
    adc_data read();
    bool isInitialized();
    float readVoltage(uint8_t pin);
    int16_t readInt(uint8_t pin);
};



#endif // ADC_MODULE_H
