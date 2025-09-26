// #include "Arduino.h"
// #include "LoRa_E22.h"

// LoRa_E22 e22ttl(&Serial1); // WeMos RX --> e22 TX - WeMos TX --> e22 RX

// #define DESTINATION_ADDL 7

// void setup()
// {
//     Serial.begin(115200);
//     delay(500);

//     Serial.println("Hi, I'm going to send message!");

//     // Startup all pins and UART

//     e22ttl.begin();
//     e22ttl.setMode(MODE_0_NORMAL);

//     delay(1000);

//     // Send message
//     ResponseStatus rs = e22ttl.sendFixedMessage(0xFF, 0xFF, 1, "Hello, world?");
//     // Check If there is some problem of successfully send
//     Serial.println(rs.getResponseDescription());
// }

// void loop()
// {
//     // If something available
//     if (e22ttl.available() > 1)
//     {
//         // read the String message
//         ResponseContainer rc = e22ttl.receiveMessage();
//         // Is something goes wrong print error
//         if (rc.status.code != 1)
//         {
//             rc.status.getResponseDescription();
//         }
//         else
//         {
//             // Print the data received
//             Serial.println(rc.data);
//         }
//     }
//     if (Serial.available())
//     {
//         String input = Serial.readString();
//         ResponseStatus rs = e22ttl.sendFixedMessage(0xFF, 0xFF, 1, input);
//         // Check If there is some problem of successfully send
//         Serial.println(rs.getResponseDescription());
//     }
// }