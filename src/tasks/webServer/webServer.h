#pragma once

#include <WiFi.h>
#include <ESPmDNS.h>
#include <LittleFS.h>

#include "config/config.h"
#include "wifiSettings.h"
#include "utilities/jsonHandlers.h"

class CustomAsyncLoggingMiddleware : public AsyncLoggingMiddleware 
{
    void run(AsyncWebServerRequest *request, ArMiddlewareNext next) override;
};

void webServerTask(void *pvParameters);

void initializeWebServer();
void initializeWebSocket();
void initializeMDNS();

