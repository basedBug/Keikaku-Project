#include <WiFi.h>
#include <ESPmDNS.h>
#include <LittleFS.h>

#include "tasks/webServer/webServer.h"
#include "wifiSettings.h"
#include "utilities/jsonHandlers.h"


void webServerTask(void *pvParameters)
{
	// Initialize wifi
	WiFi.begin(ssid, password);
	Serial.println("[WiFi] Connecting to wifi...");
	
    while (WiFi.status() != WL_CONNECTED)
	{
		vTaskDelay(pdMS_TO_TICKS(500));
		Serial.println(".");
	}

	Serial.print("Connected to ");
	Serial.println(ssid);
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());

	initializeWebServer();
    initializeWebSocket();

    // Start TCP (HTTP) web server
	server.begin();
	Serial.println("[Web] TCP web server started");

    initializeMDNS();
    /*
	// Add service to mDNS-SD
	MDNS.addService("http", "tcp", mdnsPort);
	Serial.print("[mDNS] Service 'http' added on port: ");
	Serial.println(mdnsPort);
    */
    
	/*
		Just to keep the task alive
		Cant just delete the task, the lost references would break the system
	*/
	while (true)
	{
		vTaskDelay(pdTICKS_TO_MS(1000));
	}
}

void initializeWebSocket()
{
    ws.onEvent([](AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len)
	{
		switch (type)
		{
			case WS_EVT_CONNECT:
				Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
				/*
				ws.textAll("new client connected");
				Serial.println("ws connect");
				client->setCloseClientOnQueueFull(false);
				client->ping();
				*/
				break;

			case WS_EVT_DISCONNECT:
				Serial.printf("[Web] WebSocket client #%u disconnected\n", client->id());
				/*
				ws.textAll("client disconnected");
				Serial.println("ws disconnect");
				*/
				break;

			case WS_EVT_ERROR:
				Serial.println("[Web] ws error");
				break;

			case WS_EVT_PONG:
				Serial.println("[Web] ws pong");
				break;

			case WS_EVT_DATA:
			{
				AwsFrameInfo* info = (AwsFrameInfo*)arg;
				//Serial.printf("[Web] index: %" PRIu64 ", len: %" PRIu64 ", final: %" PRIu8 ", opcode: %" PRIu8 "\n", info->index, info->len, info->final, info->opcode);
				
				if (info->final && info->index == 0 && info->len == len) 
				{
					if (info->opcode == WS_TEXT)
					{
						/*
							data[len] = 0; // Set string read limit via 0
							Serial.printf("ws text: %s\n", (char*)data);
						*/

						JsonDocument rx_doc;

						DeserializationError error = deserializeJson(rx_doc, data, len);
						if (error)
						{
							Serial.print("[Web] JSON parse error:");
							Serial.println(error.f_str());
							return;
						}
						
						// Print contents into serial
						printJsonContents(rx_doc);
						/*
						const char *brand = rx_doc["brand"];
						const char *type = rx_doc["type"];
						const int year = rx_doc["year"];
						const char *value = rx_doc["value"];

						Serial.println("Received data:");
						Serial.print("brand:"); Serial.println(brand);
						Serial.print("type:"); Serial.println(type);
						Serial.print("year:"); Serial.println(year);
						Serial.print("value:"); Serial.println(value);
						*/
					}
				}
				break;

			}
			default:
				break;
		}
	});
    
	// Add handler for websocket on the server
	server.addHandler(&ws);
}

void initializeWebServer()
{
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request)
	{
		request->send(LittleFS, "/index.html", "text/html");
	});
    
    server.onNotFound([](AsyncWebServerRequest* request)
	{
		request->send(LittleFS, "/not_found.html", "text/html");
	});

	server.on("/images/bruh_moment-min.jpg", HTTP_GET, [](AsyncWebServerRequest* request)
	{
		// apparently MIME type needs to be jpeg not jpg
		request->send(LittleFS, "/images/bruh_moment-min.jpg", "image/jpeg");
	});
}

void initializeMDNS()
{
    // Set up mDNS responder
	if (!MDNS.begin("esp32")) // "esp32.local"
	{
		Serial.println("[mDNS] Error setting up MDNS responder");
		while (true)
		{
			//delay(1000);
			vTaskDelay(pdMS_TO_TICKS(1000));
		}
	}
	Serial.println("[mDNS] mDNS responder started");

    // Add service to mDNS service discovery
	MDNS.addService("http", "tcp", mdnsPort);
	Serial.print("[mDNS] Service 'http' added on port: ");
	Serial.println(mdnsPort);
}