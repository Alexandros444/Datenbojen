#include "sleep_module.h"

void sleep_module::system_cut_power() {
    digitalWrite(SLEEP_PIN, HIGH);
    delay(10000); // Wait 10s, for testing without sleep chip
    ESP.restart();
}

void sleep_module::begin() {
    pinMode(SLEEP_PIN, OUTPUT);
    digitalWrite(SLEEP_PIN, LOW);
}