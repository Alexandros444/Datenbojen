#ifndef YAZDAN_CODE_H
#define YAZDAN_CODE_H

#include "GravityTDS.h"
#include "DFRobot_PH.h"

void sensors_init();
void sensors_loop();
float readTemperature();
float readTurbidity(float temperature);

extern GravityTDS gravityTds;

#endif