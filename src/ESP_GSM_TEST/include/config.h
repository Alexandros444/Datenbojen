#ifndef CONFIG_H
#define CONFIG_H

#include <Wire.h>

#define I2C_SDA 21
#define I2C_SCL 22

#define DEBUG_PRINT 1 // Remove to disable debug prints

#ifdef DEBUG_PRINT
    #define DPRINT(...) Serial.print(__VA_ARGS__)
    #define DPRINTLN(...) Serial.println(__VA_ARGS__)
    #define DPRINTF(...) Serial.printf(__VA_ARGS__)
    #define DPRINT_IF(condition, ...) if (condition) Serial.print(__VA_ARGS__)
    #define DPRINTLN_IF(condition, ...) if (condition) Serial.println(__VA_ARGS__)
    #define DPRINTF_IF(condition, ...) if (condition) Serial.printf(__VA_ARGS__)
#else
    #define DPRINT(...)
    #define DPRINTLN(...)
    #define DPRINTF(...)
    #define DPRINT_IF(condition, ...)
    #define DPRINTLN_IF(condition, ...)
    #define DPRINTF_IF(condition, ...)
#endif

#endif