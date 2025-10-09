#ifndef SLEEP_MODULE_H
#define SLEEP_MODULE_H

#define SLEEP_PIN 12

#include <Arduino.h>

class sleep_module {
public:
    sleep_module(){};
    ~sleep_module(){};
    void begin();
    void system_cut_power();
};


#endif