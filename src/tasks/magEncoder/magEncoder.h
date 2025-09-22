#pragma once

#include <Wire.h>
#include <AS5600.h>
#include <ArduinoJson.h>

enum class PowerMode : uint8_t
{
    NORMAL = 0,
    LOW_POWER_MODE_1 = 1,
    LOW_POWER_MODE_2 = 2,
    LOW_POWER_MODE_3 = 3,
    UNKNOWN     // Just here in case anything goes very wrong
};

enum class Hysteresis : uint8_t
{
    OFF = 0,
    LSB_1 = 1,
    LSB_2 = 2,
    LSB_3 = 3,
    UNKNOWN     // Just here in case anything goes very wrong
};

enum class SlowFilter : uint8_t
{
    _16X = 0,
    _8X = 1,
    _4X = 2,
    _2X = 3,
    UNKNOWN     // Just here in case anything goes very wrong
};

enum class FastFilter : uint8_t
{
    SLOW_FILTER_ONLY = 0,
    LSB_6 = 1,
    LSB_7 = 2,
    LSB_9 = 3,
    LSB_18 = 4,
    LSB_21 = 5,
    LSB_24 = 6,
    LSB_10 = 7,
    UNKNOWN     // Just here in case anything goes very wrong
};

void magEncoderTask(void *pvParameters);

void initEncoder();
void getEncoderData(JsonObject &payload);
void handleEncoderCmd(JsonObject encoderCmd);

// Conversion functions from JSON compatible string to bitmask
uint8_t powerModeToBitmask(const char* keyValue);
uint8_t hysteresisToBitmask(const char* keyValue);
uint8_t slowFilterToBitmask(const char* keyValue);
uint8_t fastFilterToBitmask(const char* keyValue);

// Conversion functions from bitmask to JSON compatible string
const char* bitmaskToPowerMode(uint8_t bitmask);
const char* bitmaskToHysteresis(uint8_t bitmask);
const char* bitmaskToSlowFilter(uint8_t bitmask);
const char* bitmaskToFastFilter(uint8_t bitmask);
