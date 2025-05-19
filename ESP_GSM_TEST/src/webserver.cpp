#include "webserver_util.h"
#include "gps_module.h"

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
String networkTime = "Unknown";
String locationString = "Unknown";
String altitudeString = "Unknown";

String get_status = "-1";
String post_status = "-1";

String post_url = "http://jserv.ddns.net:8080";


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
    networkTime = status.networkTime;

}


void updateGPSStatus() {
    locationString = gps_string();
    altitudeString = getAltitudeString();
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
    page += "<li>Network Time: " + networkTime + "</li>";
    page += "<li>GPS Location: " + locationString + "</li>";
    page += "<li>GPS Altitude: " + altitudeString + "</li>";
    page += "</ul>";

    // page += "<form action=\"/send_sms\" method=\"POST\">";
    // page += "<button type=\"submit\">Send SMS</button>";
    // page += "</form>";

    page += "<form action=\"/https_get\" method=\"POST\">";
    page += "<input type=\"text\" name=\"url\" placeholder=\"Enter Get URL\" required> ";
    page += "<button type=\"submit\">HTTPS Get</button>";
    page += " Status: " + get_status;
    page += "</form>";

    page += "<br>";

    page += "<form action=\"/https_post\" method=\"POST\">";
    // page += "<input type=\"text\" name=\"url\" placeholder=\"Enter Post URL\" required> ";
    page += "<input type=\"text\" name=\"data\" placeholder=\"Enter Post Data\" required> ";
    page += "<button type=\"submit\">HTTPS Post</button>";
    page += " Status: " + post_status;
    page += "</form>";

    // page += "<form action=\"/https_post\" method=\"POST\">";
    // page += "<button type=\"submit\">HTTPS Post</button>";
    // page += "</form>";

    

    page += "</body></html>";
    return page;
}

void handleRoot() {
    updateGSMStatus();
    updateGPSStatus();
    server.send(200, "text/html", htmlPage());
}

void handleSendSMS() {
    sendSMS();
    server.sendHeader("Location", "/");
    server.send(303);
}

void handleGetHttps() {
    String url = server.hasArg("url") ? server.arg("url") : "www.httpbin.org";
    int status = perform_get_https(url);
    get_status = status == 200 ? "Success: " + String(status) : "Failed: " + String(status);
    server.sendHeader("Location", "/");
    server.send(303);
}

void handlePostHttps() {
    // String url = server.hasArg("url") ? server.arg("url") : "www.httpbin.org";
    String data = server.hasArg("data") ? server.arg("data") : "null";
    int status = perform_post_https(post_url, data);
    post_status = status == 200 ? "Success: " + String(status) : "Failed: " + String(status);
    server.sendHeader("Location", "/");
    server.send(303);
}

void webserverSetup() {
    Serial.println("Setting up AP and Web Server...");
    WiFi.mode(WIFI_AP);
    // Start WiFi AP
    WiFi.softAP(ssid, password);
    IPAddress Ip(10, 0, 0, 1);
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