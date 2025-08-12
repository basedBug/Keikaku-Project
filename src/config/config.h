#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "tasks/sysMonitor/sysMonitor.h"

// Neccesary globals



extern SemaphoreHandle_t wsMutex;

/*
    Tasks handles
    Used by FreeRTOS to setup tasks and for the system monitor
*/
extern TaskHandle_t webServerTaskHandle;
extern TaskHandle_t dataHandlerTaskHandle;
extern TaskHandle_t sysMonitorTaskHandle;
