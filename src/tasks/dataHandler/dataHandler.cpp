#include "tasks/dataHandler/dataHandler.h"


void dataHandlerTask(void *pvParameters)
{
	char rxJsonMsgBuffer[MAX_MSG_SIZE];

	//TickType_t xLastWakeTime;
	//const TickType_t xTimeInterval = pdMS_TO_TICKS(1000);

	// Initialise the xLastWakeTime variable with the current time.
    //xLastWakeTime = xTaskGetTickCount();

	Serial.println("[DataHandler] Task started");

	while (true)
	{
		// Reception of data
		size_t receivedBytes = xMessageBufferReceive(
			wsToDatahandlerTaskMessageBuffer,	// Target message buffer handle
			rxJsonMsgBuffer,					// Pointer to the buffer for the received message
			sizeof(rxJsonMsgBuffer), 			// Length of the buffer for the received message
			pdMS_TO_TICKS(50)					// Max time this task should be in the Blocked state
												// waiting for a message, if there buffer is empty
		);
		
		if (receivedBytes > 0)
		{
			// Parse and process the JSON
			JsonDocument rx_doc;
			
			DeserializationError error = deserializeJson(rx_doc, rxJsonMsgBuffer, receivedBytes);
			if (error)
			{
				Serial.printf("[Web] JSON parse error: %s \n", error.c_str());
				//Serial.println(error.f_str());
			}
			
			// Print contents into serial
			printJsonContents(rx_doc);
		}
		
		// Sending of data
		/*
		Any modifications made to the JSON object that references the doc
		are reflected into the original doc
		*/
		JsonDocument tx_doc;
		JsonObject tx_data = tx_doc.to<JsonObject>();
		
		// Load up the data
		tx_data["rand1"] = random(100);
		tx_data["rand2"] = random(100);

		sendToWebServer(tx_doc);

		//serializeJson(tx_doc, buffer->get(), len);

		/*
		size_t sentBytes = xMessageBufferSend(
			taskWsMessageBuffer,	// Target message buffer handle
			data,					// Pointer to data being sent
			len, 					// Length of the message
			pdMS_TO_TICKS(10)		// Max time this task should be the in Blocked state
									// for enough space in the buffer, if there's 
									// insufficient space when the call is made
		);
		*/
		
		//vTaskDelayUntil(&xLastWakeTime, xTimeInterval);
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}

bool sendToWebServer(JsonDocument &doc)
{
	char txJsonMsgBuffer[MAX_MSG_SIZE];
	const size_t len = measureJson(doc);
	if (len == 0) 
	{
		Serial.println("[DataHandler] Warning: Tried to send JSON message of size 0");
		return false;
	}
	if (len > sizeof(txJsonMsgBuffer))
	{
		Serial.printf("[DataHandler] Warning: JSON message %u bigger than message buffer %u, dropping JSON \n",
			len, 
			sizeof(txJsonMsgBuffer)
		);
		return false;
	}

	serializeJson(doc, txJsonMsgBuffer, len);

	size_t sentBytes = xMessageBufferSend(
		datahandlerToWsMessageBuffer,	// Target message buffer handle
		txJsonMsgBuffer,				// Pointer to data being sent
		len, 							// Length of the message
		pdMS_TO_TICKS(50)				// Max time this task should be the in Blocked state
										// for enough space in the buffer, if there's 
										// insufficient space when the call is made
	);

	if (sentBytes != len) {
		Serial.println("[DataHandler] Warning: Message buffer full, message dropped");
		return false;
	}

	//Serial.printf("[DataHandler] Sent JSON message of size: %u \n", sentBytes);
	return true;
}