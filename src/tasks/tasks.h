#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "tasks/webServer/webServer.h"
#include "tasks/dataHandler/dataHandler.h"
#include "tasks/sysMonitor/sysMonitor.h"

void createTasks();