#include "lora_module.h"

void lora_module::begin(){
    Serial.println("Initializing LoRa...");
    LoRa.setPins(LORA_CS_PIN, LORA_RST_PIN, LORA_DO1_PIN);
    int lora_status = LoRa.begin(LORA_FREQ);
    if (lora_status != 1) {
        Serial.printf("LoRa init failed. Status %d\n", lora_status);
        return;
    }
    Serial.printf("LoRa Initialized. Setting Parameters...\n");
    Serial.printf("LoRa Parameters set. Setting TX Power...\n");
    LoRa.setTxPower(3);
    Serial.printf("LoRa TX Power set. Setting Callbacks...\n");
    LoRa.onReceive(nullptr);
    LoRa.onTxDone(nullptr);
    Serial.printf("LoRa Callbacks set. Putting in Idle mode...\n");
    LoRa.idle();
    Serial.printf("LoRa in Idle mode. LoRa Initialization Complete.\n");
}

int lora_module::sendPacket(String message){
    LoRa.beginPacket();
    LoRa.print(message);
    return LoRa.endPacket();
}  