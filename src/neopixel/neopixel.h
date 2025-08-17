#pragma once

#include <FastLED.h>
#include <ArduinoJson.h>
//#include <fl/assert.h>

void initNeopixel();
void handleNeopixel(JsonObject neopixelCmd);
void getNeopixelState(JsonObject &payload);
