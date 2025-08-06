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
    // TODO: KA compiled nicht anton fix it, wenn du unbedingt mit so high level C arbeiten musst lol
    // std::vector<int> bm{ std::vector<int>(NUM_LEDS) };
    // std::vector<int> lh{ std::vector<int>(NUM_LEDS) };
    // void generate_lighthouse_brightness_mask();
    // void generate_bar_brightness_mask();
    void rotateArr(std::vector<int>& arr);
    // template<class ForwardIt, class T>
    // constexpr // since C++20
    // void my_iota(ForwardIt first, ForwardIt last, T value);
    // template<class ForwardIt, class T>
    // constexpr // since C++20
    // void evil_iota(ForwardIt first, ForwardIt last, T value);
    unsigned long last_time_ms = 0;
    int progress = 0;
    void inc_progress() {progress = (progress + 1) % NUM_LEDS;};
public:
    void begin();
    void loop();
    void staticBrightness();
    // void circularAnimation(circularType animation);
    void breatheAnimation();
    void pulseAnimation();
    void wipeStrip();
    void colourFadeTest();
    void clear(){wipeStrip();};
    void example_anim_no_dly();
};


#endif