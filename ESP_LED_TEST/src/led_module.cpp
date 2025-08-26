#include <Arduino.h>
#include <FastLED.h>
#include <bits/stdc++.h>
#include "led_module.h"

// Static vectors for brightness masks
std::vector<int> bm(NUM_LEDS);
std::vector<int> lh(NUM_LEDS);

struct ColorState {
    uint8_t hue = 96;
    uint8_t sat = SATURATION;
} currentColor;


void led_module::begin() {
    gen_bm();
    gen_lh();
    FastLED.addLeds<WS2812B, DATA_PIN>(leds, NUM_LEDS);
}

void led_module::loop() {
    circularAnimation(bar);
    circularAnimation(light);
    breatheAnimation();
    pulseAnimation();
    staticBrightness();
}

void led_module::setColor(uint8_t hue, uint8_t sat) {
    currentColor.hue = hue;
    currentColor.sat = sat;
}

void led_module::updateColor() {
    static CHSV startColor = CHSV(currentColor.hue, currentColor.sat, 255);
    static CHSV targetColor = CHSV(random8(), currentColor.sat, 255);
    static uint8_t blendAmount = 0;
    static bool forward = true;
    static unsigned long lastUpdate = 0;
    const unsigned long fadeInterval = 20 * TIMEFACTOR;
    const uint8_t blendStep = 1;

    if (millis() - lastUpdate > fadeInterval) {
        lastUpdate = millis();

        CHSV blended = blend(startColor, targetColor, blendAmount);
        currentColor.hue = blended.hue;
        currentColor.sat = blended.sat;

        if (forward) {
            if (blendAmount < 255 - blendStep) {
                blendAmount += blendStep;
            } else {
                if (random8() < 128) {
                    forward = false;
                }
            }
        } else {
            if (blendAmount > blendStep) {
                blendAmount -= blendStep;
            } else {
                if (random8() < 128) {
                    forward = true;
                }
            }
        }
    }
}

void led_module::rotateArr(std::vector<int>& arr) {
    int n = arr.size();
    int last = arr[n - 1];
    for (int j = n - 1; j > 0; j--) {
        arr[j] = arr[j - 1];
    }
    arr[0] = last;
}

void led_module::evil_iota(std::vector<int>::iterator first, std::vector<int>::iterator last, int value) {
    for (; first != last; ++first, value -= 2) {
        if (value <= MIN_BRIGHTNESS - 1) continue;
        *first = value;
    }
}

void led_module::my_iota(std::vector<int>::iterator first, std::vector<int>::iterator last, int value) {
    for (; first != last; ++first, value += 2) {
        if (value >= MAX_BRIGHTNESS + 1) {
            *first = MAX_BRIGHTNESS;
            continue;
        }
        *first = value;
    }
}

void led_module::gen_bm() {
    int section1, section2;
    section1 = NUM_LEDS / 6;
    section2 = NUM_LEDS - section1;
    my_iota(bm.begin(), bm.begin() + section2 / 2, MIN_BRIGHTNESS);
    evil_iota(bm.begin() + section2 / 2, bm.begin() + section2, bm[(section2 / 2) - 1]);
    std::fill(bm.begin() + section2, bm.end(), 0);
}

void led_module::gen_lh() {
    int section_size = NUM_LEDS / 4;
    int cmp = NUM_LEDS % 4;
    int section1 = section_size;
    int section2 = (cmp == 2 || cmp == 3) ? section_size + 1 : section_size;

    std::fill(lh.begin(), lh.begin() + section1, MAX_BRIGHTNESS);
    std::fill(lh.begin() + section1, lh.begin() + section1 + section2, 0);
    std::fill(lh.begin() + section1 + section2, lh.begin() + section1 * 2 + section2, MAX_BRIGHTNESS);
    std::fill(lh.begin() + section1 * 2 + section2, lh.end(), 0);
}

void led_module::circularAnimation(circularType animation) {
    static int frame = 0;
    static circularType last_anim = bar;
    std::vector<int>& tempbar = (animation == bar ? bm : lh);

    if (animation != last_anim) {
        frame = 0;
        last_anim = animation;
    }

    if (frame > 288) return;

    if (millis() - last_time_ms < 20 * TIMEFACTOR) return;
    last_time_ms = millis();

    updateColor();
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CHSV(currentColor.hue, currentColor.sat, tempbar[i]);
    }
    FastLED.show();
    rotateArr(tempbar);
    frame++;
}

void led_module::breatheAnimation() {
    static int a = MIN_BRIGHTNESS;
    static bool up = true;
    static int cycle = 0;

    if (cycle >= 4) return;

    if (millis() - last_time_ms < 20 * TIMEFACTOR) return;
    last_time_ms = millis();

    updateColor();
    for (int dot = 0; dot < NUM_LEDS; dot++) {
        leds[dot] = CHSV(currentColor.hue, currentColor.sat, a);
    }
    FastLED.show();

    if (up) {
        a++;
        if (a >= MAX_BRIGHTNESS) {
            up = false;
            cycle++;
        }
    } else {
        a--;
        if (a <= MIN_BRIGHTNESS) {
            up = true;
            cycle++;
        }
    }
}

void led_module::pulseAnimation() {
    static int phase = 0;
    static int a = 65;
    static int i = 0;
    static int j = 0;

    if (j >= 2) return;

    if (phase == 0) {
        if (millis() - last_time_ms < 1000 * TIMEFACTOR) return;
        last_time_ms = millis();
        updateColor();
        for (int dot = 0; dot < NUM_LEDS; dot++) {
            leds[dot] = CHSV(currentColor.hue, currentColor.sat, a);
        }
        FastLED.show();
        phase = 1;
        i = a;
    } else if (phase == 1) {
        if (millis() - last_time_ms < 5 * TIMEFACTOR) return;
        last_time_ms = millis();
        updateColor();
        for (int dot = 0; dot < NUM_LEDS; dot++) {
            leds[dot] = CHSV(currentColor.hue, currentColor.sat, i);
        }
        FastLED.show();
        i += 2;
        if (i >= MAX_BRIGHTNESS) {
            phase = 2;
            i = MAX_BRIGHTNESS;
        }
    } else if (phase == 2) {
        if (millis() - last_time_ms < 5 * TIMEFACTOR) return;
        last_time_ms = millis();
        updateColor();
        for (int dot = 0; dot < NUM_LEDS; dot++) {
            leds[dot] = CHSV(currentColor.hue, currentColor.sat, i);
        }
        FastLED.show();
        i -= 2;
        if (i <= a) {
            phase = 0;
            j++;
        }
    }
}

void led_module::staticBrightness() {

    if (millis() - last_time_ms < 1500 * TIMEFACTOR) return;
        last_time_ms = millis();

    
    for (int dot = 0; dot < NUM_LEDS; dot++) {
        updateColor();
        leds[dot] = CHSV(currentColor.hue, currentColor.sat, MAX_BRIGHTNESS);
        
    }
    FastLED.show();
}