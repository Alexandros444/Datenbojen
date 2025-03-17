// #include <Arduino.h>
// #define TINY_GSM_MODEM_SIM800
// #define TINY_GSM_DEBUG Serial
// #include "TinyGsmClient.h"
// #define SMS_TARGET "015739119874"
// const char apn[] = "internet.v6.telekom";
// // HardwareSerial Serial2(2);
// #define GSM_RX_PIN 16
// #define GSM_TX_PIN 17
// #define GSM_BAUD 9600
// TinyGsm modem(Serial2);

// #define TDS_SENSOR_PIN 12
// #define VREF 3.3          // analog reference voltage(Volt) of the ADC
// #define SCOUNT 30         // sum of sample point
// int analogBuffer[SCOUNT]; // store the analog value in the array, read from ADC
// int analogBufferTemp[SCOUNT];
// int analogBufferIndex = 0, copyIndex = 0;
// float averageVoltage = 0, tdsValue = 0, temperature = 25;

// int getMedianNum(int bArray[], int iFilterLen);

// char ser_buffer[1024];

// void updateSerial()
// {
//   while(Serial2.available()) 
//   {
//     size_t ser_in_size = Serial2.read(ser_buffer,1024);
//     Serial.write(ser_buffer,ser_in_size);//Forward what Software Serial received to Serial Port
//   }
//   while (Serial.available()) 
//   {
//     size_t ser_in_size = Serial.read(ser_buffer,1024);
//     Serial.write(ser_buffer, ser_in_size);
//     Serial2.write(ser_buffer, ser_in_size);//Forward what Serial received to Software Serial Port
//   }
//   delay(2000);
// }

// void sendATCommand(String cmd){
//   Serial.printf("Sending to GSM: %s",cmd);
//   Serial2.println(cmd);
//   updateSerial();
// }

// void test_AT_Raw(){
//   sendATCommand("AT"); //Once the handshake test is successful, it will back to OK
//   sendATCommand("AT+CSQ"); //Signal quality test, value range is 0-31 , 31 is the best
//   sendATCommand("AT+CCID"); //Read SIM information to confirm whether the SIM is plugged
//   sendATCommand("AT+CREG?"); //Check whether it has registered in the network
// }

// void sendSMS(float val)
// {
//   bool res = modem.sendSMS(SMS_TARGET, String("Esp32 Sensor Data: ") + String(val));
//   DBG("SMS:" + res ? "OK" : "fail");
// }


// void sendSMS_RAW(){
//   Serial.println("Sending SMS");
// //List of supported responses
//   sendATCommand("AT+CMGF=?");
  
// //Get the current SMS Mode
// // 0 - PDU Mode, 1- Text Mode,
//   sendATCommand("AT+CMGF?");

// //WAIT for 1 sec
//   // sendATCommand("WAIT=1");

// //Check whether device is registered
// //If device is not registered, SMS cannot be sent
//   sendATCommand("AT+CREG?");

// //First let us send SMS in text mode
//   sendATCommand("AT+CMGF=1");
// // Send the message. Set your To number below
// // Yazdan "016096667146"
//   sendATCommand("AT+CMGS=\"015739119874\"");
//   //Type the SMS text below
//   sendATCommand("Text Message Test from GSM Modul -Alex");
//   Serial2.write(0x1A);
//   delay(2000);


// //Send the CTRL+Z character. ^z is special command of AT Command Script engine.
//   Serial.println("SMS Send");
// }


// void setup_main()
// {
//   Serial.begin(112500);
//   while (!Serial)
//   {
//     delay(10);
//   }
//   Serial2.begin(9600, SERIAL_8N1, GSM_RX_PIN, GSM_TX_PIN);

//   // test_AT_Raw();
//   // sendSMS_RAW();


//   // serial_relay();

//   // S.println("AT");
//   // delay(100);
//   // if (GSM_Serial.available())
//   // {
//   //   Serial.println(GSM_Serial.readString());
//   // }
//   // serial_relay();

//   modem.setBaud(9600);
//   modem.begin();
//   modem.getSimStatus();
//   modem.getModemInfo();
//   // printInfoGSM();
//   // modem.sendAT("AT+CMGF=0");
//   Serial.printf("Connected: %d", modem.isNetworkConnected());
//   modem.sendSMS(SMS_TARGET, "Hello World!");  // Setzt CMGF=1 -> Textmodus


//   // modem.ge("http://www.google.com");''

//   // bool ret = modem.isNetworkConnected();
//   // DBG("Network:", ret ? "Connected" : "Not Connected");
//   // modem.maintain();

//   // Serial.println("Connecting GSM Module");
//   // if (!modem.restart())
//   // {
//   //   Serial.println("Failed to restart modem, delaying 10s and retrying");
//   //   delay(10000);
//   //   esp_restart();
//   // }
//   // Serial.println("GSM Module connected");
//   // sendSMS_RAW();


//   // while (!modem.isNetworkConnected()) {
//   //   Serial.print(F("GSM...."));
//   //   while (!modem.restart()) {
//   //     Serial.print(".");
//   //   }
//   //   Serial.println();

//   //   Serial.print(F("Waiting for network...."));
//   //   if (modem.waitForNetwork() && modem.isNetworkConnected()) {
//   //     Serial.println(F("connected."));
//   //   }
//   //   else {
//   //     Serial.println(F("connection error."));
//   //   }
//   // }
//   // Serial.println("Module Connected to Network");

//   // modem.sendSMS("Hello World!",SMS_TARGET);
//   // modem.gprsConnect(apn);
//   // delay(5000);
//   // if (!modem.isGprsConnected())
//   //   Serial.println("GPRS not Connected!");
//   // sendSMS(1);
//   // pinMode(TDS_SENSOR_PIN, INPUT);
//   while(true){
//     updateSerial();
//   }
// }



// void printInfoGSM(){
  
//   String modemInfo = modem.getModemInfo();
//   DBG("Modem Info:", modemInfo);

//   String name = modem.getModemName();
//   DBG("Modem Name:", name);

//   String manufacturer = modem.getModemManufacturer();
//   DBG("Modem Manufacturer:", manufacturer);

//   String hw_ver = modem.getModemModel();
//   DBG("Modem Hardware Version:", hw_ver);

//   String fv_ver = modem.getModemRevision();
//   DBG("Modem Firware Version:", fv_ver);

//   bool res = modem.isGprsConnected();
//   DBG("GPRS status:", res ? "connected" : "not connected");

//   String ccid = modem.getSimCCID();
//   DBG("CCID:", ccid);

//   String imei = modem.getIMEI();
//   DBG("IMEI:", imei);

//   String imsi = modem.getIMSI();
//   DBG("IMSI:", imsi);

//   String cop = modem.getOperator();
//   DBG("Operator:", cop);

//   String prov = modem.getProvider();
//   DBG("Provider:", prov);

//   IPAddress local = modem.localIP();
//   DBG("Local IP:", local);

//   int csq = modem.getSignalQuality();
//   DBG("Signal quality:", csq);
// }




// void loop_main()
// {
//   static unsigned long analogSampleTimepoint = millis();
//   if (millis() - analogSampleTimepoint > 40U) // every 40 milliseconds,read the analog value from the ADC
//   {
//     analogSampleTimepoint = millis();
//     analogBuffer[analogBufferIndex] = analogRead(TDS_SENSOR_PIN); // read the analog value and store into the buffer
//     analogBufferIndex++;
//     if (analogBufferIndex == SCOUNT)
//       analogBufferIndex = 0;
//   }
//   static unsigned long printTimepoint = millis();
//   if (millis() - printTimepoint > 800U)
//   {
//     printTimepoint = millis();
//     for (copyIndex = 0; copyIndex < SCOUNT; copyIndex++)
//       analogBufferTemp[copyIndex] = analogBuffer[copyIndex];
//     averageVoltage = getMedianNum(analogBufferTemp, SCOUNT) * (float)VREF / 4096.0;                                                                                                  // read the analog value more stable by the median filtering algorithm, and convert to voltage value
//     float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);                                                                                                               // temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
//     float compensationVolatge = averageVoltage / compensationCoefficient;                                                                                                            // temperature compensation
//     tdsValue = (133.42 * compensationVolatge * compensationVolatge * compensationVolatge - 255.86 * compensationVolatge * compensationVolatge + 857.39 * compensationVolatge) * 0.5; // convert voltage value to tds value
//     // Serial.print("voltage:");
//     // Serial.print(averageVoltage,2);
//     // Serial.print("V   ");
//     Serial.print("TDS Value:");
//     Serial.print(tdsValue, 0);
//     Serial.println("ppm");
//     if (tdsValue > 20){
//       sendSMS(tdsValue);
//     }
//   }
// }

// int getMedianNum(int bArray[], int iFilterLen)
// {
//   int bTab[iFilterLen];
//   for (byte i = 0; i < iFilterLen; i++)
//     bTab[i] = bArray[i];
//   int i, j, bTemp;
//   for (j = 0; j < iFilterLen - 1; j++)
//   {
//     for (i = 0; i < iFilterLen - j - 1; i++)
//     {
//       if (bTab[i] > bTab[i + 1])
//       {
//         bTemp = bTab[i];
//         bTab[i] = bTab[i + 1];
//         bTab[i + 1] = bTemp;
//       }
//     }
//   }
//   if ((iFilterLen & 1) > 0)
//     bTemp = bTab[(iFilterLen - 1) / 2];
//   else
//     bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
//   return bTemp;
// }