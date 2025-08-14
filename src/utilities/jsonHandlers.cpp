#include "utilities/jsonHandlers.h"

/*
    Change to json msgPack serialization and deserealization
    It has performance improvements in latency and data usage

    https://github.com/fabianoriccardi/benchmark-json-messagepack
    https://github.com/ESP32Async/ESPAsyncWebServer/discussions/26

    In fact, it is used in 
    https://github.com/ayushsharma82/ESP-DASH/blob/master/src/ESPDash.cpp
*/

void sendJson(JsonDocument &doc, AsyncWebSocket &ws)
{
    // Buffer to speed up JSON serialization
	const size_t len = measureMsgPack(doc);
	AsyncWebSocketMessageBuffer *buffer = ws.makeBuffer(len);
	if (!buffer) // Buffer initialization check
		return;
	serializeMsgPack(doc, buffer->get(), len);

    // Messagepack encodes in binary data
	ws.binaryAll(buffer); // Send data

    // Free JSON doc memory contents (doesnt delete the JSON object, but frees memory)
    doc.clear(); 
}

void receiveJson(uint8_t* data, size_t len)
{
    JsonDocument rx_doc;

    DeserializationError error = deserializeMsgPack(rx_doc, data, len);
    if (error)
    {
        Serial.print("[Web] JSON parse error:");
        Serial.println(error.f_str());
        return;
    }
    
    // Print contents into serial
    printJsonContents(rx_doc);

    /*
        JSON isnt cleared here, as it's data will be extracted and used by 
        other functions down the line
    */
}

void printJsonContents(const JsonDocument &doc)
{
    Serial.println(F("[Web] Received JSON:"));

    JsonObjectConst obj = doc.as<JsonObjectConst>();
    for (JsonPairConst keyvalue : obj) // Iterate over every keyvalue (element)
    {
        const char *key = keyvalue.key().c_str();
        JsonVariantConst value = keyvalue.value();

        if (keyvalue.value().is<const char*>())
            Serial.printf("[Web]   %s: \"%s\"\n", key, value.as<const char*>());
        else if (keyvalue.value().is<int>())
            Serial.printf("[Web]   %s: %d\n", key, value.as<int>());
        else if (keyvalue.value().is<unsigned int>())
            Serial.printf("[Web]   %s: %u\n", key, value.as<unsigned int>());
        else if (keyvalue.value().is<bool>())
            Serial.printf("[Web]   %s: %s\n", key, value.as<bool>() ? "true" : "false");
        else if (keyvalue.value().is<float>())
            Serial.printf("[Web]   %s: %.2f\n", key, value.as<float>());
        else if (keyvalue.value().is<JsonArrayConst>())
            Serial.printf("[Web]   %s: {jsonArray}\n", key);
        else if (keyvalue.value().is<JsonObjectConst>())
            Serial.printf("[Web]   %s: {jsonObject}\n", key);
        else if (keyvalue.value().isNull())
            Serial.printf("[Web]   %s: null\n", key);
        else
            Serial.printf("[Web]   %s: (unhandled type)\n", key);
    }
}