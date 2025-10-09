#ifndef GPIO_MODULE_H
#define GPIO_MODULE_H

#include "PCF8575.h"
#include "Wire.h"

// PCF8575 pin definitions for convenience
#define PCF0  100
#define PCF1  101
#define PCF2  102
#define PCF3  103
#define PCF4  104
#define PCF5  105
#define PCF6  106
#define PCF7  107
#define PCF8  108
#define PCF9  109
#define PCF10 110
#define PCF11 111
#define PCF12 112
#define PCF13 113
#define PCF14 114
#define PCF15 115

#define PCF_INT_PIN 2

// Global pointer to PCF8575 instance, must be defined in main code
extern PCF8575* pcf8575;

// Redefine digitalRead, digitalWrite, and pinMode macros
#define digitalReadPCF(pin) ((pcf8575) ? pcf8575->digitalRead((pin) - 100) : 0)

#define digitalWritePCF(pin, val)  ((pcf8575) ? pcf8575->digitalWrite((pin) - 100, val) : false)

#define pinModePCF(pin, mode) ((pcf8575) ? pcf8575->pinMode((pin) - 100, mode) : (void)0)


class gpio_module {
public:
    gpio_module(){};
    ~gpio_module(){};
    void begin();
};

#endif