#include <Arduino.h>

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "config/config.h"
#include "tasks/webServer/webServer.h"
#include "tasks/dataHandler/dataHandler.h"
#include "tasks/taskMonitor/taskMonitor.h"

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");


// Task handles located in the config file

// Mutex for protecting the websocket access
SemaphoreHandle_t wsMutex = NULL;

void setup()
{
	// Serial port for debug
	Serial.begin(500000);

	// Initialize LittleFS
	if (!LittleFS.begin())
	{
		Serial.println("[FS] Failed to mount LittleFS");
		while (true);	// Idle due to error, restart needed
	}
	Serial.println("[FS] LittleFS mounted successfully");

	// Create websocket mutex
	wsMutex = xSemaphoreCreateMutex();
	if (wsMutex == NULL)
	{
		Serial.println("[RTOS] Failed to create websocket mutex");
		while(true);	// Idle due to error, restart needed
	}
	
	// Create tasks
	xTaskCreatePinnedToCore(
		webServerTask,			// Task function to be called
		"WebServer",			// Task name (for debug)
		10000,					// Stack size (bytes)
		NULL,					// Task parameters
		1,						// Priority
		&webServerTaskHandle,	// Task handle
		1						// Core to run on
	);

	xTaskCreatePinnedToCore(
		dataHandlerTask,		// Task function to be called
		"dataHandler",			// Task name (for debug)
		10000,					// Stack size (bytes)
		NULL,					// Task parameters
		2,						// Priority
		&dataHandlerTaskHandle,	// Task handle
		1						// Core to run on
	);

	xTaskCreatePinnedToCore(
		monitorTask,			// Task function to be called
		"monitorTask",			// Task name (for debug)
		7000,					// Stack size (bytes)
		NULL,					// Task parameters
		3,						// Priority
		&monitorTaskHandle,	// Task handle
		1						// Core to run on
	);
}

void loop()
{
}