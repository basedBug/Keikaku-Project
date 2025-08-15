#include <Arduino.h>

//#include <WiFi.h>
//#include <AsyncTCP.h>
//#include <ESPAsyncWebServer.h>
//#include <ESPmDNS.h>
//#include <LittleFS.h>
//#include <ArduinoJson.h>
//#include <AsyncJson.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "filesystem/filesystem.h"
#include "tasks/tasks.h"
#include "globals/globals.h"

// Task handles located in tasks header file

void setup()
{
	// Serial port for debug
	Serial.begin(512000);

	vTaskDelay(pdMS_TO_TICKS(2000));	// A little delay to permit me to connect the damn serial to my logger

	initFs();

	/*
	// Create websocket mutex
	wsMutex = xSemaphoreCreateMutex();
	if (wsMutex == NULL)
	{
		Serial.println("[RTOS] Failed to create websocket mutex");
		while(true);	// Idle due to error, restart needed
	}
	*/
	
	datahandlerToWsMessageBuffer = xMessageBufferCreate(BUFFER_SIZE);
	if (!datahandlerToWsMessageBuffer)
	{
		Serial.println("[RTOS] Failed to create TASK to WEBSERVER message buffer");
	}

	wsToDatahandlerTaskMessageBuffer = xMessageBufferCreate(BUFFER_SIZE);
	if (!wsToDatahandlerTaskMessageBuffer)
	{
		Serial.println("[RTOS] Failed to create WEBSERVER to TASK message buffer");
	}

	// Create tasks
	createTasks();
	
}

void loop()
{
}