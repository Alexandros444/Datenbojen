#ifndef webserver_h
#define webserver_h

#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "gsm_module.h"

void webserverSetup();
void webserverLoop();



#endif