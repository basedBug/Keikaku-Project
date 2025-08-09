#pragma once

#include "config/config.h"

void webServerTask(void *pvParameters);

void initializeWebServer();
void initializeWebSocket();
void initializeMDNS();