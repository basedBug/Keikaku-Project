#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "config/config.h"

void monitorTask(void *pvParameters);

void printTasksStats();
const char* taskStatusToString(eTaskState state);

/*
class TaskMonitor
{
    public:
    static void printTasksStats();
    static void checkStackUsage();
};
*/

//extern TaskMonitor taskMonitor;