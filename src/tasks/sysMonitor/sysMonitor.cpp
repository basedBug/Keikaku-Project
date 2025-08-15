#include "tasks/sysMonitor/sysMonitor.h"

void sysMonitorTask(void *pvParameters)
{
    TickType_t xLastPrintTime;
    const TickType_t xTimeInterval = pdMS_TO_TICKS(10000); // 10s

    // Initialise the xLastWakeTime variable with the current time.
    xLastPrintTime = xTaskGetTickCount();

    Serial.println("[Sys Monitor] Task started");

    while (true)
    {
        // Wait for the next cycle
        vTaskDelayUntil(&xLastPrintTime, xTimeInterval);
    
        Serial.println("\n[Sys Monitor] === SYSTEM STATUS ===");

        // Get the total free size of all the heap memory regions
        Serial.printf("[Sys Monitor] Free heap: %u bytes\n", ESP.getFreeHeap());              //

        /* 
            This adds all the low watermarks of the heap regions. This result gives a "worst case"
            indication for all-time minimum free heap.
        */
        Serial.printf("[Sys Monitor] Min free heap: %u bytes\n", ESP.getMinFreeHeap());       //

        // Get the largest free block of heap memory able to be allocated.
        Serial.printf("[Sys Monitor] Max alloc heap: %u bytes\n", ESP.getMaxAllocHeap());

        //printTasksStats();
    }
}

void printTasksStats()
{
    char lineBuffer[128];    // Increased from 64 to avoid any problems at all (64 should be enough)

    TaskStatus_t *pxTaskStatusArray;

    // This causes a momentary noticeable lag
    volatile UBaseType_t uxArraySize = uxTaskGetNumberOfTasks(); // Volatile as it may change at any moment

    pxTaskStatusArray = (TaskStatus_t *)pvPortMalloc(uxArraySize * sizeof(TaskStatus_t));

    if(pxTaskStatusArray != NULL)
    {
        uxArraySize = uxTaskGetSystemState(pxTaskStatusArray, uxArraySize, NULL);

        Serial.println("[Sys Monitor] --------------------------------------------------");
        Serial.println("[Sys Monitor] | Task Name      | State     | Prio | Stack Free |");
        Serial.println("[Sys Monitor] --------------------------------------------------");

        for(UBaseType_t taskId = 0; taskId < uxArraySize; taskId++)
        {
            //Serial.printf("%s\t%s\t%u\t\t%u\n",
            snprintf(lineBuffer, sizeof(lineBuffer), "[Sys Monitor] | %-15s | %-9s | %-4u | %-9u |",
                pxTaskStatusArray[taskId].pcTaskName,
                taskStatusToString(pxTaskStatusArray[taskId].eCurrentState),
                pxTaskStatusArray[taskId].uxCurrentPriority,
                pxTaskStatusArray[taskId].usStackHighWaterMark
            );
            Serial.println(lineBuffer);
        }
        Serial.println("[Sys Monitor] --------------------------------------------------");

        vPortFree(pxTaskStatusArray);
    }
    else
        Serial.println("[Sys Monitor] ERROR! Could not allocate memory for tasks stats");
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