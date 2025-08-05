#include "tft_sd_module.h"


void sd_module::begin(tft_module tft) {
    Serial.println("Initializing SD card...");

    if (!SD.begin(SD_CS, tft.getSPIinstance())) {
        Serial.println("SD card initialization failed!");
        return;
    }
    Serial.println("SD card initialized successfully.");
    is_init = true;
    
    listFiles();
}

void sd_module::listFiles() {
    if (!is_init)return;
    Serial.println("Listing files on SD card:");
    File root = SD.open("/");
    while (File file = root.openNextFile()) {
        Serial.print("File: ");
        Serial.println(file.name());
        file.close();
    }
}

File sd_module::openFile(char* filename) {
    if (!is_init) return File();
    return SD.open(filename);
}


// TODO: Nicht fertig
// int sd_module::loadBitmap(const char* filename, uint8_t* buffer) {
//     File file = SD.open(filename);
//     if (!file) {
//         Serial.println("Failed to open file");
//         return 0;
//     }
//     buffer = (uint8_t*) malloc(file.size());
//     if (!buffer) {
//         Serial.println("Failed to allocate memory for bitmap");
//         file.close();
//         return 0;
//     }
//     file.read(buffer, file.size());
//     file.close();
//     return file.size();
// }



