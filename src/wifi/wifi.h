#pragma once

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>

#include "wifi/wifiSettings.h"

void WiFiEvent();
void initWifi();
