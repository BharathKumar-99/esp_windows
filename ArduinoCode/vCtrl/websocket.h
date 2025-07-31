#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <WiFi.h>
#include <WebSocketsClient.h>
#include "wifi_config.h"

extern WebSocketsClient webSocket;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);
void sendWebSocket(String message);
void initWebSocket();
void makeloop();

#endif
