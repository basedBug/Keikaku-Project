#pragma once

#include <FastLED.h>
#include <ArduinoJson.h>
//#include <fl/assert.h>

void initNeopixel();
void handleNeopixelCmd(JsonObject neopixelCmd);
void getNeopixelState(JsonObject &payload);
