#include "utilities/jsonHandlers.h"

/*
void sendJson(JsonDocument &doc, AsyncWebSocket &ws)
{
	const size_t len = measureJson(doc);
	AsyncWebSocketMessageBuffer *buffer = ws.makeBuffer(len);
	if (!buffer) // Buffer initialization check
    return;
	serializeJson(doc, buffer->get(), len);
    
	ws.textAll(buffer);
}

void receiveJson(char* data, size_t len)
{
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
}
*/

void printJsonContents(const JsonDocument &doc)
{
    Serial.println(F("[Web] Received JSON from server:"));

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