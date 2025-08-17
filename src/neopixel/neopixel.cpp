#include "neopixel/neopixel.h"

const uint8_t NUM_LEDS = 1;
const gpio_num_t NEOPIXEL_PIN = GPIO_NUM_48;

CRGB leds[NUM_LEDS];

void initNeopixel()
{
    FastLED.addLeds<WS2811, NEOPIXEL_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(32);
}

void handleNeopixel(JsonObject neopixelCmd)
{
    bool updateNeeded = false;

    //if (JsonObject color = neopixelCmd["color"].as<JsonObject>())
    if (neopixelCmd["color"].is<JsonObject>())
    {
        JsonObject color = neopixelCmd["color"].as<JsonObject>();
        leds[0].setRGB(
            color["r"],
            color["g"],
            color["b"]
        );
        updateNeeded = true;
    }

    // Only update LED if any changes incurred
    if (updateNeeded)
    {
        FastLED.show();
    }
}

void getNeopixelState(JsonObject &payload)
{
    JsonObject neopixel = payload["neopixel"].to<JsonObject>();
    JsonObject colorObj = neopixel["color"].to<JsonObject>();
    
    colorObj["r"] = leds[0].r;
    colorObj["g"] = leds[0].g;
    colorObj["b"] = leds[0].b;
}

/*
void sendNeopixelState(const CRGB& color)
{
    JsonDocument doc;
    
    JsonObject neopixel = doc.to<JsonObject>();
    
    JsonObject colorObj = neopixel.createNestedObject("color");
    colorObj["r"] = color.r;
    colorObj["g"] = color.g;
    colorObj["b"] = color.b;
    
    
}
*/