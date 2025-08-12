#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void sysMonitorTask(void *pvParameters);

void printTasksStats();

const char* taskStatusToString(eTaskState state);