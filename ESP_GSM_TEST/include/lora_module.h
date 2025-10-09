#ifndef LORA_MODULE_H
#define LORA_MODULE_H

#include <LoRa.h>
#include <SPI.h>
#include "gpio_module.h"

#define LORA_RST_PIN PCF0 // RST
#define LORA_CS_PIN PCF1 // NSS 
#define LORA_DO1_PIN 0 // DO1 - Interrupt Pin (Don't know if it works with the pcf in chain) (Pin to interrupt mit 100 funzt auf jeden nicht )

#define LORA_FREQ 433E6

class lora_module {
    public:
        lora_module(){};
        ~lora_module(){};
        void begin();
        int sendPacket(String message);
};


#endif