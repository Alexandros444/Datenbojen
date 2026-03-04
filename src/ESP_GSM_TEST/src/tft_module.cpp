#include "tft_sd_module.h"

// #define TFT_LED 25
// #define TFT_SCLK 15
// #define TFT_SDA 2
// #define TFT_DC 0
// #define TFT_RST 18
// #define TFT_CS 19

void tft_module::begin() {
    Serial.println("TFT setup");
    tft.begin();

    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_BLUE, TFT_BLACK);
    tft.setTextSize(1);
    tft.setCursor(0, 0);
    tft.println("TFT setup done");
    debug_info();
    Serial.println("TFT setup done");
}

void tft_module::clear() {
    tft.fillScreen(TFT_BLACK);
    row_counter = 0; // Reset the row counter
}

void tft_module::print(String text) {
    if (row_counter >= max_rows) {
        Serial.println("Maximum rows reached, clearing display");
        clear(); // Clear the display if maximum rows are reached
    }
    print(text, row_counter);
    row_counter++;

}

void tft_module::print_offset(String text, int offset){
    if (row_counter >= max_rows) {
        Serial.println("Maximum rows reached, clearing display");
        clear(); // Clear the display if maximum rows are reached
    }
    print_offset(text, row_counter, offset);
    row_counter++;
}

#define BUFFPIXEL 8

int tft_module::draw_bmp_img(File bmpFile, uint8_t x, uint16_t y) {
    int      bmpWidth, bmpHeight;
    uint8_t  bmpDepth;
    uint32_t bmpImageoffset;
    uint32_t rowSize;
    uint8_t  sdbuffer[3 * BUFFPIXEL];
    uint8_t  buffidx = sizeof(sdbuffer);
    boolean  goodBmp = false;
    boolean  flip = true;
    int      w, h, row, col;
    uint8_t  r, g, b;
    uint32_t pos = 0, startTime = millis();

    if (bmpFile.size() <= 0) {
        Serial.print(F("File not found"));
        return -1;
    }

    clear();

    Serial.printf("Reading BMP file: %s\n", bmpFile.name());

    if (read16(bmpFile) == 0x4D42) {
        read32(bmpFile);
        (void)read32(bmpFile);
        bmpImageoffset = read32(bmpFile);
        bmpImageoffset, DEC;
        read32(bmpFile);
        bmpWidth = read32(bmpFile);
        bmpHeight = read32(bmpFile);
        if (read16(bmpFile) == 1) {
            bmpDepth = read16(bmpFile);
            Serial.print(F("Bit Depth: ")); Serial.println(bmpDepth);
            if ((bmpDepth == 24) && (read32(bmpFile) == 0)) {

                goodBmp = true;
                Serial.print(F("Image size: "));
                Serial.print(bmpWidth);
                Serial.print('x');
                Serial.println(bmpHeight);


                rowSize = (bmpWidth * 3 + 3) & ~3;


                if (bmpHeight < 0) {
                    bmpHeight = -bmpHeight;
                    flip = false;
                }
                w = bmpWidth;
                h = bmpHeight;
                if ((x + w - 1) >= tft.width())  w = tft.width() - x;
                if ((y + h - 1) >= tft.height()) h = tft.height() - y;
                tft.setAddrWindow(x, y, x + w - 1, y + h - 1);
                for (row = 0; row < h; row++) {
                    if (flip)
                        pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
                    else
                        pos = bmpImageoffset + row * rowSize;
                    if (bmpFile.position() != pos) {
                        bmpFile.seek(pos);
                        buffidx = sizeof(sdbuffer);
                    }

                    for (col = 0; col < w; col++) {
                        if (buffidx >= sizeof(sdbuffer)) {
                            bmpFile.read(sdbuffer, sizeof(sdbuffer));
                            buffidx = 0;
                        }


                        b = sdbuffer[buffidx++];
                        g = sdbuffer[buffidx++];
                        r = sdbuffer[buffidx++];
                        tft.pushColor(tft.color565(r, g, b));
                    }
                }
                Serial.print(F("Loaded in "));
                Serial.print(millis() - startTime);
                Serial.println(" ms");
            }
        }
    }

    bmpFile.close();
    if (!goodBmp) {
        Serial.println(F("BMP format not recognized."));
        return -2;
    }
    return 0;
}


uint16_t tft_module::read16(File& f) {
    uint16_t result;
    ((uint8_t*)&result)[0] = f.read();
    ((uint8_t*)&result)[1] = f.read();
    return result;
}

uint32_t tft_module::read32(File& f) {
    uint32_t result;
    ((uint8_t*)&result)[0] = f.read();
    ((uint8_t*)&result)[1] = f.read();
    ((uint8_t*)&result)[2] = f.read();
    ((uint8_t*)&result)[3] = f.read();
    return result;
}


// void tft_module::draw_image(const char* filename, sd_module* sd) {


//     uint8_t* buffer = nullptr;
//     if (!sd->loadBitmap(filename, buffer)) {
//         Serial.println("Failed to load bitmap");
//         return;
//     }

//     tft.drawBitmap(0, 0, buffer, TFT_WIDTH, TFT_HEIGHT, TFT_ORANGE, TFT_BLACK);
// }

void tft_module::print_offset(String text, int row, int offset) {
    int y_pos = row * row_height + offset; // Assuming each row is 15 pixels high
    tft.fillRect(0, y_pos, tft.width(), row_height, TFT_BLACK);
    tft.setCursor(0, y_pos);
    tft.println(text.c_str());
}

void tft_module::print(String text, int row) {
    int y_pos = row * row_height; // Assuming each row is 15 pixels high
    tft.fillRect(0, y_pos, tft.width(), row_height, TFT_BLACK);
    tft.setCursor(0, y_pos);
    tft.println(text.c_str());
}

void tft_module::print_sensors(adc_module* adc, sensors_module* sensors) {
    if (sensors == nullptr || adc == nullptr) {
        Serial.println("Error: Null pointer detected for sensors or adc");
        return;
    }

    clear(); // Clear the display before printing sensor values

    print("Temperature: " + String((float)sensors->temperature, 2) + " C");
    print("TDS Value: " + String((float)sensors->tdsValue, 2) + " ppm");
    print("         ADC " + String(TDS_ADC_PIN) + ": " + String((float)adc->readVoltage(TDS_ADC_PIN), 3) + " V");
    print("pH Value: " + String((float)sensors->phValue, 2));
    print("         ADC " + String(PH_ADC_PIN) + ": " + String((float)adc->readVoltage(PH_ADC_PIN), 3) + " V");
    print("DO Value: " + String((float)sensors->doValue, 2) + " %");
    print("         ADC " + String(DO_ADC_PIN) + ": " + String((float)adc->readVoltage(DO_ADC_PIN), 3) + " V");
    print("Turbidity: " + String((float)sensors->turbidity, 2) + " NTU");
    print("         ADC " + String(TRB_ADC_PIN) + ": " + String((float)adc->readVoltage(TRB_ADC_PIN), 3) + " V");

    // String adc_data = "ADC: 0: " + String((float)adc->readVoltage(0), 3) + "V, " +
    //                   "1: " + String((float)adc->readVoltage(1), 3) + " V, " +
    //                   "2: " + String((float)adc->readVoltage(2), 3) + " V, " +
    //                   "3: " + String((float)adc->readVoltage(3), 3) + " V";
    // print(adc_data);
}

void tft_module::print_sensors_example() {
    int offset = 6;
    clear(); // Clear the display before printing sensor values
    print("==========Sensors=========");
    print_offset("Temperature  - 18.5 C", offset);
    row_counter++;
    print_offset("Oxygen       - 8.2 mg/L", offset);
    row_counter++;
    print_offset("pH-Value     - 7,5", offset);
    row_counter++;
    print_offset("Conductivity - 420 uS/cm", offset);
    row_counter++;
    print_offset("Turbidity    - 10 NTU", offset);
    
}

void tft_module::print_connection_status_example() {
    int offset = 6;
    clear(); // Clear the display before printing connection status
    print("==========Status==========");
    print_offset("Network  - Connected", offset);
    row_counter++;
    print_offset("GPRS     - Connected", offset);
    row_counter++;
    print_offset("Battery  - 42%", offset);
    row_counter++;
    print_offset("Signal   - Good", offset);
    row_counter++;
    print_offset("Location - HU Forum Berlin", offset);
}

void tft_module::print_url() {
    // tft.setTextColor(TFT_WHITE, TFT_BLACK);
    print("      Spreeberlin.de", 10);
    // tft.setTextColor(TFT_BLUE, TFT_BLACK);
}

void tft_module::print_quotes(){
    clear(); // Clear the display before printing connection status
    print("\"I am the river, the river is me\"\n - Erena Rhoese"); 
    row_counter++;
    row_counter++;

    print("\"We are all Bodies of Water.\"\n - Astrida Neimanis"); 
    row_counter++;
    row_counter++;
    
    print("\"In me everything is already flowing.\"\n - Luce Irigaray"); 
    row_counter++;
    row_counter++;
}


void tft_module::print_connection_status(gsm_module* gsm) {
    if (gsm == nullptr && gsm->is_init()) {
        Serial.println("Error: Null pointer detected for gsm");
        return;
    }
    statusInfo status = gsm->getStatusInfo();

    clear(); // Clear the display before printing connection status
    print("Network: " + String(status.isNetworkConnected ? "Connected" : "Disconnected"));
    print("GPRS: " + String(status.isGprsConnected ? "Connected" : "Disconnected"));
    print("Battery: " + status.batt);
    row_counter++;
    print("Signal: " + status.signalQuality);
    print("Reg Status: " + status.regStatus);
    print("Location: " + status.loc);
    print("Operator: " + status.operatorName);
    print("Modem Info: " + status.modemInfo);
    print("Network Time: " + status.networkTime);
}

void tft_module::dot_test() {
    // reapeat print dots 
    static unsigned long lastTime = 0;
    static unsigned long dot_pos_x = 0;
    unsigned long currentTime = millis();
    if (currentTime - lastTime > 1000) {
        // Serial.println("TFT loop");
        lastTime = currentTime;
        tft.setCursor(dot_pos_x, 20);
        tft.println(".");
        dot_pos_x += 6;
    }
    if (dot_pos_x > tft.width()) {
        // clear one line
        tft.fillRect(0, 20, tft.width(), 20, TFT_BLACK);
        dot_pos_x = 0;
    }
}


SPIClass& tft_module::getSPIinstance(void) {
    return tft.getSPIinstance();
}


// void tft_module::disp_qr(){
//     tft.drawBitmap(0, 0, qr_code, 100, 100, TFT_WHITE);
// }




void tft_module::debug_info(void) {
    setup_t user;

    tft.getSetup(user); //
    
    Serial.print("\n[code]\n");
    
    Serial.print ("TFT_eSPI ver = "); Serial.println(user.version);
    printProcessorName(user);
    #if defined (ESP32) || defined (ARDUINO_ARCH_ESP8266)
      if (user.esp < 0x32F000 || user.esp > 0x32FFFF) { Serial.print("Frequency    = "); Serial.print(ESP.getCpuFreqMHz());Serial.println("MHz"); }
    #endif
    #ifdef ARDUINO_ARCH_ESP8266
      Serial.print("Voltage      = "); Serial.print(ESP.getVcc() / 918.0); Serial.println("V"); // 918 empirically determined
    #endif
    Serial.print("Transactions = "); Serial.println((user.trans  ==  1) ? "Yes" : "No");
    Serial.print("Interface    = "); Serial.println((user.serial ==  1) ? "SPI" : "Parallel");
    #ifdef ARDUINO_ARCH_ESP8266
    if (user.serial ==  1){ Serial.print("SPI overlap  = "); Serial.println((user.overlap == 1) ? "Yes\n" : "No\n"); }
    #endif
    if (user.tft_driver != 0xE9D) // For ePaper displays the size is defined in the sketch
    {
      Serial.print("Display driver = "); Serial.println(user.tft_driver, HEX); // Hexadecimal code
      Serial.print("Display width  = "); Serial.println(user.tft_width);  // Rotation 0 width and height
      Serial.print("Display height = "); Serial.println(user.tft_height);
      Serial.println();
    }
    else if (user.tft_driver == 0xE9D) Serial.println("Display driver = ePaper\n");
    
    if (user.r0_x_offset  != 0)  { Serial.print("R0 x offset = "); Serial.println(user.r0_x_offset); } // Offsets, not all used yet
    if (user.r0_y_offset  != 0)  { Serial.print("R0 y offset = "); Serial.println(user.r0_y_offset); }
    if (user.r1_x_offset  != 0)  { Serial.print("R1 x offset = "); Serial.println(user.r1_x_offset); }
    if (user.r1_y_offset  != 0)  { Serial.print("R1 y offset = "); Serial.println(user.r1_y_offset); }
    if (user.r2_x_offset  != 0)  { Serial.print("R2 x offset = "); Serial.println(user.r2_x_offset); }
    if (user.r2_y_offset  != 0)  { Serial.print("R2 y offset = "); Serial.println(user.r2_y_offset); }
    if (user.r3_x_offset  != 0)  { Serial.print("R3 x offset = "); Serial.println(user.r3_x_offset); }
    if (user.r3_y_offset  != 0)  { Serial.print("R3 y offset = "); Serial.println(user.r3_y_offset); }
    
    if (user.pin_tft_mosi != -1) { Serial.print("MOSI    = "); Serial.print("GPIO "); Serial.println(getPinName(user,user.pin_tft_mosi)); }
    if (user.pin_tft_miso != -1) { Serial.print("MISO    = "); Serial.print("GPIO "); Serial.println(getPinName(user,user.pin_tft_miso)); }
    if (user.pin_tft_clk  != -1) { Serial.print("SCK     = "); Serial.print("GPIO "); Serial.println(getPinName(user,user.pin_tft_clk)); }
    
    #ifdef ARDUINO_ARCH_ESP8266
    if (user.overlap == true)
    {
      Serial.println("Overlap selected, following pins MUST be used:");
    
                                 Serial.println("MOSI     = SD1 (GPIO 8)");
                                 Serial.println("MISO     = SD0 (GPIO 7)");
                                 Serial.println("SCK      = CLK (GPIO 6)");
                                 Serial.println("TFT_CS   = D3  (GPIO 0)\n");
    
      Serial.println("TFT_DC and TFT_RST pins can be user defined");
    }
    #endif
    String pinNameRef = "GPIO ";
    #ifdef ARDUINO_ARCH_ESP8266
      pinNameRef = "PIN_D";
    #endif
    
    if (user.esp == 0x32F) {
      Serial.println("\n>>>>> Note: STM32 pin references above D15 may not reflect board markings <<<<<");
      pinNameRef = "D";
    }
    if (user.pin_tft_cs != -1) { Serial.print("TFT_CS   = " + pinNameRef); Serial.println(getPinName(user,user.pin_tft_cs)); }
    if (user.pin_tft_dc != -1) { Serial.print("TFT_DC   = " + pinNameRef); Serial.println(getPinName(user,user.pin_tft_dc)); }
    if (user.pin_tft_rst!= -1) { Serial.print("TFT_RST  = " + pinNameRef); Serial.println(getPinName(user,user.pin_tft_rst)); }
    
    if (user.pin_tch_cs != -1) { Serial.print("TOUCH_CS = " + pinNameRef); Serial.println(getPinName(user,user.pin_tch_cs)); }
    
    if (user.pin_tft_wr != -1) { Serial.print("TFT_WR   = " + pinNameRef); Serial.println(getPinName(user,user.pin_tft_wr)); }
    if (user.pin_tft_rd != -1) { Serial.print("TFT_RD   = " + pinNameRef); Serial.println(getPinName(user,user.pin_tft_rd)); }
    
    if (user.pin_tft_d0 != -1) { Serial.print("\nTFT_D0   = " + pinNameRef); Serial.println(getPinName(user,user.pin_tft_d0)); }
    if (user.pin_tft_d1 != -1) { Serial.print("TFT_D1   = " + pinNameRef); Serial.println(getPinName(user,user.pin_tft_d1)); }
    if (user.pin_tft_d2 != -1) { Serial.print("TFT_D2   = " + pinNameRef); Serial.println(getPinName(user,user.pin_tft_d2)); }
    if (user.pin_tft_d3 != -1) { Serial.print("TFT_D3   = " + pinNameRef); Serial.println(getPinName(user,user.pin_tft_d3)); }
    if (user.pin_tft_d4 != -1) { Serial.print("TFT_D4   = " + pinNameRef); Serial.println(getPinName(user,user.pin_tft_d4)); }
    if (user.pin_tft_d5 != -1) { Serial.print("TFT_D5   = " + pinNameRef); Serial.println(getPinName(user,user.pin_tft_d5)); }
    if (user.pin_tft_d6 != -1) { Serial.print("TFT_D6   = " + pinNameRef); Serial.println(getPinName(user,user.pin_tft_d6)); }
    if (user.pin_tft_d7 != -1) { Serial.print("TFT_D7   = " + pinNameRef); Serial.println(getPinName(user,user.pin_tft_d7)); }
    
    #if defined (TFT_BL)
      Serial.print("\nTFT_BL           = " + pinNameRef); Serial.println(getPinName(user,user.pin_tft_led));
      #if defined (TFT_BACKLIGHT_ON)
        Serial.print("TFT_BACKLIGHT_ON = "); Serial.println(user.pin_tft_led_on == HIGH ? "HIGH" : "LOW");
      #endif
    #endif
    
    Serial.println();
    
    uint16_t fonts = tft.fontsLoaded();
    if (fonts & (1 << 1))        Serial.print("Font GLCD   loaded\n");
    if (fonts & (1 << 2))        Serial.print("Font 2      loaded\n");
    if (fonts & (1 << 4))        Serial.print("Font 4      loaded\n");
    if (fonts & (1 << 6))        Serial.print("Font 6      loaded\n");
    if (fonts & (1 << 7))        Serial.print("Font 7      loaded\n");
    if (fonts & (1 << 9))        Serial.print("Font 8N     loaded\n");
    else
    if (fonts & (1 << 8))        Serial.print("Font 8      loaded\n");
    if (fonts & (1 << 15))       Serial.print("Smooth font enabled\n");
    Serial.print("\n");
    
    if (user.serial==1)        { Serial.print("Display SPI frequency = "); Serial.println(user.tft_spi_freq/10.0); }
    if (user.pin_tch_cs != -1) { Serial.print("Touch SPI frequency   = "); Serial.println(user.tch_spi_freq/10.0); }
    
    Serial.println("[/code]");
    
    delay(3000);
}
    
void tft_module::printProcessorName(setup_t user){
      Serial.print("Processor    = ");
      if ( user.esp == 0x8266) Serial.println("ESP8266");
      if ( user.esp == 0x32)   Serial.println("ESP32");
      if ( user.esp == 0x32F)  Serial.println("STM32");
      if ( user.esp == 0x2040) Serial.println("RP2040");
      if ( user.esp == 0x0000) Serial.println("Generic");
}
    
    // Get pin name
int8_t tft_module::getPinName(setup_t user, int8_t pin){
      // For ESP32 and RP2040 pin labels on boards use the GPIO number
      if (user.esp == 0x32 || user.esp == 0x2040) return pin;
    
      if (user.esp == 0x8266) {
        // For ESP8266 the pin labels are not the same as the GPIO number
        // These are for the NodeMCU pin definitions:
        //        GPIO       Dxx
        if (pin == 16) return 0;
        if (pin ==  5) return 1;
        if (pin ==  4) return 2;
        if (pin ==  0) return 3;
        if (pin ==  2) return 4;
        if (pin == 14) return 5;
        if (pin == 12) return 6;
        if (pin == 13) return 7;
        if (pin == 15) return 8;
        if (pin ==  3) return 9;
        if (pin ==  1) return 10;
        if (pin ==  9) return 11;
        if (pin == 10) return 12;
      }
    
      if (user.esp == 0x32F) return pin;
    
      return pin; // Invalid pin
}
    