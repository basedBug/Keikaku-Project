#pragma once

//#include <Arduino.h>
//#include <ESPAsyncWebServer.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// Neccesary globals (only used by dataHandler task for now)
//extern SemaphoreHandle_t wsMutex;

extern MessageBufferHandle_t datahandlerToWsMessageBuffer;
extern MessageBufferHandle_t wsToDatahandlerTaskMessageBuffer;
constexpr size_t MAX_MSG_SIZE = 1024;
constexpr size_t BUFFER_SIZE = MAX_MSG_SIZE*2;


