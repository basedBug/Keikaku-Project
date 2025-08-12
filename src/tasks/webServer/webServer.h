#pragma once

#include <WiFi.h>
#include <ESPmDNS.h>
#include <LittleFS.h>

#include "utilities/jsonHandlers.h"
#include "wifiSettings.h"

class CustomAsyncLoggingMiddleware : public AsyncLoggingMiddleware 
{
    void run(AsyncWebServerRequest *request, ArMiddlewareNext next) override;
};

void webServerTask(void *pvParameters);

void initializeWebServer();
void initializeWebSocket();
void initializeMDNS();

