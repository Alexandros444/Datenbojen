#include <Arduino.h>
#include <FastLED.h>
#include <bits/stdc++.h>

#define NUM_LEDS 144
#define DATA_PIN 4 
#define COLOUR 96
#define SATURATION 255
#define MAX_BRIGHTNESS 128
#define MIN_BRIGHTNESS 32
#define TIMEFACTOR 2


enum circularType {
    bar,
    light
};
CRGB leds[NUM_LEDS];
// static vector<int> barmask = {32,34,36,38,40,42,44,46,48,50,52,54,56,58,60,62,64,66,68,70,72,74,76,78,80,82,84,86,88,90,92,94,96,98,100,102,104,106,108,110,112,114,116,118,120,122,124,126,128,126,124,122,120,118,116,114,112,110,108,106,104,102,100,98,96,94,92,90,88,86,84,82,80,78,76,74,72,70,68,66,64,62,60,58,56,54,52,50,48,46,44,42,40,38,36,34,32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
// static vector<int> leuchtturm = {128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

std::vector<int> bm(NUM_LEDS);
std::vector<int> lh(NUM_LEDS);

// put function declarations here:
void rotateArr(std::vector<int>& arr, int d);
void circularAnimation(circularType animation);
void breatheAnimation();
void pulseAnimation();
void staticBrightness();
void wipeStrip();
void colourFadeTest();
void gen_lh();
void gen_bm();

void setup() {
  // put your setup code here, to run once:
  gen_bm();
  gen_lh();
  FastLED.addLeds<WS2812B, DATA_PIN>(leds, NUM_LEDS);
}


void loop() {
    circularAnimation(bar);
    wipeStrip();
    circularAnimation(light);
    wipeStrip();
    breatheAnimation();
    wipeStrip();
    pulseAnimation();
    wipeStrip();
    staticBrightness();
    wipeStrip();
    colourFadeTest();
}


// put function definitions here:

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
        // getColour here
        for(int dot = 0; dot < NUM_LEDS; dot++){
            leds[dot] = CHSV( COLOUR, SATURATION, MAX_BRIGHTNESS);
        }
        FastLED.show();
        delay(1500);
    }
}

void wipeStrip(){
    fadeToBlackBy(leds,NUM_LEDS,128);
    FastLED.show();
    delay(10);
    fadeToBlackBy(leds,NUM_LEDS,255);
    FastLED.show();
    delay(500);
}

void circularAnimation(circularType animation){
    std::vector<int> tempbar;
    tempbar = animation == bar ? bm : lh;

    for (int frame = 0;frame <= 288; frame++) {
        // getColour here
        for (int i=0;i<NUM_LEDS;i++) {
            leds[i] = CHSV( COLOUR, SATURATION, tempbar[i]);
        }

        FastLED.show();
        rotateArr(tempbar);
        delay(20);
    }   
}

void breatheAnimation(){
    for (int i=0;i<2;i++){
        for(int a=MIN_BRIGHTNESS;a<MAX_BRIGHTNESS;a++){
            // getColour here
            for(int dot = 0; dot < NUM_LEDS; dot++){
                leds[dot] = CHSV( COLOUR, SATURATION, a);
            }
            FastLED.show();
            delay(20);
        }
        for(int b=MAX_BRIGHTNESS;b>MIN_BRIGHTNESS;b--){
            // getColour here
            for(int dot = 0; dot < NUM_LEDS; dot++){
                leds[dot] = CHSV( COLOUR, SATURATION, b);
            }
            FastLED.show();
            delay(20);
        }
    }
}

void pulseAnimation(){
    int midbright = 65;
    for (int j=0;j<2;j++){
        // getColour here
        for(int dot = 0; dot < NUM_LEDS; dot++){
            leds[dot] = CHSV( COLOUR, SATURATION, midbright);
        }
        FastLED.show();
        delay(1000);
        for (int i=0;i<2;i++){
            for(int a=midbright;a<MAX_BRIGHTNESS;a= a+2){
                // getColour here
                for(int dot = 0; dot < NUM_LEDS; dot++){
                    leds[dot] = CHSV( COLOUR, SATURATION, a);
                }
                FastLED.show();
                delay(5);
            }
            for(int b=MAX_BRIGHTNESS;b>midbright;b= b-2){
                // getColour here
                for(int dot = 0; dot < NUM_LEDS; dot++){
                    leds[dot] = CHSV( COLOUR, SATURATION, b);
                }
                FastLED.show();
                delay(5);
            }
        }
    }
}