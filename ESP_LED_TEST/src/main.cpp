#include <Arduino.h>
#include <FastLED.h>
#include <bits/stdc++.h>
#include "led_module.h"


// put function declarations here:

led_module led;

void setup() {
  // put your setup code here, to run once:
  led.begin();
}


void loop() {
  led.loop();
}



// put function definitions here:

// For reference
// void colourFadeTest_no_delay(){
//     // Overflow to zero wenn i > 255
//     static uint8_t i = 0;
//     static unsigned long lastUpdate = 0;
//     if (millis() - lastUpdate < 20*TIMEFACTOR)
//         return;
//     lastUpdate = millis();
//     CHSV first = CHSV(96,SATURATION,MAX_BRIGHTNESS);
//     CHSV second = CHSV(72,SATURATION,MAX_BRIGHTNESS);
//     CHSV target;
//     for (;i<255;i++){
//         target = blend(first,second,i);
//         for(int dot = 0; dot < NUM_LEDS; dot++){
//             leds[dot] = target;
//         }
//         FastLED.show();
//         return;
//     }
//     // i manuell zurücksetzen, weil loop ja bei letzten wert weitermacht
//     i = 0;
// }