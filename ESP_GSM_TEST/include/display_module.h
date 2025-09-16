#ifndef DISPLAY_MODULE_H
#define DISPLAY_MODULE_H

#include "tft_sd_module.h"
#include "adc_module.h"
#include "sensors_module.h"

#define DISP_PAGES 5

enum disp_page{
	SENSOR_PAGE,
	NETWORK_PAGE,
	QR_PAGE,
	URL_PAGE,
	QUOTES_PAGE
};

class display_module{
private:
    sd_module* sd = nullptr;
    adc_module* adc = nullptr;
    sensors_module* sensors = nullptr;
    tft_module* tft = nullptr;
    disp_page current_page = SENSOR_PAGE; // Default page to display
    unsigned long disp_times_ms[DISP_PAGES] = {10000, 10000, 20000, 5000, 10000};
    
public:
    display_module(){};
    ~display_module(){};
    void begin(sd_module* sd, tft_module* tft , adc_module* adc, sensors_module* sensors);
    void loop();
};



#endif // DISPLAY_MODULE_H