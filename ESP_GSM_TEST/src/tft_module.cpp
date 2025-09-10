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