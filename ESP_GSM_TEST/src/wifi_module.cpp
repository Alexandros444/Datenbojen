#include "wifi_module.h"

void wifi_module::begin(sensors_module* sensors, gsm_module* gsm) {
    this->gsm = gsm;
    this->sensors = sensors;
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");
}

int wifi_module::sendData() {
    Serial.printf("Sending data to %s\n", serverName.c_str());
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(serverName);
        http.addHeader("Content-Type", "application/json");

        sensors->print(); // Update sensor readings

        StaticJsonDocument<200> jsonDoc;

        jsonDoc["ADC0"] = sensors->adc->readInt(0);
        jsonDoc["ADC1"] = sensors->adc->readInt(1);
        jsonDoc["ADC2"] = sensors->adc->readInt(2);
        jsonDoc["ADC3"] = sensors->adc->readInt(3);
        jsonDoc["Temp C"] = sensors->temperature;
        jsonDoc["TDS ppm"] = sensors->tdsValue;
        jsonDoc["Current_mA"] = sensors->read_Current_mA();

        
        statusInfo gsmStatus = gsm->getStatusInfo();
        jsonDoc["GPRS"] = gsmStatus.isGprsConnected;
        jsonDoc["Network"] = gsmStatus.isNetworkConnected;
        jsonDoc["Signal"] = gsmStatus.signalQuality;
        jsonDoc["Battery"] = gsmStatus.batt;
        jsonDoc["Reg Status"] = gsmStatus.regStatus;
        jsonDoc["Location"] = gsmStatus.loc;
        jsonDoc["Operator"] = gsmStatus.operatorName;
        jsonDoc["Modem"] = gsmStatus.modemInfo;
        jsonDoc["Time"] = gsmStatus.networkTime;

        String requestBody;
        serializeJson(jsonDoc, requestBody);

        int httpResponseCode = http.POST(requestBody);

        if (httpResponseCode == 200) {
            String response = http.getString();
            Serial.println("HTTP Response code: " + String(httpResponseCode));
            Serial.println("Response from server: " + response);
            http.end();
            return 0;
        } else if (httpResponseCode > 0) {
            String response = http.getString();
            Serial.println("HTTP Response code: " + String(httpResponseCode));
            Serial.println("Response from server: " + response);
            http.end();
            return -3;    
        } else {
            Serial.println("Error on sending POST: " + String(httpResponseCode));
            http.end();
            return -2;
        }

        http.end();
    } else {
        Serial.println("WiFi not connected");
        return -1;
    }
}