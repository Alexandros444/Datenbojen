/***************************************************
 DFRobot Gravity: Analog TDS Sensor/Meter
 <https://www.dfrobot.com/wiki/index.php/Gravity:_Analog_TDS_Sensor_/_Meter_For_Arduino_SKU:_SEN0244>
 
 ***************************************************
 This sample code shows how to read the tds value and calibrate it with the standard buffer solution.
 707ppm(1413us/cm)@25^c standard buffer solution is recommended.
 
 Created 2018-1-3
 By Jason <jason.ling@dfrobot.com@dfrobot.com>
 
 GNU Lesser General Public License.
 See <http://www.gnu.org/licenses/> for details.
 All above must be included in any redistribution.
 ****************************************************/

#ifndef GRAVITY_TDS_H
#define GRAVITY_TDS_H

#include "Arduino.h"
#include "adc_module.h"

#ifndef ReceivedBufferLength
#define ReceivedBufferLength 15
#endif
#define TdsFactor 0.5  // tds = ec / 2

class GravityTDS
{
public:
    GravityTDS();
    ~GravityTDS();

    void begin(uint8_t pin, adc_module* adc, uint16_t EEPROM_ADDRESS);  //initialization
    void update(); //read and calculate
    void setTemperature(float temp);  //set the temperature and execute temperature compensation
    float getTdsValue();
    float getEcValue();

private:
    int pin;
    float aref;  // default 5.0V on Arduino UNO
    float adcRange;
    float temperature;
    int kValueAddress;     //the address of the K value stored in the EEPROM
    char cmdReceivedBuffer[ReceivedBufferLength+1];   // store the serial cmd from the serial monitor
    byte cmdReceivedBufferIndex;
 
    float kValue;      // k value of the probe,you can calibrate in buffer solution ,such as 706.5ppm(1413us/cm)@25^C 
    float analogValue;
    float voltage;
    float ecValue; //before temperature compensation
    float ecValue25; //after temperature compensation
    float tdsValue;

    void readKValues();
    boolean cmdSerialDataAvailable();
    byte cmdParse();
    void ecCalibration(byte mode);
    void resetKValue(); // Reset kValue to default (1.0)

    adc_module* adc; // ADC module for the sensors
};

#endif
