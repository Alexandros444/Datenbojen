#include "adc_module.h"

adc_module::adc_module() {}
adc_module::~adc_module() {}

int adc_module::begin() {
    Serial.println("Initializing ADS1115 ADC module...");
    Serial.println("Single-ended readings from AIN0 with >3.0V comparator");
    Serial.println("ADC Range: +/- 6.144V (1 bit = 0.1875mV)");

    Wire.begin(I2C_SDA, I2C_SCL);

    // The ADC input range (or gain) can be changed via the following
    // functions, but be careful never to exceed VDD +0.3V max, or to
    // exceed the upper and lower limits if you adjust the input range!
    // Setting these values incorrectly may destroy your ADC!
    //                                                                ADS1015  ADS1115
    //                                                                -------  -------
    // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
    // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
    // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
    // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
    // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
    // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
    if (!ads.begin(ADS1X15_ADDRESS, &Wire)) {
        Serial.println("Failed to initialize ADS.");
        initialized = false;
        return 1;
    }
    // Setup 3V comparator on channel 0
    ads.startComparator_SingleEnded(0, 1000);
    initialized = true;
    return 0;
}


void adc_module::print_data(){
    if (!initialized)
        return;

    adc_data data = read();
    String adc_data_str = "ADC Data: ";
    adc_data_str += "ADC0: " + String(data.adc0) + " (" + String(data.adc0_volts, 4) + "V), ";
    adc_data_str += "ADC1: " + String(data.adc1) + " (" + String(data.adc1_volts, 4) + "V), ";
    adc_data_str += "ADC2: " + String(data.adc2) + " (" + String(data.adc2_volts, 4) + "V), ";
    adc_data_str += "ADC3: " + String(data.adc3) + " (" + String(data.adc3_volts, 4) + "V)";
    Serial.println(adc_data_str);
}

adc_data adc_module::read() {
    if (!initialized)
        return (adc_data) {0x0000, 0.0f, 0x0000, 0.0f, 0x0000, 0.0f, 0x0000, 0.0f};
    adc_data data;
    int16_t a = ads.getLastConversionResults();
    data.adc0 = ads.readADC_SingleEnded(0);
    data.adc0_volts = ads.computeVolts(data.adc0);
    data.adc1 = ads.readADC_SingleEnded(1);
    data.adc1_volts = ads.computeVolts(data.adc1);
    data.adc2 = ads.readADC_SingleEnded(2);
    data.adc2_volts = ads.computeVolts(data.adc2);
    data.adc3 = ads.readADC_SingleEnded(3);
    data.adc3_volts = ads.computeVolts(data.adc3);
    return data;
}