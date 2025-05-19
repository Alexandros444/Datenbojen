// ESP32 + Neo-6M GPS communication using TinyGPS++
#include "gps_module.h"

TinyGPSPlus gps;
#define GPSSerial Serial2

#define GPS_BAUD 9600
#define GPS_RX_PIN 14
#define GPS_TX_PIN 12

void gps_setup() {
    GPSSerial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
}

String gps_string() {
    String gps_data = "";
    gps_data += "Location: ";
    if (gps.location.isValid())
        gps_data += String(gps.location.lat(), 6) + "," + String(gps.location.lng(), 6);
    else
        gps_data += "INVALID";

    gps_data += "  Date/Time: ";
    if (gps.date.isValid())
        gps_data += String(gps.date.day()) + "." + String(gps.date.month()) + "." + String(gps.date.year());
    else
        gps_data += "INVALID";

    gps_data += "  ";

    if (gps.time.isValid())
        gps_data += String(gps.time.hour()) + ":" + String(gps.time.minute()) + ":" + String(gps.time.second());
    else
        gps_data += "INVALID";

    gps_data += "\n";
    return gps_data;
}

void gps_loop() {
    while (GPSSerial.available() > 0)
        gps.encode(GPSSerial.read());

    static bool msg = false;
    if (!msg && millis() > 5000 && gps.charsProcessed() < 10) {
        msg = true;
        Serial.println(F("No GPS detected: check wiring."));
    }
}

String getLocationString() {
    if (gps.location.isValid()) {
        String loc = String(gps.location.lat(), 6) + "," + String(gps.location.lng(), 6);
        return loc;
    }
    else {
        return "No Location";
    }
}

String getAltitudeString() {
    if (gps.altitude.isValid()) {
        String alt = String(gps.altitude.meters(), 2);
        return alt;
    }
    else {
        return "No Altitude";
    }
}
