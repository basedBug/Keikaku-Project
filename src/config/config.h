#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// Neccesary globals (only used by dataHandler task for now)
extern SemaphoreHandle_t wsMutex;


