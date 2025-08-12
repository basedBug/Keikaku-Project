#pragma once

#include "ArduinoJson.h"

#include "config/config.h"
#include "Utilities/jsonHandlers.h"
#include "tasks/webServer/webServer.h"

extern AsyncWebSocket ws;

void dataHandlerTask(void *pvParameters);