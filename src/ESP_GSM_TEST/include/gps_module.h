#ifndef GPS_MODULE_H
#define GPS_MODULE_H

#include <Arduino.h>
#include <TinyGPS++.h>
#include <string.h>

void gps_setup();
void gps_loop();
String getLocationString();
String getAltitudeString();
String gps_string();


#endif