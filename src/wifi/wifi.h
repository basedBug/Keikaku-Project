#pragma once

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>

#include "wifi/wifiSettings.h"

void initWifi();
void setWifiEventHandlers();

void evWifiReady(WiFiEvent_t event, WiFiEventInfo_t info);
void evWifiStationStarted(WiFiEvent_t event, WiFiEventInfo_t info);
void evWifiStationStopped(WiFiEvent_t event, WiFiEventInfo_t info);
void evWifiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info);
void evWifiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);
void evWifiStationGotIp(WiFiEvent_t event, WiFiEventInfo_t info);
void evWifiStationLostIp(WiFiEvent_t event, WiFiEventInfo_t info);