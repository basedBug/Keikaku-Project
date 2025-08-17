#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "tasks/webServer/webServer.h"
#include "tasks/dataHandler/dataHandler.h"
#include "tasks/sysMonitor/sysMonitor.h"

extern TaskHandle_t webServerTaskHandle;
extern TaskHandle_t dataHandlerTaskHandle;
extern TaskHandle_t sysMonitorTaskHandle;

void createTasks();