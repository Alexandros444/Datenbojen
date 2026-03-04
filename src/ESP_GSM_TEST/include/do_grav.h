/*
MIT License

Copyright (c) 2020 Atlas Scientific

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE
*/

#ifndef DO_GRAV_H
#define DO_GRAV_H

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "adc_module.h"

#define DEFAULT_SAT_VOLTAGE_CONST (40.0*11.0)


class Gravity_DO {
	public:
	
		Gravity_DO() {};
		
		bool begin(uint8_t pin, adc_module* adc, uint16_t EEPROM_offset); // Initialisiere den DO-Sensor
	
		virtual float read_voltage();
		virtual float read_voltage_no_adc();
		float read_do_percentage(float voltage_mV);
		float read_do_percentage();
        
        void cal();
		void cal_clear();

		void ask_manual_calibration();

		enum grav_type{
            GRAV_PH = 1,
            GRAV_DO,
            GRAV_ORP,
            GRAV_RTD
        };
		
protected:
		static const int volt_avg_len = 1000;
		static const uint8_t magic_char = 0xAA;
		
private:
		adc_module* adc = nullptr;
        const float DEFAULT_SAT_VOLTAGE = DEFAULT_SAT_VOLTAGE_CONST;
		struct DO {
		  const uint8_t magic = magic_char;
          const uint8_t type = GRAV_DO;
		  float full_sat_voltage = DEFAULT_SAT_VOLTAGE_CONST;
		};
		struct DO Do;
		uint8_t pin = 0;
		uint16_t EEPROM_offset = 0;
};

#endif