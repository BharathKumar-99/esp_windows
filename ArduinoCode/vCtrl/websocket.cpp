#include "websocket.h"

WebSocketsClient webSocket;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED: Serial.println("[WSc] Disconnected!"); break;
    case WStype_CONNECTED: Serial.printf("[WSc] Connected to %s\n", payload); break;
    case WStype_TEXT: Serial.printf("[WSc] Received: %s\n", payload); break;
    default: break;
  }
}

void sendWebSocket(String message) {
  if (webSocket.isConnected()) {
    webSocket.sendTXT(message);
  }
}

void initWebSocket() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  webSocket.begin(websocket_server, websocket_port, "/");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(2000);
}

void makeloop() {
  webSocket.loop();
}
