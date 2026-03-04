#include "gpio_module.h"

#include "config.h"

// #include "webserver_util.h"
// #include "gps_module.h"
// #include "tft_sd_module.h"
#include "adc_module.h"
#include "sensors_module.h"
// #include "led_module.h"
// #include "display_module.h"
// #include "lora_module.h"
#include "wifi_module.h"
#include "sleep_module.h"
#include "gsm_module.h"
#include "state_module.h"

state_module sys_state;
gpio_module pcf;
wifi_module wifi_mod;
sleep_module sleep_mod;
// lora_module lora;
adc_module adc;
sensors_module sensors;
// tft_module tft;
// sd_module sd;
gsm_module gsm; // Create a GSM module instance
// led_module led;
// display_module display;


// TaskHandle_t Task1;
// TaskHandle_t Task2;

// void led_loop(void *pvParameters);

void setup(){

	Serial.begin(115200);
	Serial.println("Serial Initialzied.");

	sys_state.begin();

	delay(1000); // Wait for Serial Monitor to open

	pcf.begin();
	// lora.begin();

	sleep_mod.begin();

	adc.begin();
	sensors.begin(&adc);

	gsm.begin();
	
	// wifi_mod.begin(&sensors, &gsm);


	// pinMode(PCF0, OUTPUT);

	// tft.begin(); // Initialize TFT display
	// sd.begin(&tft);
	// display.begin(&sd, &tft, &adc, &sensors);

	// led.begin();
	// // webserverSetup();

	// // gps_setup();

	// adc.begin();

	// sensors.begin(&adc); // Initialize sensors with the ADC module

	// Serial.println("Core in Use for Main Loop: "+ String(xPortGetCoreID()));

	// xTaskCreatePinnedToCore(
	//       led_loop, /* Function to implement the task */
	//       "led_loop", /* Name of the task */
	//       5000,  /* Stack size in words */
	//       NULL,  /* Task input parameter */
	//       0,  /* Priority of the task */
	//       &Task1,  /* Task handle. */
	//       0); /* Core where the task should run */

	// gsm.begin();
}

static int retry_count = 0;

void loop(){
	while (gsm.is_debug_mode())
		gsm.loop();

	// delay(10000);
	// lora.sendPacket("Test");
	int transmission_status = gsm.sendData(&sensors);
	Serial.printf("Packet status: %d, retry: %d\n", transmission_status, retry_count);

	if (transmission_status != 0 && retry_count < 3) {
		Serial.println("Error in data transmission, retrying in 10 seconds...");
		delay(10000);
		retry_count++;
		return;
	}
	retry_count = 0;

	Serial.println("Entering Sleep Mode");
	
	gsm.turn_off();

	// wifi_mod.turn_off();

	sleep_mod.system_cut_power();

	// digitalWrite(PCF0, HIGH);
	// Serial.println("HIGH");
	// delay(2000);
	// digitalWrite(PCF0, LOW);
	// Serial.println("LOW");
	// delay(2000);	

	// gsm.loop();

	// display.loop();

}

// void led_loop(void *pvParameters)
// {
// 	Serial.println("Core in Use for LED Loop: " + String(xPortGetCoreID()));
// 	for (;;)
// 	{
// 		led.loop();
// 		delay(1);
// 		led.rand_color_change();
// 	}
// }