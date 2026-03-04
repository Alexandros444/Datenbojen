#include <Arduino.h>
#include <FastLED.h>
#include <bits/stdc++.h>
#include "led_module.h"

// Static vectors for brightness masks
std::vector<int> bm(NUM_LEDS);
std::vector<int> lh(NUM_LEDS);


void led_module::begin() {
    gen_bm();
    gen_lh();
    FastLED.addLeds<WS2812B, DATA_PIN>(leds, NUM_LEDS);
    FastLED.clear();
    fill_rainbow(leds, NUM_LEDS, 160, 20);
    FastLED.show();
}



void led_module::loop() {

    switch (anim_func){
    case 0:
    /* code */
        anim_done = breatheAnimation();
        break;
    // case 1:
    //     anim_done = circularAnimation(light);
    //     break;
    // case 2:
    //     anim_done = breatheAnimation();
    //     break;
    // case 3:
    //     anim_done = pulseAnimation();
    //     break;
    // case 4:
    //     anim_done = staticBrightness();
    //     break;
    default:
        anim_func %= 1;
        break;
    }

    if (anim_done){
        anim_func++;
        anim_done = false;
        reset_anim_progress();
    }
    
}

void led_module::rand_color_change(){
    static unsigned long last_change = 0;
    static int hue_dir = 1;
    static int sat_dir = 1;

    if (wait_ms(500)) return;

    if (random8() < 128){
        if (hue_dir && currentColor.hue < 255){
            currentColor.hue++;
        } else if (currentColor.hue > 0){
            currentColor.hue--;
        }        
    }

    if (random8() < 128){
        if (sat_dir && currentColor.sat < 255){
            currentColor.sat++;
        } else if (currentColor.sat > 0){
            currentColor.sat--;
        }        
    }

    if (random8() == 0)
        hue_dir = !hue_dir;

    if (random8() == 0)
        sat_dir = !sat_dir;

}

void led_module::setColor(uint8_t hue, uint8_t sat) {
    currentColor.hue = hue;
    currentColor.sat = sat;
}

void led_module::exhibitionColor(){
    static CHSV green = CHSV(0, currentColor.sat, 255);
    static CHSV red = CHSV(96, currentColor.sat, 255);
    static CHSV yellow = CHSV(60, currentColor.sat, 255);
    static CHSV orange = CHSV(80, currentColor.sat, 255);
    static CHSV blue = CHSV(160, currentColor.sat, 255);
    static int blendercounter = 0; 
    static uint8_t stepcounter = 0; 
    static unsigned long lastUpdate = 0;
    const unsigned long fadeInterval = 100;


    if (millis() - lastUpdate > fadeInterval) {
        lastUpdate = millis();

        if (blendercounter == 0) { 
            switch (stepcounter%4)
            {
            case 0:
                currentColor.hue = green.hue;
                currentColor.sat = green.sat;
                break;
            case 1:
                currentColor.hue = yellow.hue;
                currentColor.sat = yellow.sat;
                break;
            case 2:
                currentColor.hue = orange.hue;
                currentColor.sat = orange.sat;
                break;
            case 3:
                currentColor.hue = red.hue;
                currentColor.sat = red.sat;
            default:
                break;
            }
        }else{
            CHSV blended;
            switch (stepcounter%4)
            {
            case 0:
                blended = blend(green, yellow, ((256/30.0)*blendercounter));
                // blended = green;
                break;
            case 1:
                blended = blend(yellow, orange, ((256/30.0)*blendercounter));
                // blended = yellow;
                break;
            case 2:
                blended = blend(orange, red, ((256/30.0)*blendercounter));
                // blended = orange;
                break;
            case 3:
                blended = blend(red, green, ((256/30.0)*blendercounter));
                break;
            default:
                break;
            }
            currentColor.hue = blended.hue;
            currentColor.sat = blended.sat;
        }
        blendercounter++;
        if (blendercounter > 29){
            stepcounter++;
            blendercounter = 0;
        }
    }
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

bool led_module::circularAnimation(circularType animation) {
    std::vector<int>& tempbar = (animation == bar ? bm : lh);

    if (progress > 288) return true;

    if (wait_ms(20)) return false;

    updateColor();
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CHSV(currentColor.hue, currentColor.sat, tempbar[i]);
    }
    FastLED.show();
    rotateArr(tempbar);
    progress++;
    return false;
}

bool led_module::breatheAnimation() {
    static int a = MIN_BRIGHTNESS;
    static bool up = true;

    if (progress >= 4) return true;

    if (wait_ms(20)) return false;

    // updateColor();
    exhibitionColor();
    fill_solid(leds, NUM_LEDS, CHSV(currentColor.hue, currentColor.sat, a));
    FastLED.show();

    if (up) {
        a++;
        if (a >= MAX_BRIGHTNESS) {
            up = false;
            progress++;
        }
    } else {
        a--;
        if (a <= MIN_BRIGHTNESS) {
            up = true;
            progress++;
        }
    }
    return false;
}


bool led_module::pulseAnimation() {
    static int phase = 0;
    static int a = 65;
    static int i = 0;

    if (progress >= 2) {
        phase = 0;
        a = 65;
        i = 0;
        return true;
    }

    if (phase == 0) {
        if (wait_ms(1000)) return false;
        updateColor();
        fill_solid(leds, NUM_LEDS, CHSV(currentColor.hue, currentColor.sat, a));
        FastLED.show();
        phase = 1;
        i = a;
    } else if (phase == 1) {
        if (wait_ms(5)) return false;
        updateColor();
        fill_solid(leds, NUM_LEDS, CHSV(currentColor.hue, currentColor.sat, i));
        FastLED.show();
        
        i += 2;
        if (i >= MAX_BRIGHTNESS) {
            phase = 2;
            i = MAX_BRIGHTNESS;
        }
    } else if (phase == 2) {
        if (wait_ms(5)) return false;
        updateColor();
        fill_solid(leds, NUM_LEDS, CHSV(currentColor.hue, currentColor.sat, i));
        FastLED.show();
        i -= 2;
        if (i <= a) {
            phase = 0;
            progress++;
        }
    }
    return false;
}

bool led_module::staticBrightness() {    
    
    if (wait_ms(1500)) return false;
    
    if (progress) return true;

    for (int dot = 0; dot < NUM_LEDS; dot++) {
        updateColor();
        leds[dot] = CHSV(currentColor.hue, currentColor.sat, MAX_BRIGHTNESS);
    }
    FastLED.show();

    progress++;

    return false;
}


bool led_module::wait_ms(unsigned long ms){
    if (millis() - last_time_ms < ms * TIMEFACTOR) 
        return true;
    last_time_ms = millis();
    return false;
}

void led_module::reset_anim_progress(){
// last_time_ms auf 0 gesetzt, damit immer die nächste Anim immidiately startet.
    last_time_ms = 0;
    progress = 0;
}