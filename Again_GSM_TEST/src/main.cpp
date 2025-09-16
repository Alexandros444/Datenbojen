#include <Arduino.h>

#define MODEM_RX 16
#define MODEM_TX 17

HardwareSerial serial_AT(2);

void setup() {
    Serial.begin(115200);
    Serial.printf("Initializing GSM, TX: %d RX: %d\n", MODEM_TX, MODEM_RX);
    // Serial1.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
    serial_AT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
}

void loop() {
    // forward from USB -> SIM
    if (Serial.available()) {
        String s = Serial.readStringUntil('\n');
        Serial.println(s);
        serial_AT.println(s);
    }
    // forward from SIM -> USB
    if (serial_AT.available()) {
        while (serial_AT.available()) {
            Serial.write(serial_AT.read());
        }
    }
    delay(10);
}