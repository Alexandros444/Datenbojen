#include "debug_info.h"

void print_info(){
    // print esp FREE RAM
    Serial.printf("Free RAM: %d\n", ESP.getFreeHeap());
    // pritn esp FREE Flash
    Serial.printf("Free Flash: %d\n", ESP.getFreeSketchSpace());
    // print esp SKETCH SIZE
    Serial.printf("Sketch Size: %d\n", ESP.getSketchSize());
}