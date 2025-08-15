#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <ArduinoJson.h>

//#include "ArduinoJson.h"
#include "utilities/jsonHandlers.h"
#include "tasks/webServer/webServer.h"
#include "globals/globals.h"

extern AsyncWebSocket ws;

void dataHandlerTask(void *pvParameters);
bool sendToWebServer(JsonDocument &doc);