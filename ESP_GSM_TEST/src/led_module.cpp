#include "led_module.h"

// BIG TODO:: No Delay! No Active Wait Time!.
/* Etwa So
 * anim x
 * static progress (persists throughout runs) (oder Class-Variable)
 * wenn time - last_time > wait time
 *  do anim, progress ++
 * return
 */


void led_module::begin() {
    // TODO: KA compiled nicht anton fix it, wenn du unbedingt mit so high level C arbeiten musst lol
  // put your setup code here, to run once:
//   generate_bar_brightness_mask();
//   generate_lighthouse_brightness_mask();
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
}


void led_module::example_anim_no_dly() {
    // xms Delay between animation Steps
    if (millis() - last_time_ms < 20)
        return;

    last_time_ms = millis();

    static int dir = 0;
    leds[progress] = dir == 0 ? CRGB::Red : CRGB::Black;
    if (progress == 0)
        dir = !dir;
    
    inc_progress();

    FastLED.setBrightness(20);

    FastLED.show();    
}

void led_module::loop() {
    // circularAnimation(bar);
    // wipeStrip();
    // circularAnimation(light);
    // wipeStrip();
    // breatheAnimation();
    // wipeStrip();
    // pulseAnimation();
    // wipeStrip();
    // staticBrightness();
    // wipeStrip();
    // colourFadeTest();
    example_anim_no_dly();

}


// put function definitions here:

void led_module::rotateArr(std::vector<int>& arr) {
    int n = arr.size();
      
    // Right rotate the array by one position
    int last = arr[n - 1];
    for (int j = n - 1; j > 0; j--) {
        arr[j] = arr[j - 1];
    }
    arr[0] = last;
}

// TODO: KA compiled nicht anton fix it, wenn du unbedingt mit so high level C arbeiten musst lol
// void led_module::evil_iota(ForwardIt first, ForwardIt last, T value)
// {
//     for (; first != last; ++first, value -= 2){
//         if(value <= MIN_BRIGHTNESS-1)continue;
//         *first = value;
//     }
        
// }

// void led_module::my_iota(ForwardIt first, ForwardIt last, T value)
// {
//     for (; first != last; ++first, value += 2){
//         if(value >= MAX_BRIGHTNESS+1) {
//             *first = MAX_BRIGHTNESS;
//             continue;
//         }
//         *first = value;
//     }
// }

/**
 * @brief Generates a brightness mask for the bar animation.
 *
 * The brightness mask is a vector of brightness values that will be used to
 * control the brightness of the LEDs during the bar animation. The mask is
 * divided into three sections. The first section is a ramp from MIN_BRIGHTNESS
 * to MAX_BRIGHTNESS. The second section is a ramp from MAX_BRIGHTNESS back down
 * to MIN_BRIGHTNESS. The third section is a constant value of 0.
 */
// void led_module::generate_bar_brightness_mask(){
//     int section1, section2;
//     section1 = NUM_LEDS/6;
//     section2 = NUM_LEDS-section1;
//     my_iota(bm.begin(), bm.begin()+section2/2, MIN_BRIGHTNESS);
//     evil_iota(bm.begin()+section2/2, bm.begin()+section2, bm[(section2/2)-1]);
//     std::fill(bm.begin()+section2, bm.end(), 0);
// }

/**
 * @brief Generates a brightness mask for the lighthouse animation.
 *
 * The lighthouse mask is a vector of brightness values that will be used to
 * control the brightness of the LEDs during the lighthouse animation. The mask
 * is divided into four sections. The first and third sections have a brightness
 * of MAX_BRIGHTNESS, while the second and fourth sections have a brightness of 0.
 */
// void led_module::generate_lighthouse_brightness_mask() {
//     int section_size = NUM_LEDS / 4;
//     int cmp = NUM_LEDS % 4;
//     int section1 = section_size;
//     int section2 = (cmp == 2 || cmp == 3) ? section_size + 1 : section_size;

//     std::fill(lh.begin(), lh.begin() + section1, MAX_BRIGHTNESS);
//     std::fill(lh.begin() + section1, lh.begin() + section1 + section2, 0);
//     std::fill(lh.begin() + section1 + section2, lh.begin() + section1 * 2 + section2, MAX_BRIGHTNESS);
//     std::fill(lh.begin() + section1 * 2 + section2, lh.end(), 0);
// }

void led_module::colourFadeTest(){
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

void led_module::staticBrightness(){
    for (int i=0;i<2;i++){
        // getColour here
        for(int dot = 0; dot < NUM_LEDS; dot++){
            leds[dot] = CHSV( COLOUR, SATURATION, MAX_BRIGHTNESS);
        }
        FastLED.show();
        delay(1500);
    }
}

void led_module::wipeStrip(){
    fadeToBlackBy(leds,NUM_LEDS,128);
    FastLED.show();
    delay(10);
    fadeToBlackBy(leds,NUM_LEDS,255);
    FastLED.show();
    delay(500);
}

// TODO: KA compiled nicht anton fix it, wenn du unbedingt mit so high level C arbeiten musst lol
// void led_module::circularAnimation(circularType animation){
//     std::vector<int> tempbar;
//     tempbar = animation == bar ? bm : lh;

//     for (int frame = 0;frame <= 288; frame++) {
//         // getColour here
//         for (int i=0;i<NUM_LEDS;i++) {
//             leds[i] = CHSV( COLOUR, SATURATION, tempbar[i]);
//         }

//         FastLED.show();
//         rotateArr(tempbar);
//         delay(20);
//     }   
// }

void led_module::breatheAnimation(){
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

void led_module::pulseAnimation(){
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