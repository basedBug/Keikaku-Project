#include "tasks/webServer/webServer.h"

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

CustomAsyncLoggingMiddleware requestLogger; // Thanks to https://github.com/ESP32Async/ESPAsyncWebServer/blob/main/examples/Logging/Logging.ino

void webServerTask(void *pvParameters)
{
	initWifi();
	initWebServer();
    initWebSocket();

    // Start TCP (HTTP) web server
	server.begin();
	Serial.println("[Web] TCP web server started");

    initMDNS();
    
	/*
		Just to keep the task alive
		Cant just delete the task, the lost references would break the system
	*/
	while (true)
	{
		vTaskDelay(pdTICKS_TO_MS(1000));
	}
}

void initWifi()
{
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
}

void initWebSocket()
{
    ws.onEvent([](AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len)
	{
		switch (type)
		{
			case WS_EVT_CONNECT:
				Serial.printf("[Web] WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
				
				//client->
				/*
				ws.textAll("new client connected");
				Serial.println("ws connect");
				client->setCloseClientOnQueueFull(false);
				client->ping();
				*/
				break;

			case WS_EVT_DISCONNECT:
				Serial.printf("[Web] WebSocket client #%u disconnected\n", client->id());
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

						receiveJson(data, len);
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

void initWebServer()
{
	requestLogger.setEnabled(true);

	// Needed as the middleware bitches about not having an output (altough i hardcoded it)
	requestLogger.setOutput(Serial);

	server.addMiddleware(&requestLogger);

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

void initMDNS()
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

void CustomAsyncLoggingMiddleware::run(AsyncWebServerRequest *request, ArMiddlewareNext next) 
{
	if (!isEnabled()) 
	{
		next();
		return;
	}
	Serial.print(F("[Web] * Connection from "));
	#ifndef LIBRETINY
	Serial.print(request->client()->remoteIP().toString());
	#else
	Serial.print(request->client()->remoteIP());
	#endif
	Serial.print(':');
	Serial.println(request->client()->remotePort());
	
	Serial.print(F("[Web] > "));
	Serial.print(request->methodToString());
	Serial.print(' ');
	Serial.print(request->url().c_str());
	Serial.print(F(" HTTP/1."));
	Serial.println(request->version());

	for (auto &h : request->getHeaders()) {
		if (h.value().length()) {
		Serial.print(F("[Web] > "));
		Serial.print(h.name());
		Serial.print(':');
		Serial.print(' ');
		Serial.println(h.value());
		}
	}
	Serial.print(F("[Web] > "));
	
	uint32_t elapsed = millis();
	next();
	elapsed = millis() - elapsed;
	AsyncWebServerResponse *response = request->getResponse();
	if (response) 
	{
		Serial.print(F("* Processed in "));
		Serial.print(elapsed);
		Serial.println(F(" ms"));

		Serial.print(F("[Web] < HTTP/1."));
		Serial.print(request->version());
		Serial.print(' ');
		Serial.print(response->code());
		Serial.print(' ');
		Serial.println(AsyncWebServerResponse::responseCodeToString(response->code()));

		for (auto &h : response->getHeaders()) {
			if (h.value().length()) 
			{
				Serial.print(F("[Web] < "));
				Serial.print(h.name());
				Serial.print(':');
				Serial.print(' ');
				Serial.println(h.value());
			}
		}
		Serial.println(F("[Web] <"));
	} 
	else 
	{
		Serial.println(F("[Web] * Connection closed!"));
	}
}