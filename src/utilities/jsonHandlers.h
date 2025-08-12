#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>

void sendJson(JsonDocument &doc, AsyncWebSocket &ws);
void receiveJson(uint8_t* data, size_t len);
void printJsonContents(const JsonDocument &doc);