#include "tasks/tasks.h"

TaskHandle_t webServerTaskHandle = NULL;
TaskHandle_t dataHandlerTaskHandle = NULL;
TaskHandle_t sysMonitorTaskHandle = NULL;

void createTasks()
{
    xTaskCreatePinnedToCore(
		webServerTask,			// Task function to be called
		"WebServer",			// Task name (for debug)
		10000,					// Stack size (bytes)
		NULL,					// Task parameters
		3,						// Priority
		&webServerTaskHandle,	// Task handle
		0						// Core to run on
	);

	xTaskCreatePinnedToCore(
		dataHandlerTask,		// Task function to be called
		"dataHandler",			// Task name (for debug)
		10000,					// Stack size (bytes)
		NULL,					// Task parameters
		1,						// Priority
		&dataHandlerTaskHandle,	// Task handle
		1						// Core to run on
	);

	xTaskCreatePinnedToCore(
		sysMonitorTask,			// Task function to be called
		"sysMonitor",			// Task name (for debug)
		7000,					// Stack size (bytes)
		NULL,					// Task parameters
		3,						// Priority
		&sysMonitorTaskHandle,	// Task handle
		1						// Core to run on
	);
}