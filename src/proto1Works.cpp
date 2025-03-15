#define TINY_GSM_MODEM_SIM800
#define TINY_GSM_DEBUG Serial
#include "TinyGsmClient.h"
#include "yazdan_code.h"

#include <ArduinoJson.h>

#include <FastLED.h>

#define SerialMon Serial
#define SerialAT Serial2

#define GSM_RX_PIN 16
#define GSM_TX_PIN 17

// const char apn[]  = "internet.telekom"; // APN for your GSM network
// const char user[] = "telekom";         // User for APN, if required
// const char pass[] = "tm";         // Password for APN, if required

const char server[] = "http://jserv.ddns.net:8080";

#define SMS_TARGET "+4915739119874"
#define SMS_TARGET2 "+4916091253550"

#define PIXEL_PIN 32
#define NUM_LEDS 1
#define BRIGHTNESS 50

CRGB leds[NUM_LEDS];

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);

#define SERIAL_RELAY_BUFFER_SIZE 1024
char ser_buffer[SERIAL_RELAY_BUFFER_SIZE];
int manual_at_mode = 0;

void printGSMNetworkStatus()
{
    Serial.printf("GSM Module Status:\n");
    Serial.printf("Connected: %d\n", modem.isNetworkConnected());
    Serial.printf("GPRS: %d\n", modem.isGprsConnected());
    Serial.printf("Signal strength: %d\n", modem.getSignalQuality());
}

void printBattStatus()
{
    int8_t chargeState, percent;
    int16_t milliVolts;
    modem.getBattStats(chargeState, percent, milliVolts);
    Serial.printf("Battery level: %d%%\n", percent);
    Serial.printf("Battery voltage: %dmV\n", milliVolts);
    Serial.printf("Battery charging: %d\n", chargeState);
}

void led_update()
{
    // Turn the LED on, then pause
    leds[0] = CRGB::Red;
    FastLED.show();
    delay(500);
    // Now turn the LED off, then pause
    leds[0] = CRGB::Black;
    FastLED.show();
    delay(500);
}

void play_startup_Led_Blink()
{
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
}

void init_leds()
{
    Serial.println("Initializing LED strip...");
    FastLED.addLeds<WS2812B, PIXEL_PIN, GRB>(leds, NUM_LEDS); // GRB ordering is typical
    FastLED.setBrightness(BRIGHTNESS);

    led_update();
    led_update();
    led_update();
    led_update();
}

void init_modem()
{
    SerialMon.println("Initializing modem...");
    // Set GSM module baud rate
    SerialAT.begin(9600, SERIAL_8N1, GSM_RX_PIN, GSM_TX_PIN);
    delay(3000);

    modem.setBaud(9600);
    modem.begin();
    modem.getSimStatus();
    modem.getModemInfo();
    // printInfoGSM();
    // modem.sendAT("AT+CMGF=0");
    Serial.printf("Connected: %d\n", modem.isNetworkConnected());
}

void init_serial()
{
    // Set console baud rate
    SerialMon.begin(115200);
    delay(1000);
}

void setup()
{
    play_startup_Led_Blink();

    init_serial();

    init_leds();

    init_modem();

    sensors_init();
}

void updateSerial()
{
    while (SerialAT.available())
    {
        size_t ser_in_size = SerialAT.read(ser_buffer, SERIAL_RELAY_BUFFER_SIZE);
        Serial.print("GSM: ");
        Serial.write(ser_buffer, ser_in_size); // Forward what Software Serial received to Serial Port
    }
    while (Serial.available())
    {
        size_t ser_in_size = Serial.read(ser_buffer, SERIAL_RELAY_BUFFER_SIZE);
        Serial.print("Send to GSM: ");
        Serial.write(ser_buffer, ser_in_size);
        SerialAT.write(ser_buffer, ser_in_size); // Forward what Serial received to Software Serial Port
    }
    delay(2000);
}

void handleSerial()
{
    if (Serial.available())
    {
        char a = Serial.peek();
        if (a == 's')
        {
            manual_at_mode = !manual_at_mode;
            Serial.printf("Manual AT mode: %d\n", manual_at_mode);
        }
    }
    updateSerial();
}

int echo_gms_cmds = 1;

void sendCmd(const char *cmd)
{
    if (echo_gms_cmds)
    {
        Serial.print("Send to GSM: ");
        Serial.println(cmd);
        // Serial.flush();
    }
    SerialAT.println(cmd);
    SerialAT.flush();
    delay(1000);
}

/**
 * 1  Error
 * 0 OK
 *
 *  */
int waitForResp(const char *resp, unsigned int timeout)
{
    int len = strlen(resp);
    int sum = 0;
    unsigned long timerStart, timerEnd;
    timerStart = millis();

    while (1)
    {
        if (SerialAT.available())
        {
            char c = SerialAT.read();
            Serial.print(c);
            sum = (c == resp[sum] || resp[sum] == 'X') ? sum + 1 : 0;
            if (sum == len)
                break;
        }
        timerEnd = millis();
        if (timerEnd - timerStart > timeout)
        {
            return 1;
        }
    }

    while (SerialAT.available())
    {
        SerialAT.read();
    }

    Serial.println();

    return 0;
}

int sendCmdAndWaitForResp(const char *cmd, const char *resp, unsigned timeout)
{
    sendCmd(cmd);
    return waitForResp(resp, timeout);
}

void send_sms_data()
{
    int status = modem.sendSMS(SMS_TARGET2, "Temp: " + String(readTemperature())); // Setzt CMGF=1 -> Textmodus
    Serial.printf("SMS sent: %s\n", status ? "OK" : "fail");
}

void gsm_send_serial(String cmd)
{
    Serial.println(cmd);
    SerialAT.println(cmd);
    delay(1000);
    updateSerial();
}

#define STD_WAIT_TIME 4000

void end_http()
{
    sendCmdAndWaitForResp("AT+HTTPTERM", "OK", STD_WAIT_TIME);
    sendCmdAndWaitForResp("AT+SAPBR=0,1", "OK", STD_WAIT_TIME);
}

int send_network_data()
{
    Serial.println(" --- Start GPRS & HTTP --- ");

    float temp = readTemperature();
    char data[100];
    int len = snprintf(data, 100, "{\"temp\":\"%f\"}", temp);
    int resp = 0;
    resp += sendCmdAndWaitForResp("AT+CREG?", "+CREG: 0,5", STD_WAIT_TIME);
    resp += sendCmdAndWaitForResp("AT+SAPBR=1,1", "OK", STD_WAIT_TIME);
    resp += sendCmdAndWaitForResp("AT+SAPBR=2,1", "+SAPBR: 1,1,\"X.X.X.X\"", STD_WAIT_TIME);
    resp += sendCmdAndWaitForResp("AT+SAPBR=4,1", "OK", STD_WAIT_TIME);
    resp += sendCmdAndWaitForResp("AT+HTTPINIT", "OK", STD_WAIT_TIME);
    resp += sendCmdAndWaitForResp("AT+HTTPPARA=CID,1", "OK", STD_WAIT_TIME);
    resp += sendCmdAndWaitForResp("AT+HTTPPARA=URL,http://jserv.ddns.net:8080", "OK", STD_WAIT_TIME);
    resp += sendCmdAndWaitForResp("AT+HTTPPARA=CONTENT,application/json", "OK", STD_WAIT_TIME);
    resp += sendCmdAndWaitForResp("AT+HTTPDATA=100,5000", "DOWNLOAD", STD_WAIT_TIME);
    resp += sendCmdAndWaitForResp(data, "OK", STD_WAIT_TIME);
    resp += sendCmdAndWaitForResp("AT+HTTPACTION=1", "+HTTPACTION: 1,200", STD_WAIT_TIME);
    resp += sendCmdAndWaitForResp("AT+HTTPREAD", "OK", STD_WAIT_TIME);
    resp += sendCmdAndWaitForResp("AT+HTTPTERM", "OK", STD_WAIT_TIME);
    resp += sendCmdAndWaitForResp("AT+SAPBR=0,1", "OK", STD_WAIT_TIME);

    // TODO: Angeben ob Response Compare erfolgreich, und response gut formatiert ausgeben

    // if (resp)
    //     end_http();
    return resp;
}

void old_send_data()
{
    Serial.println(" --- Start GPRS & HTTP --- ");

    float temp = readTemperature();
    char data[100];
    int len = snprintf(data, 100, "{\"temp\":\"%f\"}", temp);
    gsm_send_serial("AT+CREG?");
    gsm_send_serial("AT+SAPBR=1,1");
    gsm_send_serial("AT+SAPBR=2,1");
    gsm_send_serial("AT+SAPBR=4,1");
    gsm_send_serial("AT+HTTPINIT");
    gsm_send_serial("AT+HTTPPARA=CID,1");
    gsm_send_serial("AT+HTTPPARA=URL," + String(server));
    gsm_send_serial("AT+HTTPPARA=CONTENT,application/json");
    gsm_send_serial("AT+HTTPDATA=" + String(len) + ",5000");
    gsm_send_serial(data);
    gsm_send_serial("AT+HTTPACTION=1");
    gsm_send_serial("AT+HTTPREAD");
    gsm_send_serial("AT+HTTPTERM");
    gsm_send_serial("AT+SAPBR=0,1");
}

int serial_confirm_send()
{
    Serial.println("Stopped. Data ready to Send.\nWaiting for serial Input.\n");
    while (!Serial.available())
    {
        delay(1000);
    }
    handleSerial();
    if (manual_at_mode)
        return false;
    Serial.println("Sending Data:");
    return true;
}

void send_data(){
    if (!serial_confirm_send())
        return;

    if (!send_network_data())
        send_sms_data();
}

void loop()
{
    handleSerial();
    if (manual_at_mode)
        return;

    printBattStatus();
    printGSMNetworkStatus();

    sensors_loop();

    send_data();
}