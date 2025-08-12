#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "ArduinoJson.h"
#include "config/config.h"
#include "Utilities/jsonHandlers.h"
#include "tasks/webServer/webServer.h"

extern AsyncWebSocket ws;

void dataHandlerTask(void *pvParameters);