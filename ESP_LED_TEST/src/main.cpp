#include <Arduino.h>
#include <FastLED.h>
#include <bits/stdc++.h>

#define NUM_LEDS 144
#define DATA_PIN 4 
#define COLOUR 96
#define SATURATION 255
#define MAX_BRIGHTNESS 128
#define MIN_BRIGHTNESS 32
#define TIMEFACTOR 8


enum circularType {
    bar,
    light
};
CRGB leds[NUM_LEDS];
// static vector<int> barmask = {32,34,36,38,40,42,44,46,48,50,52,54,56,58,60,62,64,66,68,70,72,74,76,78,80,82,84,86,88,90,92,94,96,98,100,102,104,106,108,110,112,114,116,118,120,122,124,126,128,126,124,122,120,118,116,114,112,110,108,106,104,102,100,98,96,94,92,90,88,86,84,82,80,78,76,74,72,70,68,66,64,62,60,58,56,54,52,50,48,46,44,42,40,38,36,34,32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
// static vector<int> leuchtturm = {128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

std::vector<int> bm(NUM_LEDS);
std::vector<int> lh(NUM_LEDS);

struct ColorState {
    uint8_t hue = 96;
    uint8_t sat = SATURATION;
} currentColor;

// put function declarations here:
void rotateArr(std::vector<int>& arr);
void circularAnimation(circularType animation);
void breatheAnimation();
void pulseAnimation();
void staticBrightness();
void wipeStrip();
void colourFadeTest();
void gen_lh();
void gen_bm();
void setColor(uint8_t hue, uint8_t sat);
void updateColor();

void setup() {
  // put your setup code here, to run once:
  gen_bm();
  gen_lh();
  FastLED.addLeds<WS2812B, DATA_PIN>(leds, NUM_LEDS);
}


void loop() {
    circularAnimation(bar);
    // wipeStrip();
    circularAnimation(light);
    // wipeStrip();
    breatheAnimation();
    // wipeStrip();
    pulseAnimation();
    // wipeStrip();
    staticBrightness();
    // wipeStrip();
}


// put function definitions here:

void setColor(uint8_t hue, uint8_t sat) {
    currentColor.hue = hue;
    currentColor.sat = sat;
}

void updateColor() {
    // Smoothly blend between two colors over time, with possible direction reversal
    static CHSV startColor = CHSV(currentColor.hue, currentColor.sat, 255);
    static CHSV targetColor = CHSV(random8(), currentColor.sat, 255);
    static uint8_t blendAmount = 0;
    static bool forward = true; // true: start->target, false: target->start
    static unsigned long lastUpdate = 0;
    const unsigned long fadeInterval = 20 * TIMEFACTOR; // ms between blend steps
    const uint8_t blendStep = 1; // how much to increment/decrement blendAmount per step

    if (millis() - lastUpdate > fadeInterval) {
        lastUpdate = millis();

        // Blend from startColor to targetColor or reverse
        CHSV blended = blend(startColor, targetColor, blendAmount);
        currentColor.hue = blended.hue;
        currentColor.sat = blended.sat;

        if (forward) {
            if (blendAmount < 255 - blendStep) {
                blendAmount += blendStep;
            } else {
                // At target, randomly decide to reverse
                if (random8() < 128) {
                    forward = false; // reverse direction
                }
            }
        } else {
            if (blendAmount > blendStep) {
                blendAmount -= blendStep;
            } else {
                // At start, randomly decide to go forward again
                if (random8() < 128) {
                    forward = true; // go forward again
                }
            }
        }
    }
}


// Rotates the given array to the right by one position.
void rotateArr(std::vector<int>& arr) {
    int n = arr.size();
      
    // Right rotate the array by one position
    int last = arr[n - 1];
    for (int j = n - 1; j > 0; j--) {
        arr[j] = arr[j - 1];
    }
    arr[0] = last;
}

template<class ForwardIt, class T>
constexpr // since C++20
void evil_iota(ForwardIt first, ForwardIt last, T value)
{
    for (; first != last; ++first, value -= 2){
        if(value <= MIN_BRIGHTNESS-1)continue;
        *first = value;
    }
        
}
template<class ForwardIt, class T>
constexpr // since C++20
void my_iota(ForwardIt first, ForwardIt last, T value)
{
    for (; first != last; ++first, value += 2){
        if(value >= MAX_BRIGHTNESS+1) {
            *first = MAX_BRIGHTNESS;
            continue;
        }
        *first = value;
    }
}

/**
 * @brief Generates a brightness mask for the bar animation.
 *
 * The brightness mask is a vector of brightness values that will be used to
 * control the brightness of the LEDs during the bar animation. The mask is
 * divided into three sections. The first section is a ramp from MIN_BRIGHTNESS
 * to MAX_BRIGHTNESS. The second section is a ramp from MAX_BRIGHTNESS back down
 * to MIN_BRIGHTNESS. The third section is a constant value of 0.
 */
void gen_bm(){
    int section1, section2;
    section1 = NUM_LEDS/6;
    section2 = NUM_LEDS-section1;
    my_iota(bm.begin(), bm.begin()+section2/2, MIN_BRIGHTNESS);
    evil_iota(bm.begin()+section2/2, bm.begin()+section2, bm[(section2/2)-1]);
    std::fill(bm.begin()+section2, bm.end(), 0);
}

/**
 * @brief Generates a brightness mask for the lighthouse animation.
 *
 * The lighthouse mask is a vector of brightness values that will be used to
 * control the brightness of the LEDs during the lighthouse animation. The mask
 * is divided into four sections. The first and third sections have a brightness
 * of MAX_BRIGHTNESS, while the second and fourth sections have a brightness of 0.
 */
void gen_lh() {
    int section_size = NUM_LEDS / 4;
    int cmp = NUM_LEDS % 4;
    int section1 = section_size;
    int section2 = (cmp == 2 || cmp == 3) ? section_size + 1 : section_size;

    std::fill(lh.begin(), lh.begin() + section1, MAX_BRIGHTNESS);
    std::fill(lh.begin() + section1, lh.begin() + section1 + section2, 0);
    std::fill(lh.begin() + section1 + section2, lh.begin() + section1 * 2 + section2, MAX_BRIGHTNESS);
    std::fill(lh.begin() + section1 * 2 + section2, lh.end(), 0);
}

void colourFadeTest(){
    CHSV first = CHSV(96,SATURATION,MAX_BRIGHTNESS);
    CHSV second = CHSV(72,SATURATION,MAX_BRIGHTNESS);
    CHSV target;
    for (int i=0;i<255;i++){
        target = blend(first,second,i);
        for(int dot = 0; dot < NUM_LEDS; dot++){
            leds[dot] = target;
        }
        FastLED.show();
        delay(20*TIMEFACTOR);
    }
}

void staticBrightness(){
    for (int i=0;i<2;i++){
        updateColor();
        for(int dot = 0; dot < NUM_LEDS; dot++){
            leds[dot] = CHSV(currentColor.hue, currentColor.sat, MAX_BRIGHTNESS);
        }
        FastLED.show();
        delay(1500*TIMEFACTOR);
    }
}

void wipeStrip(){
    fadeToBlackBy(leds,NUM_LEDS,128);
    FastLED.show();
    delay(10*TIMEFACTOR);
    fadeToBlackBy(leds,NUM_LEDS,255);
    FastLED.show();
    delay(500*TIMEFACTOR);
}

/**
 * @brief Runs a circular animation on an LED strip.
 *
 * This function animates the LEDs in a circular pattern by rotating the brightness or color values
 * in the specified array (either 'bm' or 'lh', depending on the animation type). For each frame,
 * it updates the color, sets the LED values, displays them, rotates the array, and waits for a short delay.
 *
 * @param animation The type of circular animation to perform (e.g., 'bar' or another type).
 *
 * Dependencies:
 * - Uses global variables: NUM_LEDS, leds, currentColor, bm, lh, TIMEFACTOR.
 * - Uses external functions: updateColor(), rotateArr().
 * - Uses FastLED library for LED control.
 */
void circularAnimation(circularType animation){
    std::vector<int>& tempbar = (animation == bar ? bm : lh);
    for (int frame = 0; frame <= 288; frame++) {
        updateColor(); // Optionally update color each frame
        for (int i = 0; i < NUM_LEDS; i++) {
            leds[i] = CHSV(currentColor.hue, currentColor.sat, tempbar[i]);
        }
        FastLED.show();
        rotateArr(tempbar);
        delay(20*TIMEFACTOR);
    }
}

/**
 * @brief Runs a "breathe" animation on the LED strip.
 *
 * Gradually increases and decreases the brightness of all LEDs to create a smooth breathing effect.
 * The animation cycles twice, fading in from MIN_BRIGHTNESS to MAX_BRIGHTNESS and then fading out.
 * Optionally updates the color on each frame using updateColor().
 *
 * Uses the global variables:
 * - leds[]: Array of LED objects.
 * - NUM_LEDS: Number of LEDs in the strip.
 * - MIN_BRIGHTNESS: Minimum brightness value.
 * - MAX_BRIGHTNESS: Maximum brightness value.
 * - currentColor: Struct containing current hue and saturation.
 * - TIMEFACTOR: Multiplier for controlling animation speed.
 *
 * Dependencies:
 * - FastLED library for controlling LEDs.
 * - CHSV color model for setting LED colors.
 */
void breatheAnimation(){
    for (int i = 0; i < 2; i++) {
        for (int a = MIN_BRIGHTNESS; a < MAX_BRIGHTNESS; a++) {
            updateColor(); // Optionally update color each frame
            for (int dot = 0; dot < NUM_LEDS; dot++) {
                leds[dot] = CHSV(currentColor.hue, currentColor.sat, a);
            }
            FastLED.show();
            delay(20*TIMEFACTOR);
        }
        for (int b = MAX_BRIGHTNESS; b > MIN_BRIGHTNESS; b--) {
            updateColor(); // Optionally update color each frame
            for (int dot = 0; dot < NUM_LEDS; dot++) {
                leds[dot] = CHSV(currentColor.hue, currentColor.sat, b);
            }
            FastLED.show();
            delay(20*TIMEFACTOR);
        }
    }
}

/**
 * @brief Runs a pulse animation on the LED strip.
 *
 * The animation smoothly increases and decreases the brightness of all LEDs
 * between a mid-level brightness and the maximum brightness, creating a pulsing effect.
 * The color can be updated on each frame via the updateColor() function.
 * The animation repeats for a fixed number of cycles.
 *
 * Timing and brightness levels are controlled by the constants:
 * - NUM_LEDS: Number of LEDs in the strip.
 * - MAX_BRIGHTNESS: Maximum brightness value for the LEDs.
 * - TIMEFACTOR: Multiplier to adjust animation speed.
 *
 * The function uses the FastLED library to update the LED colors and display changes.
 */
void pulseAnimation(){
    int midbright = 65;
    for (int j = 0; j < 2; j++) {
        updateColor(); // Optionally update color each frame
        for (int dot = 0; dot < NUM_LEDS; dot++) {
            leds[dot] = CHSV(currentColor.hue, currentColor.sat, midbright);
        }
        FastLED.show();
        delay(1000 * TIMEFACTOR);
        for (int i = 0; i < 2; i++) {
            for (int a = midbright; a < MAX_BRIGHTNESS; a += 2) {
                updateColor(); // Optionally update color each frame
                for (int dot = 0; dot < NUM_LEDS; dot++) {
                    leds[dot] = CHSV(currentColor.hue, currentColor.sat, a);
                }
                FastLED.show();
                delay(5*TIMEFACTOR);
            }
            for (int b = MAX_BRIGHTNESS; b > midbright; b -= 2) {
                updateColor(); // Optionally update color each frame
                for (int dot = 0; dot < NUM_LEDS; dot++) {
                    leds[dot] = CHSV(currentColor.hue, currentColor.sat, b);
                }
                FastLED.show();
                delay(5*TIMEFACTOR);
            }
        }
    }
}