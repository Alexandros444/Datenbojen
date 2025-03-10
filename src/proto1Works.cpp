#define TINY_GSM_MODEM_SIM800
#define TINY_GSM_DEBUG Serial
#include "TinyGsmClient.h"
#include "ArduinoHttpClient.h"
#include "yazdan_code.h"

#define TINY_GSM_MODEM_SIM800
#define SerialMon Serial
#define SerialAT Serial2

const char apn[]  = "internet.telekom"; // APN for your GSM network
const char user[] = "telekom";         // User for APN, if required
const char pass[] = "tm";         // Password for APN, if required

const char server[] = "jserv.ddns.net";
const int  port = 8080;
const char resource[] = "/post";

#define SMS_TARGET "015739119874"

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
HttpClient http(client, server, port);

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

    // Set GSM module baud rate
    SerialAT.begin(9600);
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
    Serial.printf("Connected: %d", modem.isNetworkConnected());
    
    
    Serial.println("Initializing sensors...");
    sensors_init();
    Serial.println("Sensors initialized");
}

void loop() {

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

    modem.sendSMS(SMS_TARGET, buff);  // Setzt CMGF=1 -> Textmodus
    Serial.println("SMS sent: ");
    // // Prepare data to send
    // String postData = "value=" + String(tdsValue);

    // // Make a HTTP POST request
    // SerialMon.println("Making a POST request");
    // http.beginRequest();
    // http.post(resource);
    // http.sendHeader("Content-Type", "application/x-www-form-urlencoded");
    // http.sendHeader("Content-Length", postData.length());
    // http.beginBody();
    // http.print(postData);
    // http.endRequest();

    // // Read the response
    // int statusCode = http.responseStatusCode();
    // String response = http.responseBody();
    // SerialMon.print("Status code: ");
    // SerialMon.println(statusCode);
    // SerialMon.print("Response: ");
    // SerialMon.println(response);

    // Wait for 10 mins before next reading
    delay(60*1000);
}