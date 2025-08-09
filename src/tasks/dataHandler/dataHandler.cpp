#include "tasks/dataHandler/dataHandler.h"


void dataHandlerTask(void *pvParameters)
{
	TickType_t xLastWakeTime;
	const TickType_t xTimeInterval = pdMS_TO_TICKS(1000);

	// Initialise the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();

	Serial.println("[DataHandler] Task started");

	while (true)
	{
		// Wait for the next cycle
		vTaskDelayUntil(&xLastWakeTime, xTimeInterval);

		/*
			Any modifications made to the JSON object that references the doc
			are reflected into the original doc
		*/
		JsonDocument tx_doc;
		JsonObject tx_data = tx_doc.to<JsonObject>();
		
		// Load up the data
		tx_data["rand1"] = random(100);
		tx_data["rand2"] = random(100);

		// Need to protect the websocket access with a mutex
		if (xSemaphoreTake(wsMutex, portMAX_DELAY) == pdTRUE)
		{
			sendJson(tx_doc, ws);
			xSemaphoreGive(wsMutex);
		}
	}
}