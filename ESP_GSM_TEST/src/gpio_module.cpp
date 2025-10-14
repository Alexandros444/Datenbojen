#include "gpio_module.h"

PCF8575 pcf8575_instance(0x20, PCF_INT_PIN, nullptr);
PCF8575* pcf8575 = &pcf8575_instance;


void gpio_module::begin() {
    Wire.begin();
    pcf8575->begin();
}