
#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "do_grav.h"
#include "EEPROM.h"

bool Gravity_DO::begin(uint8_t pin, adc_module* adc, uint16_t EEPROM_offset) {
    this->pin = pin;
    this->adc = adc;
    this->EEPROM_offset = EEPROM_offset;
    if (!adc->isInitialized()) {
        Serial.println("ADC not initialized, cannot begin DO sensor.");
        return false;
    }

#if defined(ESP8266) || defined(ESP32)
        EEPROM.begin(1024);
    #endif 
	if((EEPROM.read(this->EEPROM_offset) == magic_char)
    && (EEPROM.read(this->EEPROM_offset + sizeof(uint8_t)) == GRAV_DO)){
		EEPROM.get(this->EEPROM_offset,Do);
		return true;
    }
	return false;
}

float Gravity_DO::read_voltage() {
    return adc->readVoltage(pin);
}


float Gravity_DO::read_voltage_no_adc() {
    float voltage_mV = 0;
    for (int i = 0; i < volt_avg_len; ++i) {
	#if defined(ESP32)
	//ESP32 has significant nonlinearity in its ADC, we will attempt to compensate 
	//but you're on your own to some extent
	//this compensation is only for the ESP32
	//https://github.com/espressif/arduino-esp32/issues/92
		voltage_mV += analogRead(this->pin) / 4095.0 * 3300.0 + 130;
	#else
		voltage_mV += analogRead(this->pin) / 1024.0 * 5000.0;
    #endif 
    }
    voltage_mV /= volt_avg_len;
    return voltage_mV;
}

float Gravity_DO::read_do_percentage(float voltage_mV) {
    return voltage_mV * 100.0 / this->Do.full_sat_voltage;
}

void Gravity_DO::cal() {
    this->Do.full_sat_voltage = read_voltage();
    EEPROM.put(this->EEPROM_offset,Do);
    #if defined(ESP8266) || defined(ESP32)
        EEPROM.commit(); 
    #endif
}

void Gravity_DO::cal_clear() {
    this->Do.full_sat_voltage = DEFAULT_SAT_VOLTAGE;
    EEPROM.put(this->EEPROM_offset,Do);
    #if defined(ESP8266) || defined(ESP32)
        EEPROM.commit(); 
    #endif
}

float Gravity_DO::read_do_percentage() {
  return(read_do_percentage(read_voltage()));
}

/***
 *   Funktion zum Parsen von Kalibrierungsbefehlen für den DO-Sensor
 */
void Gravity_DO::ask_manual_calibration() {

// Kalibrierungshinweis für den DO-Sensor ausgeben
  Serial.println(F("Gib 'CAL' ein, um den DO-Sensor (100%% Sättigung in Luft) zu kalibrieren."));
  Serial.println(F("Gib 'CAL,CLEAR' ein, um die DO-Kalibrierung zu löschen."));

  char user_input[32] = { 0 };

  size_t bytes_read = Serial.readBytesUntil('\r', user_input, sizeof(user_input));
  if (bytes_read <= 0)
    return;

  strupr(user_input);
  String cmd = String(user_input);
  if (cmd.startsWith("CAL")) {
    int index = cmd.indexOf(",");
    if (index != -1) {
      String param = cmd.substring(index + 1, cmd.length());
      if (param.equals("CLEAR")) {
        cal_clear();
        Serial.println("DO Sensorkalibrierung gelöscht");
      }
    }
    else {
      cal();
      Serial.println("DO Sensor kalibriert");
    }
  }
}