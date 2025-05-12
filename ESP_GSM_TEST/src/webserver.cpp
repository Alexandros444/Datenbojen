#include "webserver_util.h"

// Replace with your desired AP credentials
const char* ssid = "ESP32-GSM-AP";
const char* password = "12345678";

WebServer server(80);

// Dummy GSM status variables
String networkStatus = "Disconnected";
String gprsStatus = "Disconnected";
String signalQuality = "-inf dBm";
String batteryVoltage = "0.0 V";
String registrationStatus = "Unknown";
String location = "Unknown";
String modemInfo = "Unknown";
String operatorName = "no Operator";



// Dummy SMS sending function
void sendSMS() {
    // Replace this with your GSM library's send SMS code
    Serial.println("Not Implemented: Sending SMS");
}

void updateGSMStatus() {

    statusInfo status = getStatusInfo();

    networkStatus = status.isNetworkConnected ? "Connected" : "Disconnected";
    gprsStatus = status.isGprsConnected ? "Connected" : "Disconnected";
    signalQuality = status.signalQuality;
    batteryVoltage = String(status.batt) + " mV";
    registrationStatus = status.regStatus;
    location = status.loc;
    operatorName = status.operatorName;   
    modemInfo = status.modemInfo;

}

void https_get() {
    perform_get_https("www.baidu.com");
}

void https_post() {
    Serial.println("Not Implemented: POST");
}


// HTML page
String htmlPage() {
    String page = "<!DOCTYPE html><html><head><title>ESP32 GSM Status</title></head><body>";
    page += "<h1>GSM Status</h1>";

    page += "<ul>";
    page += "<li>Network Status: " + networkStatus + "</li>";
    page += "<li>GPRS Status: " + gprsStatus + "</li>";
    page += "<li>Signal Quality: " + signalQuality + "</li>";
    page += "<li>Battery Voltage: " + batteryVoltage + "</li>";
    page += "<li>Registration Status: " + registrationStatus + "</li>";
    page += "<li>Location: " + location + "</li>";
    page += "<li>Operator Name: " + operatorName + "</li>";
    page += "<li>Modem Info: " + modemInfo + "</li>";
    page += "</ul>";

    // page += "<form action=\"/send_sms\" method=\"POST\">";
    // page += "<button type=\"submit\">Send SMS</button>";
    // page += "</form>";

    page += "<form action=\"/https_get\" method=\"POST\">";
    page += "<button type=\"submit\">HTTPS Get</button>";
    page += "</form>";

    // page += "<form action=\"/https_post\" method=\"POST\">";
    // page += "<button type=\"submit\">HTTPS Post</button>";
    // page += "</form>";

    page += "</body></html>";
    return page;
}

void handleRoot() {
    updateGSMStatus();
    server.send(200, "text/html", htmlPage());
}

void handleSendSMS() {
    sendSMS();
    server.sendHeader("Location", "/");
    server.send(303);
}

void handleGetHttps() {
    https_get();
    server.sendHeader("Location", "/");
    server.send(303);
}

void handlePostHttps() {
    https_post();
    server.sendHeader("Location", "/");
    server.send(303);
}

void webserverSetup() {
    Serial.println("Setting up AP and Web Server...");
    WiFi.mode(WIFI_AP);
    // Start WiFi AP
    WiFi.softAP(ssid, password);
    IPAddress Ip(192, 168, 0, 1);
    IPAddress NMask(255, 255, 255, 0);
    WiFi.softAPConfig(Ip, Ip, NMask);
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());


    server.on("/", HTTP_GET, handleRoot);
    server.on("/send_sms", HTTP_POST, handleSendSMS);
    server.on("/https_get", HTTP_POST, handleGetHttps);
    server.on("/https_post", HTTP_POST, handlePostHttps);


    server.begin();
    Serial.println("Web Server setup done\n");
}

void webserverLoop() {
    server.handleClient();
}