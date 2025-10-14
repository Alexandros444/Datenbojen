// #include "Arduino.h"
// #include "LoRa_E22.h"
// #include <ArduinoJson.h>


// LoRa_E22 e22ttl(&Serial1, 18, 19, 21); //  RX AUX M0 M1

// // #define DESTINATION_ADDL 7

// #define CHANNEL 0
// #define SEND_INTERVAL 20000

// JsonDocument json;

// void setup()
// {
//     Serial.begin(115200);
//     delay(500);

//     Serial1.begin(9600, SERIAL_8N1, 14, 13); // RX, TX

//     Serial.println("Hi, I'm going to send message!");

//     // Startup all pins and UART

//     e22ttl.begin();
//     e22ttl.setMode(MODE_0_NORMAL);

//     delay(1000);

//     // Send message
//     // ResponseStatus rs = e22ttl.sendFixedMessage(0xFF, 0xFF, CHANNEL, "Hello, world?");
//     // // Check If there is some problem of successfully send
//     // Serial.println(rs.getResponseDescription());

//     json["Node ID"] = "0x01";
//     json["Temperature"] = 24.5;
//     json["Humidity"] = 60;
//     json["Pressure"] = 1013.25;
//     json["Battery"] = 3.7;
// }

// void loop()
// {
//     // If something available
//     if (e22ttl.available() > 1)
//     {
//         // read the String message
//         // ResponseContainer rc = e22ttl.receiveMessage();
//         ResponseContainer rc = e22ttl.receiveMessageRSSI();
        
//         // Is something goes wrong print error
//         if (rc.status.code != 1)
//         {
//             rc.status.getResponseDescription();
//         }
//         else
//         {
//             // Print the data received
//             Serial.println(rc.data);
//             Serial.print("RSSI: ");
// 			Serial.println(rc.rssi, DEC);
//         }
//     }
    
//     static ulong last_ping = 0;
//     if (millis() - last_ping > SEND_INTERVAL)
//     {
//         last_ping = millis();
//         String data;
//         serializeJson(json, data);
//         ResponseStatus rs = e22ttl.sendFixedMessage(0xFF, 0xFF, CHANNEL, data);
//         // Check If there is some problem of successfully send
//         Serial.println(rs.getResponseDescription());
//     }
// }