#include "tasks/dataHandler/dataHandler.h"


void dataHandlerTask(void *pvParameters)
{
	Serial.println("[DataHandler] Task started");
	
	initProcesses();

	uint32_t activeWsClients = 0;	// Just the same type as the notification value (simplifies math)
	uint32_t notificationValue;		// Apparently it needs to be uint32 in order to be compatible with
									// the task notification function

	TickType_t xLastWakeTime;
	const TickType_t xTimeInterval = pdMS_TO_TICKS(200);

	// Initialise the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();

	while (true)
	{
		/*
			Check if notification from the webserver (indicating active clients to send data to)
			was received. If not, theres no point in sending the data
		*/
		if (xTaskNotifyWait(
				0, 						// ulBitsToClearOnEntry, bits of the notification
										// 	value to clear when entering the notification wait
										// 	function. (Dont want to clear them, so 0)
				ULONG_MAX, 				// ulBitsToClearOnExit, bits of the notification
										// 	value to clear when before exiting the notification wait
										// 	function if a notification is received. (ULONG_MAX is all bits)
				&notificationValue, 	// pulNotificationValue, The value copied to *pulNotificationValue
										// 	is the RTOS task's notification value as it was before any 
										// 	bits were cleared due to the ulBitsToClearOnExit setting
				pdMS_TO_TICKS(0)		// Maximum time to wait in the Blocked state for a notification to 
										// 	be received if a notification is not already pending.
										// 	(Dont want to block, so 0ms)
			)
		)
		{
			Serial.print("[DataHandler] Received notification from webServer, value: ");
			Serial.println(notificationValue);
			activeWsClients = notificationValue;
		}

		// Reception of webserver data
		receiveFromWebServer();

		if (activeWsClients)
		{
		/*
				Sending of data to webserver
			Rate limited to not overwhelm the webserver connection
		*/
		if (xTaskGetTickCount() - xLastWakeTime >= xTimeInterval)
		{	
				sendDataToWebServer();

			xLastWakeTime = xTaskGetTickCount();
			}
		}
		
		vTaskDelay(pdMS_TO_TICKS(1));
			}
		}
		
void initProcesses()
{
	// The as5600 encoder is initalized by its own task
	initNeopixel();
}

void loadData(JsonObject &payload)
{
	payload["rand1"] = random(100);
	payload["rand2"] = random(100);
	
	getNeopixelState(payload); // Will load contents directly into payload
	getEncoderData(payload);
}

void manageProcesses(JsonDocument& doc)
{
	if (doc["as5600"].is<JsonObject>())
	{
		JsonObject encoderCmd = doc["as5600"].as<JsonObject>();
		handleEncoderCmd(encoderCmd);
	}

	if (doc["neopixel"].is<JsonObject>())
	{
		JsonObject neoCmd = doc["neopixel"].as<JsonObject>();
		handleNeopixelCmd(neoCmd);
	}
}

void receiveFromWebServer()
{
	char rxJsonMsgBuffer[MAX_MSG_SIZE];

	size_t receivedBytes = xMessageBufferReceive(
		wsToDatahandlerTaskMessageBuffer,	// Target message buffer handle
		rxJsonMsgBuffer,					// Pointer to the buffer for the received message
		sizeof(rxJsonMsgBuffer), 			// Length of the buffer for the received message
		pdMS_TO_TICKS(0)					// Max time this task should be in the Blocked state
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
			return;
		}
		
		// Print contents into serial
		printJsonContents(rx_doc);
		
		manageProcesses(rx_doc);
	}
}

void sendDataToWebServer()
{
	/*
		Any modifications made to the JSON object that references the doc
		are reflected into the original doc
	*/
	JsonDocument tx_doc;
	JsonObject tx_data = tx_doc.to<JsonObject>();
	
	// Load up the data
	loadData(tx_data);
	
	sendToWebServer(tx_doc);
}

bool sendToWebServer(JsonDocument &doc)
{
	// Maybe (FUTURE) replace the buffer with thread-safe allocation?
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
		Serial.println("[DataHandler] Warning: Message buffer to WebServer full, message dropped");
		return false;
	}

	//Serial.printf("[DataHandler] Sent JSON message of size: %u \n", sentBytes);
	return true;
}