#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <ArduinoJson.h>

//#include "ArduinoJson.h"
#include "utilities/jsonHandlers.h"
#include "tasks/webServer/webServer.h"
#include "globals/globals.h"

#include "neopixel/neopixel.h"

extern AsyncWebSocket ws;

void dataHandlerTask(void *pvParameters);
void receiveFromWebServer();
void loadData(JsonObject &payload);
bool sendToWebServer(JsonDocument &doc);

void initProcesses();
void manageProcesses(JsonDocument &doc);