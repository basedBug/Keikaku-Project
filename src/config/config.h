#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "tasks/taskMonitor/taskMonitor.h"

// Neccesary globals
extern AsyncWebServer server;
extern AsyncWebSocket ws;
extern SemaphoreHandle_t wsMutex;

/*
    Tasks handles
    Used by FreeRTOS to setup tasks and for the taskMonitor
*/
extern TaskHandle_t webServerTaskHandle;
extern TaskHandle_t dataHandlerTaskHandle;
extern TaskHandle_t monitorTaskHandle;

/*
    Utility handles
*/
//extern TaskMonitor taskMonitor;
