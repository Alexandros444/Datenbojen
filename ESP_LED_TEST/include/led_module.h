#ifndef LED_MODULE_H
#define LED_MODULE_H

#include <FastLED.h>


#define NUM_LEDS 144
#define DATA_PIN 4
#define COLOUR 96
#define SATURATION 255
#define MAX_BRIGHTNESS 100
#define MIN_BRIGHTNESS 32
#define TIMEFACTOR 2

#include <bits/stdc++.h>

enum circularType {
    bar,
    light
};

class led_module {
private:
    CRGB leds[NUM_LEDS];
    void gen_bm();
    void gen_lh();
    void rotateArr(std::vector<int>& arr);
    void my_iota(std::vector<int>::iterator first, std::vector<int>::iterator last, int value);
    void evil_iota(std::vector<int>::iterator first, std::vector<int>::iterator last, int value);
    unsigned long last_time_ms = 0;
    int progress = 0;
    void inc_progress() {progress = (progress + 1) % NUM_LEDS;};
    int anim_func = 0;
    bool anim_done = false;
public:
    void begin();
    void loop();
    bool staticBrightness();
    bool circularAnimation(circularType animation);
    bool breatheAnimation();
    bool pulseAnimation();
    void wipeStrip();
    void setColor(uint8_t hue, uint8_t sat);
    void updateColor();
    bool wait_ms(unsigned long ms);
    void reset_anim_progress();
    void fill_leds_show(CHSV color);
};


#endif