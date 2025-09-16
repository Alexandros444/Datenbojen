#include <Arduino.h>

#include "config.h"

#include "webserver_util.h"
#include "gps_module.h"
#include "tft_sd_module.h"
#include "adc_module.h"
#include "sensors_module.h"
#include "led_module.h"
#include "display_module.h"

adc_module adc;
sensors_module sensors;
tft_module tft;
sd_module sd;
gsm_module gsm; // Create a GSM module instance
led_module led;
display_module display;

TaskHandle_t Task1;
TaskHandle_t Task2;

void led_loop(void *pvParameters);

void setup(){

	Serial.begin(115200);
	Serial.println("Serial Initialzied.");

	delay(1000); // Wait for Serial Monitor to open

	tft.begin(); // Initialize TFT display
	sd.begin(&tft);
	display.begin(&sd, &tft, &adc, &sensors);

	led.begin();
	// webserverSetup();

	// gps_setup();

	adc.begin();

	sensors.begin(&adc); // Initialize sensors with the ADC module

	Serial.println("Core in Use for Main Loop: "+ String(xPortGetCoreID()));

	xTaskCreatePinnedToCore(
	      led_loop, /* Function to implement the task */
	      "led_loop", /* Name of the task */
	      5000,  /* Stack size in words */
	      NULL,  /* Task input parameter */
	      0,  /* Priority of the task */
	      &Task1,  /* Task handle. */
	      0); /* Core where the task should run */

	gsm.begin();
}


void loop(){

	gsm.loop();

	display.loop();

}

void led_loop(void *pvParameters)
{
	Serial.println("Core in Use for LED Loop: " + String(xPortGetCoreID()));
	for (;;)
	{
		led.loop();
		delay(1);
		led.rand_color_change();
	}
}