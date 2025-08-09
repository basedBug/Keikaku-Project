#include "tasks/taskMonitor/taskMonitor.h"

//TaskMonitor taskMonitor;

void monitorTask(void *pvParameters)
{
    TickType_t xLastPrintTime;
    const TickType_t xTimeInterval = pdMS_TO_TICKS(10000); // 10s

    // Initialise the xLastWakeTime variable with the current time.
    xLastPrintTime = xTaskGetTickCount();

    Serial.println("[Monitor] Task started");

    while (true)
    {
        // Wait for the next cycle
        vTaskDelayUntil(&xLastPrintTime, xTimeInterval);
    
        printTasksStats();
    }
}

void printTasksStats()
{
    char lineBuffer[64];

    Serial.println("\n[Task Monitor] === SYSTEM STATUS ===");
    Serial.printf("Free heap: %u bytes\n", ESP.getFreeHeap());
    Serial.printf("Min free heap: %u bytes\n", ESP.getMinFreeHeap());
    Serial.printf("Max alloc heap: %u bytes\n", ESP.getMaxAllocHeap());

    TaskStatus_t *pxTaskStatusArray;
    volatile UBaseType_t uxArraySize = uxTaskGetNumberOfTasks(); // Volatile as it may change at any moment

    pxTaskStatusArray = (TaskStatus_t *)pvPortMalloc(uxArraySize * sizeof(TaskStatus_t));

    if(pxTaskStatusArray != NULL)
    {
        uxArraySize = uxTaskGetSystemState(pxTaskStatusArray, uxArraySize, NULL);

        Serial.println("-----------------------------------------------------");
        Serial.println("| Task Name       | State   | Prio | Stack Free |");
        Serial.println("-----------------------------------------------------");

        for(UBaseType_t taskId = 0; taskId < uxArraySize; taskId++)
        {
            //Serial.printf("%s\t%s\t%u\t\t%u\n",
            snprintf(lineBuffer, sizeof(lineBuffer), "| %-15s | %-7s | %-4u | %-9u |",
                pxTaskStatusArray[taskId].pcTaskName,
                taskStatusToString(pxTaskStatusArray[taskId].eCurrentState),
                pxTaskStatusArray[taskId].uxCurrentPriority,
                pxTaskStatusArray[taskId].usStackHighWaterMark
            );
            Serial.println(lineBuffer);
        }
        Serial.println("-----------------------------------------------------");

        vPortFree(pxTaskStatusArray);
    }
    else
        Serial.println("[ERROR] Could not allocate memory for tasks stats");
}


// Helper function to return states in string form
const char* taskStatusToString(eTaskState state)
{
    switch (state)
    {
        case eRunning : return "Running";
        case eReady : return "Ready";
        case eBlocked : return "Blocked";
        case eSuspended : return "Suspended";
        case eDeleted : return "Deleted";
        default : return "Unknown";
    }
}