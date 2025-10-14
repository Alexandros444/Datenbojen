#include "display_module.h"

void display_module::begin(sd_module* sd, tft_module* tft , adc_module* adc, sensors_module* sensors){
    if (sd == nullptr || tft == nullptr || adc == nullptr || sensors == nullptr) {
        Serial.println("Error: Null pointer detected during display_module initialization");
        Serial.printf("sd: %x\n", sd);
        Serial.printf("tft: %x\n", tft);
        Serial.printf("adc: %x\n", adc);
        Serial.printf("sensors: %x\n", sensors);    
        return;
    }
    this->sd = sd;
    this->tft = tft;
    this->adc = adc;
    this->sensors = sensors;
    current_page = SENSOR_PAGE; // Start with the sensor page
}

void display_module::loop(){
	static unsigned long last_new_page_ms = 0;
	int wait_for_page = max((int) current_page - 1, 0);
	if (millis() > last_new_page_ms + disp_times_ms[wait_for_page]){
		last_new_page_ms = millis();
		adc->print_data();
		sensors->print();
		Serial.printf("Disp Page %d of %d\n", current_page, DISP_PAGES);
		if (current_page == SENSOR_PAGE)
		{
			tft->print_sensors_example(); // Print sensor values on the TFT display
		}
		else if (current_page == NETWORK_PAGE)
		{
			tft->print_connection_status_example(); // Print connection status on the TFT display
		}
		else if (current_page == QR_PAGE)
		{
			File bmpFile = sd->openFile("/qr.bmp");
			tft->draw_bmp_img(bmpFile, 1, 1);
		}
		else if (current_page == URL_PAGE)
		{
			tft->print_url();
		}
		else if (current_page == QUOTES_PAGE)
		{
			tft->print_quotes();
		}
		current_page = (disp_page)(((int)current_page + 1) % DISP_PAGES);
    }
}
