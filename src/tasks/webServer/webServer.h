#pragma once

#include <WiFi.h>
#include <ESPmDNS.h>
#include <LittleFS.h>

#include "filesystem/filesystem.h"
#include "utilities/jsonHandlers.h"
#include "wifiSettings.h"

class CustomAsyncLoggingMiddleware : public AsyncLoggingMiddleware 
{
    void run(AsyncWebServerRequest *request, ArMiddlewareNext next) override;
};

void webServerTask(void *pvParameters);

void initWifi();
void initWebServer();
void initWebSocket();
void initMDNS();

