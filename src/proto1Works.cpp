#define TINY_GSM_MODEM_SIM800
#define TINY_GSM_DEBUG Serial
#include "TinyGsmClient.h"
// #include "ArduinoHttpClient.h"
#include "yazdan_code.h"
#include <FastLED.h>

#include <ArduinoJson.h>
#include <Http.h>

#define TINY_GSM_MODEM_SIM800
#define SerialMon Serial
#define SerialAT Serial2

#define GSM_RX_PIN 16
#define GSM_TX_PIN 17

const char apn[]  = "internet.telekom"; // APN for your GSM network
const char user[] = "telekom";         // User for APN, if required
const char pass[] = "tm";         // Password for APN, if required

const char server[] = "http://jserv.ddns.net:8080";
// const uint16_t  port = 8080;
const char resource[] = "/";

#define SMS_TARGET "+4915739119874"
#define SMS_TARGET2 "+4916091253550"


#define PIXEL_PIN     32
#define NUM_LEDS      1

#define BRIGHTNESS  50

CRGB leds[NUM_LEDS];

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
// HttpClient http(client, server, port);

// unsigned int RST_PIN = 12;
// HTTP httpClient(SerialAT, 9600, true);

// http.configureBearer();
// void newModuleCode(){
    
//     Result res = httpClient.connect();
//     Serial.printf("Connect returned %d\n",res);
//     if (res != SUCCESS){
//         return;
//     }

//     char response[256];
//     char data[100];
//     snprintf(data, 100, "{\"date\":\"%d\"}",42069);
//     Result result = httpClient.post(server, data, response);
//     Serial.printf("Post returned %d\n",result);

//     Serial.println(response);

//     httpClient.disconnect();
//     Serial.printf("Disconnected returned %d\n",res);
// }


void printStatus(){
    int8_t chargeState, percent;
    int16_t milliVolts;
    // modem.simUnlock
    modem.getBattStats(chargeState, percent, milliVolts);
    Serial.printf("Connected: %d\n", modem.isNetworkConnected());
    Serial.printf("GPRS: %d\n", modem.isGprsConnected());
    Serial.printf("Signal strength: %d\n", modem.getSignalQuality());
    Serial.printf("Battery level: %d%%\n", percent);
    Serial.printf("Battery voltage: %dmV\n", milliVolts);
    Serial.printf("Battery charging: %d\n", chargeState);
}


void led_update(){
    // Turn the LED on, then pause
  leds[0] = CRGB::Red;
  FastLED.show();
  delay(500);
  // Now turn the LED off, then pause
  leds[0] = CRGB::Black;
  FastLED.show();
  delay(500);
}

void setup() {

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    
    
    // Set console baud rate
    SerialMon.begin(115200);
    delay(10);
    
    Serial.println("Initializing LED strip...");
    FastLED.addLeds<WS2812B, PIXEL_PIN, GRB>(leds, NUM_LEDS);  // GRB ordering is typical 
    FastLED.setBrightness( BRIGHTNESS );

    led_update();
    led_update();
    led_update();
    led_update();

    
    // Set GSM module baud rate
    SerialAT.begin(9600, SERIAL_8N1, GSM_RX_PIN, GSM_TX_PIN);
    delay(3000);

    // Restart takes quite some time
    // To skip it, call init() instead of restart()
    SerialMon.println("Initializing modem...");
    // modem.init();
    // delay(5000);

    // Unlock your SIM card with a PIN if needed
    // modem.simUnlock("1234");

    // SerialMon.print("Connecting to APN: ");
    // SerialMon.println(apn);
    // // GPRS Connect hängt sich auf, wenn modem nicht initialisiert, was da los??
    // if (!modem.gprsConnect(apn)) {
    //     SerialMon.println("Failed to connect to GPRS, shutting down");
    //     SerialMon.flush();
    //     delay(10000);
    //     esp_restart();
    // }
    // SerialMon.println("GPRS connected");

    modem.setBaud(9600);
    modem.begin();
    modem.getSimStatus();
    modem.getModemInfo();
    // printInfoGSM();
    // modem.sendAT("AT+CMGF=0");
    Serial.printf("Connected: %d\n", modem.isNetworkConnected());
    
    
    Serial.println("Initializing sensors...");
    sensors_init(); 
    Serial.println("Sensors initialized");

    // Serial.println("Configure Bearer ...");
    // httpClient.configureBearer("");
}



char ser_buffer[1024];

void updateSerial()
{
  while(SerialAT.available()) 
  {
    size_t ser_in_size = SerialAT.read(ser_buffer,1024);
    Serial.print("GSM: ");
    Serial.write(ser_buffer,ser_in_size);//Forward what Software Serial received to Serial Port
  }
  while (Serial.available()) 
  {
    size_t ser_in_size = Serial.read(ser_buffer,1024);
    Serial.print("Send to GSM: ");
    Serial.write(ser_buffer, ser_in_size);
    SerialAT.write(ser_buffer, ser_in_size);//Forward what Serial received to Software Serial Port
  }
  delay(2000);
}

int onlySerial = 0;

void handleSerial(){
    if (Serial.available()){
        char a = Serial.peek();
        if (a == 's'){
            onlySerial = !onlySerial;
            Serial.printf("Only Serial mode: %d\n",onlySerial);
        }
    }
    updateSerial();
}

int debugMode = 1;

void sendCmd(const char* cmd)
{
    //SerialAT.listen();
    SerialAT.flush();
    delay(500);
    SerialAT.write(cmd);
    SerialAT.flush();
}

int waitForResp(const char *resp, unsigned int timeout)
{
    int len = strlen(resp);
    int sum=0;
    unsigned long timerStart,timerEnd;
    timerStart = millis();

    while(1) {
        if(SerialAT.available()) {
            char c = SerialAT.read();
            if (debugMode) Serial.print(c);
            sum = (c == resp[sum] || resp[sum] == 'X') ? sum+1 : 0;
            if(sum == len)break;
        }
        timerEnd = millis();
        if(timerEnd - timerStart > timeout) {
            return FALSE;
        }
    }

    while(SerialAT.available()) {
        SerialAT.read();
    }

    return TRUE;
}

int sendCmdAndWaitForResp(const char* cmd, const char *resp, unsigned timeout)
{
    sendCmd(cmd);
    return waitForResp(resp,timeout);
}



void gsm_http_post(float postData);

void loop() {
    handleSerial();
    if (onlySerial)
        return;
    Serial.printf("GSM Module Status:\n");
    printStatus();
    Serial.println("Reading sensors...");
    sensors_loop();
    float tdsValue = gravityTds.getTdsValue();
    
    if (readTemperature() == -1000){
        delay(1000);
        return;
    }

    SerialMon.print("tds value: ");
    SerialMon.println(tdsValue);
    char buff[50] = {0};
    snprintf(buff, 50, "TDS: %f", tdsValue);

    // int status = modem.sendSMS(SMS_TARGET2, buff);  // Setzt CMGF=1 -> Textmodus
    // Serial.printf("SMS sent: %s\n", status ? "OK" : "fail");
    Serial.println("Stopped. Data ready to Send.\nWaiting for serial Input.\n");
    while(!Serial.available()){
        delay(1000);
    }
    while (Serial.available()){
        char a = Serial.read();
        if (a == 's'){
            onlySerial = !onlySerial;
            Serial.printf("Only Serial mode: %d\n",onlySerial);
            return;
        }
    }    
    Serial.println("Sending over Network:");
    gsm_http_post(readTemperature());
    // newModuleCode();
}

void gsm_send_serial(String cmd) {
    Serial.println(cmd);
    SerialAT.println(cmd);
    delay(1000);
    updateSerial();
}

void gsm_http_post(float postdata) {
    char data[100];
    int len = snprintf(data, 100, "{\"temp\":\"%f\"}",postdata);
    Serial.println(" --- Start GPRS & HTTP --- ");
    gsm_send_serial("AT+CREG?");
    gsm_send_serial("AT+SAPBR=1,1");
    gsm_send_serial("AT+SAPBR=2,1");
    gsm_send_serial("AT+SAPBR=4,1");
    gsm_send_serial("AT+HTTPINIT");
    gsm_send_serial("AT+HTTPPARA=CID,1");
    gsm_send_serial("AT+HTTPPARA=URL," + String(server));
    gsm_send_serial("AT+HTTPPARA=CONTENT,application/json");
    gsm_send_serial("AT+HTTPDATA="+String(len)+",5000");
    gsm_send_serial(data);
    gsm_send_serial("AT+HTTPACTION=1");
    gsm_send_serial("AT+HTTPREAD");
    gsm_send_serial("AT+HTTPTERM");
    gsm_send_serial("AT+SAPBR=0,1");
  }


// void postData(float tdsValue){
//     // TODO: Eigene Post AT Commands
//     // https://www.aeq-web.com/sim800-gprs-http-post-get-request-at-commands/?lang=en
//     // Prepare data to send
//     String postData = "value=" + String(tdsValue);
//     // Make a HTTP POST request
//     SerialMon.println("Making a POST request");
//     http.beginRequest();
//     Serial.printf("Post Returned %d\n",http.post(resource));
//     http.sendHeader("Content-Type", "text/plain");
//     http.sendHeader("Content-Length", postData.length());
//     http.beginBody();
//     Serial.printf("Body Len: %d\n",http.print(postData));
//     http.endRequest();

//     // Read the response
//     int statusCode = http.responseStatusCode();
//     String response = http.responseBody();
//     SerialMon.print("Status code: ");
//     switch (statusCode)
//     {
//     case HTTP_SUCCESS:
//         Serial.println("HTTP_SUCCESS");
//         break;
//     case HTTP_ERROR_CONNECTION_FAILED:
//         Serial.println("HTTP_ERROR_CONNECTION_FAILED");
//         break;
//     case HTTP_ERROR_API:
//         Serial.println("HTTP_ERROR_API");
//         break;
//     case HTTP_ERROR_TIMED_OUT:
//         Serial.println("HTTP_ERROR_TIMED_OUT");
//         break;
//     case HTTP_ERROR_INVALID_RESPONSE:
//         Serial.println("HTTP_ERROR_INVALID_RESPONSE");
//         break;
//     default:
//         break;
//     }
//     SerialMon.print("Response: ");
//     SerialMon.println(response);

//     // Wait for 10 mins before next reading
//     delay(10*1000);
// }
