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
#include "filesystem/filesystem.h"
#include "tasks/tasks.h"

// Task handles located in the config file

// Mutex for protecting the websocket access
SemaphoreHandle_t wsMutex = NULL;

void setup()
{
	// Serial port for debug
	Serial.begin(512000);

	vTaskDelay(pdMS_TO_TICKS(1000));	// A little delay to permit me to connect the damn serial to my logger

	initFs();

	// Create websocket mutex
	wsMutex = xSemaphoreCreateMutex();
	if (wsMutex == NULL)
	{
		Serial.println("[RTOS] Failed to create websocket mutex");
		while(true);	// Idle due to error, restart needed
	}
	
	// Create tasks
	createTasks();
	
}

void loop()
{
}