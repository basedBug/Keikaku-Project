#pragma once

#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <ESPmDNS.h>
//#include <LittleFS.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/message_buffer.h>

#include "filesystem/filesystem.h"
#include "utilities/jsonHandlers.h"
#include "wifi/wifi.h"
#include "globals/globals.h"

class CustomAsyncLoggingMiddleware : public AsyncLoggingMiddleware 
{
    void run(AsyncWebServerRequest *request, ArMiddlewareNext next) override;
};

void webServerTask(void *pvParameters);

//void initWifi();
void initWebServer();
void initWebSocket();
void onSocketEvents(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len);
void initMDNS();

