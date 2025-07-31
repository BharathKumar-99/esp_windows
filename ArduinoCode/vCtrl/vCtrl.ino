#include <BleKeyboard.h> 
#include "encoder1.h"
#include "encoder2.h"
#include "encoder3.h"
#include "websocket.h"
#include "wifi_config.h" 

BleKeyboard bleKeyboard("MediaKnob", "ESP32", 100);

 
// --- Network Config ---
const char* ssid = "Ragnorok";
const char* password = "bharth123";
const char* websocket_server = "192.168.0.106"; 
const uint16_t websocket_port = 9343;

void setup() {
  Serial.begin(115200);
  bleKeyboard.begin();
  init1();
  init2();
  init3();
  initWebSocket();
  //
}

void loop() {
  makeloop();
    if(bleKeyboard.isConnected()) {
      encoder1Run(bleKeyboard);
      encoder2Run();
      encoder3Run();
    }
    else{
      Serial.print("Bluetooth Disconected");
      delay(1000);
    }
}
