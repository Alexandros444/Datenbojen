#ifndef YAZDAN_CODE_H
#define YAZDAN_CODE_H

#include <OneWire.h>
#include <Arduino.h>
#include <EEPROM.h>

#include "GravityTDS.h"
#include "DFRobot_PH.h"
#include "do_grav.h"
#include "adc_module.h"

// DS18B20 Setup, Digitaler Temperatursensor
#define DS18B20_PIN 33
#define TDS_ADC_PIN 0
#define PH_ADC_PIN 1
#define DO_ADC_PIN 2
#define TRB_ADC_PIN 3

#define ADC_RESOLUTION 65536 // 16-bit ADC resolution
#define ADC_Voltage 5


#define TDS_EEPROM_ADDRESS 0x00 // EEPROM address for TDS calibration, 1 Float 4 Bytes
#define PH_EEPROM_ADDRESS 0x10 // EEPROM address for pH calibration, 2 Floats 8 Bytes
#define DO_EEPROM_ADDRESS 0x20 // EEPROM address for DO calibration, 1 Float, 2 uint8 6 Bytes


void sensors_init();
void sensors_loop();
float readTemperature();
float readTurbidity(float temperature);

extern GravityTDS gravityTds;

class sensors_module {
public:
    adc_module* adc; // ADC-Modul für die Sensoren
    OneWire ds; // Initialisierung des DS18B20, Digitaler Temperatursensor
    GravityTDS gravityTds;
    DFRobot_PH ph;
    Gravity_DO DO;
    sensors_module() {}
    ~sensors_module() {}
    void begin(adc_module* adc);
    void print();
    float readTemperature();
    float readTurbidity(float temperature);

    float tdsValue = 0;  // hier feste Werte von TDS-Messwerte definiert (Variable für den TDS-Wert)
    float phValue = 0;
    float turbidity = 0; // Variable für den Trübheitswert
    float doValue = 0; // Variable für den Sauerstoffwert
    float temperature = 0; // Variable für die Temperaturmessung
};

#endif


