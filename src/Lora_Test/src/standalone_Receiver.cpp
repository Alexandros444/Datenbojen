/*
 * EBYTE LoRa E22
 * Send a string message to a fixed point ADDH ADDL CHAN 0 3 23 with WOR preamble
 *
 * Write a string on serial monitor or reset to resend default value.
 *
 * Send a fixed point message, you must check that the transmitter and receiver have different
 * CHANNEL ADDL or ADDH, check down the correct configuration
 *
 * Pai attention e22 support RSSI, if you want use that functionality you must enable RSSI on configuration
 * configuration.TRANSMISSION_MODE.enableRSSI = RSSI_ENABLED;
 *
 * and uncomment #define ENABLE_RSSI true in this sketch
 *
 * Renzo Mischianti <https://www.mischianti.org>
 * https://www.mischianti.org/category/my-libraries/ebyte-lora-e22-devices/
 *
 * E22		  ----- WeMos D1 mini	----- esp32			----- Arduino Nano 33 IoT	----- Arduino MKR	----- ArduinoUNO
 * M0         ----- D7 (or 3.3v)	----- 19 (or 3.3v)	----- 4 (or 3.3v)			----- 2 (or 3.3v)	----- 7 Volt div (or 3.3v)
 * M1         ----- D6 (or GND)		----- 21 (or GND)	----- 6 (or GND)			----- 4 (or GND)	----- 6 Volt div (or GND)
 * TX         ----- D3 (PullUP)		----- TX2 (PullUP)	----- TX1 (PullUP)			----- 14 (PullUP)	----- 4 (PullUP)
 * RX         ----- D4 (PullUP)		----- RX2 (PullUP)	----- RX1 (PullUP)			----- 13 (PullUP)	----- 5 Volt div (PullUP)
 * AUX        ----- D5 (PullUP)		----- 18  (PullUP)	----- 2  (PullUP)			----- 0  (PullUP)	----- 3 (PullUP)
 * VCC        ----- 3.3v/5v			----- 3.3v/5v		----- 3.3v/5v				----- 3.3v/5v		----- 3.3v/5v
 * GND        ----- GND				----- GND			----- GND					----- GND			----- GND
 *
 */

// with this DESTINATION_ADDL 3 you must set
// WOR RECEIVER configuration to the other device and
// WOR SENDER to this device
#define DESTINATION_ADDL 3

// If you want use RSSI uncomment //#define ENABLE_RSSI true
// and use relative configuration with RSSI enabled
// #define ENABLE_RSSI true

#include "Arduino.h"
#include "LoRa_E22.h"
#include <SoftwareSerial.h>

// ---------- esp8266 pins --------------
// LoRa_E22 e22ttl(RX, TX, AUX, M0, M1);  // Arduino RX <-- e220 TX, Arduino TX --> e220 RX
// LoRa_E22 e22ttl(D3, D4, D5, D7, D6); // Arduino RX <-- e220 TX, Arduino TX --> e220 RX AUX M0 M1
// LoRa_E22 e22ttl(D2, D3); // Config without connect AUX and M0 M1

// #include <SoftwareSerial.h>
// SoftwareSerial mySerial(D2, D3); // Arduino RX <-- e220 TX, Arduino TX --> e220 RX
// LoRa_E22 e22ttl(&mySerial, D5, D7, D6); // AUX M0 M1
//  -------------------------------------

// ---------- Arduino pins --------------
// LoRa_E22 e22ttl(4, 5, 3, 7, 6); // Arduino RX <-- e220 TX, Arduino TX --> e220 RX AUX M0 M1
// LoRa_E22 e22ttl(4, 5); // Config without connect AUX and M0 M1

// #include <SoftwareSerial.h>
// SoftwareSerial mySerial(4, 5); // Arduino RX <-- e220 TX, Arduino TX --> e220 RX
// LoRa_E22 e22ttl(&mySerial, 3, 7, 6); // AUX M0 M1
//  -------------------------------------

// ------------- Arduino MKR WiFi 1010 -------------
// LoRa_E22 e22ttl(&Serial1, 2, 4, 6); //  RX AUX M0 M1
// -------------------------------------------------

// ------------- Arduino MKR WiFi 1010 -------------
// LoRa_E22 e22ttl(&Serial1, 0, 2, 4); //  RX AUX M0 M1
// -------------------------------------------------

// ---------- esp32 pins --------------
LoRa_E22 e22ttl(&Serial2, 18, 19, 21); //  RX AUX M0 M1

// LoRa_E22 e22ttl(&Serial2, 22, 4, 18, 21, 19, UART_BPS_RATE_9600); //  esp32 RX <-- e220 TX, esp32 TX --> e220 RX AUX M0 M1
//  -------------------------------------

#define ENABLE_RSSI 1

#define CHANNEL 0

Configuration conf;

void setup()
{
	Serial.begin(115200);
	delay(500);

    Serial2.begin(9600, SERIAL_8N1, 16, 17); // RX, TX

    // Startup all pins and UART
	e22ttl.begin();
	e22ttl.setMode(MODE_0_NORMAL);
	ResponseStructContainer c = e22ttl.getConfiguration();
	conf = *(Configuration*)c.data;

	conf.ADDL = 0xFF; // Example address (must match destination/relevant setup)
	conf.ADDH = 0xFF;
	conf.CHAN = 23; // For 433 MHz (E22-400), or set accordingly for your module
	// Optionally set spreading factor, air data rate, etc.
	conf.SPED.airDataRate = AIR_DATA_RATE_010_24; // 2.4kbps default

	// Save configuration and write to module
	e22ttl.setConfiguration(conf, WRITE_CFG_PWR_DWN_SAVE);
	c.close();

	// Configuration config = (Configuration) e22ttl.getConfiguration().data;
   
	// e22ttl.setConfiguration(config, WRITE_CFG_PWR_DWN_SAVE);



	Serial.println("Hi, I'm going to send message!");

	// Send message
	ResponseStatus rs = e22ttl.sendFixedMessage(conf.ADDL, conf.ADDH, conf.CHAN, "Hello, world?");
	// Check If there is some problem of succesfully send
	Serial.println(rs.getResponseDescription());
}

void loop()
{
	// If something available
	if (e22ttl.available() > 1)
	{
		// read the String message
#ifdef ENABLE_RSSI
		ResponseContainer rc = e22ttl.receiveMessageRSSI();
#else
		ResponseContainer rc = e22ttl.receiveMessage();
#endif
		// Is something goes wrong print error
		if (rc.status.code != 1)
		{
			Serial.println(rc.status.getResponseDescription());
		}
		else
		{
			// Print the data received
			Serial.println(rc.status.getResponseDescription());
			Serial.println(rc.data);
#ifdef ENABLE_RSSI
			Serial.print("RSSI: ");
			Serial.println(rc.rssi, DEC);
			Serial.println(rc.status.getResponseDescription());
			Serial.println(rc.data);
#endif
		}
	}
	if (Serial.available())
	{
		String input = Serial.readString();
		ResponseStatus rsSend = e22ttl.sendFixedMessage(conf.ADDL, conf.ADDH, conf.CHAN, "We have received the message!");
		// Check If there is some problem of succesfully send
		Serial.println(rsSend.getResponseDescription());
	}
}
