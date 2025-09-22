#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <ArduinoJson.h>

//#include "ArduinoJson.h"
#include "utilities/jsonHandlers.h"
#include "tasks/webServer/webServer.h"
#include "tasks/magEncoder/magEncoder.h"
#include "neopixel/neopixel.h"
#include "globals/globals.h"


extern AsyncWebSocket ws;

void dataHandlerTask(void *pvParameters);

void checkWebServerNotification(uint32_t &flagVar);
void receiveFromWebServer();
void sendDataToWebServer();
bool sendToWebServer(JsonDocument &doc);

void loadData(JsonObject &payload);
void initProcesses();
void manageProcesses(JsonDocument &doc);