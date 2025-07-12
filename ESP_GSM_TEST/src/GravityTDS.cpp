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

#include <EEPROM.h>
#include "GravityTDS.h"

#define EEPROM_write(address, p) {int i = 0; byte *pp = (byte*)&(p);for(; i < sizeof(p); i++) EEPROM.write(address+i, pp[i]);}
#define EEPROM_read(address, p)  {int i = 0; byte *pp = (byte*)&(p);for(; i < sizeof(p); i++) pp[i]=EEPROM.read(address+i);}

GravityTDS::GravityTDS()
{
    this->pin = 4;
    this->temperature = 25.0;
    this->kValueAddress = 8;
    this->kValue = 1.0;
}

GravityTDS::~GravityTDS()
{
}

void GravityTDS::setTemperature(float temp)
{
	this->temperature = temp;
}

void GravityTDS::begin(uint8_t pin, adc_module* adc, uint16_t EEPROM_ADDRESS)
{
  this->kValueAddress = EEPROM_ADDRESS;
  this->pin = pin;
  this->adc = adc;
  readKValues();
  if (this->kValue == 0.0)
    resetKValue();
}

void GravityTDS::update()
{
  DPRINTF("Updating TDS sensor on pin %d, temperature: %f, kValue: %f, ecValue: %f, ecValue25: %f, tdsValue: %f, reading voltage: %f\n", this->pin, this->temperature, this->kValue, this->ecValue, this->ecValue25, this->tdsValue, this->voltage);
  
  this->voltage = adc->readVoltage(this->pin); // read the voltage from the ADC
	this->ecValue=(133.42*this->voltage*this->voltage*this->voltage - 255.86*this->voltage*this->voltage + 857.39*this->voltage)*this->kValue;
	this->ecValue25  =  this->ecValue / (1.0+0.02*(this->temperature-25.0));  //temperature compensation
	this->tdsValue = ecValue25 * TdsFactor;
	if(cmdSerialDataAvailable() > 0)
        {
            ecCalibration(cmdParse());  // if received serial cmd from the serial monitor, enter into the calibration mode
        }
}

float GravityTDS::getTdsValue()
{
	return tdsValue;
}

float GravityTDS::getEcValue()
{
      return ecValue25;
}


void GravityTDS::resetKValue(){
  this->kValue = 1.0; // reset kValue to default
  EEPROM_write(this->kValueAddress, this->kValue);
  Serial.println(F(">>>K Value Reset to Default (1.0)<<<"));
}
  
void GravityTDS::readKValues()
{
    EEPROM_read(this->kValueAddress, this->kValue);  
    if(EEPROM.read(this->kValueAddress)==0xFF && EEPROM.read(this->kValueAddress+1)==0xFF && EEPROM.read(this->kValueAddress+2)==0xFF && EEPROM.read(this->kValueAddress+3)==0xFF)
    {
      this->kValue = 1.0;   // default value: K = 1.0
      EEPROM_write(this->kValueAddress, this->kValue);
    }
}

boolean GravityTDS::cmdSerialDataAvailable()
{
  char cmdReceivedChar;
  static unsigned long cmdReceivedTimeOut = millis();
  while (Serial.available()>0) 
  {   
    if (millis() - cmdReceivedTimeOut > 500U) 
    {
      cmdReceivedBufferIndex = 0;
      memset(cmdReceivedBuffer,0,(ReceivedBufferLength+1));
    }
    cmdReceivedTimeOut = millis();
    cmdReceivedChar = Serial.read();
    if (cmdReceivedChar == '\n' || cmdReceivedBufferIndex==ReceivedBufferLength){
		cmdReceivedBufferIndex = 0;
		strupr(cmdReceivedBuffer);
		return true;
    }else{
      cmdReceivedBuffer[cmdReceivedBufferIndex] = cmdReceivedChar;
      cmdReceivedBufferIndex++;
    }
  }
  return false;
}

byte GravityTDS::cmdParse()
{
  byte modeIndex = 0;
  if(strstr(cmdReceivedBuffer, "ENTER") != NULL) 
      modeIndex = 1;
  else if(strstr(cmdReceivedBuffer, "EXIT") != NULL) 
      modeIndex = 3;
  else if(strstr(cmdReceivedBuffer, "CAL:") != NULL)   
      modeIndex = 2;
  return modeIndex;
}

void GravityTDS::ecCalibration(byte mode)
{
    char *cmdReceivedBufferPtr;
    static boolean ecCalibrationFinish = 0;
    static boolean enterCalibrationFlag = 0;
    float KValueTemp,rawECsolution;
    switch(mode)
    {
      case 0:
      if(enterCalibrationFlag)
         Serial.println(F("Command Error"));
      break;
      
      case 1:
      enterCalibrationFlag = 1;
      ecCalibrationFinish = 0;
      Serial.println();
      Serial.println(F(">>>Enter Calibration Mode<<<"));
      Serial.println(F(">>>Please put the probe into the standard buffer solution<<<"));
      Serial.println();
      break;
     
      case 2:
      cmdReceivedBufferPtr=strstr(cmdReceivedBuffer, "CAL:");
      cmdReceivedBufferPtr+=strlen("CAL:");
      rawECsolution = strtod(cmdReceivedBufferPtr,NULL)/(float)(TdsFactor);
      rawECsolution = rawECsolution*(1.0+0.02*(temperature-25.0));
      if(enterCalibrationFlag)
      {
         // Serial.print("rawECsolution:");
         // Serial.print(rawECsolution);
         // Serial.print("  ecvalue:");
         // Serial.println(ecValue);
          KValueTemp = rawECsolution/(133.42*voltage*voltage*voltage - 255.86*voltage*voltage + 857.39*voltage);  //calibrate in the  buffer solution, such as 707ppm(1413us/cm)@25^c
          if((rawECsolution>0) && (rawECsolution<2000) && (KValueTemp>0.25) && (KValueTemp<4.0))
          {
              Serial.println();
              Serial.print(F(">>>Confrim Successful,K:"));
              Serial.print(KValueTemp);
              Serial.println(F(", Send EXIT to Save and Exit<<<"));
              kValue =  KValueTemp;
              ecCalibrationFinish = 1;
          }
          else{
            Serial.println();
            Serial.println(F(">>>Confirm Failed,Try Again<<<"));
            Serial.println();
            ecCalibrationFinish = 0;
          }        
      }
      break;

        case 3:
        if(enterCalibrationFlag)
        {
            Serial.println();
            if(ecCalibrationFinish)
            {
               EEPROM_write(kValueAddress, kValue);
               Serial.print(F(">>>Calibration Successful,K Value Saved"));
            }
            else Serial.print(F(">>>Calibration Failed"));       
            Serial.println(F(",Exit Calibration Mode<<<"));
            Serial.println();
            ecCalibrationFinish = 0;
            enterCalibrationFlag = 0;
        }
        break;
    }
}
