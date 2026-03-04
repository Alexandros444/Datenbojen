#ifndef STATE_MODULE_H
#define STATE_MODULE_H

#include <EEPROM.h>

#define STATE_EEPROM_ADDRESS 0x30

enum system_status_code {
    SYSTEM_OK,
    SYSTEM_LOW_BATTERY
};

struct system_state{
    system_status_code status;
    int wakeup_count;
};


class state_module {
private:
    system_state get_state() {
        system_state current_state;
        EEPROM.get(STATE_EEPROM_ADDRESS, current_state);
        return current_state;
    };
    void set_state(system_state new_state){
        EEPROM.put(STATE_EEPROM_ADDRESS, new_state);
    };
public:
    state_module(){};
    ~state_module() {};

    system_state current_state;

    void begin() {
        current_state = get_state();
    };
};

#endif