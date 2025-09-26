#ifndef TFT_MODULE_H
#define TFT_MODULE_H

// TFT SETUP
#include "User_Setup.h"
#include <TFT_eSPI.h>
#include <SD.h>
#include <SPI.h>
#include <FS.h>

#include "adc_module.h"
#include "sensors_module.h"
#include "gsm_module.h"
// #include "sd_module.h"

class tft_module {
private:
    TFT_eSPI tft = TFT_eSPI(); // TFT display object
    int row_height = 12; // Height of each row in pixels
    int row_counter = 0;
    int max_rows = TFT_HEIGHT / row_height; // Calculate maximum number of rows based on display height
    uint16_t read16(File &f);
    uint32_t read32(File &f);
public:
    tft_module() {};
    ~tft_module() {};
    void begin();
    void print(String text, int row);
    void print(String text);
    void clear();
    void dot_test(); // Function to print dots in a loop
    void print_sensors(adc_module* adc, sensors_module* sensors); // Function to print sensor values on the TFT display
    void print_connection_status(gsm_module* gsm); // Function to print connection status
    // void draw_image(const char* filename, sd_module* sd); // Function to draw an image on the TFT display
    int draw_bmp_img(File bmpFile, uint8_t x, uint16_t y);
    
    SPIClass& getSPIinstance(void);

    int width() { return tft.width(); };
    int height() { return tft.height(); };
    
    void print_connection_status_example();
    void print_sensors_example();
    void print_url();
    void print_quotes();
    void print_offset(String text, int row, int offset);
    void print_offset(String text, int offset);
};


#define SD_CS 32


class sd_module {
private:
    bool is_init = false;
public:
    sd_module() {};
    ~sd_module() {};

    void begin(tft_module* tft);
    void listFiles();
    // int loadBitmap(const char* filename, uint8_t* buffer);
    File openFile(char* filename);
};




#endif