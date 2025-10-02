#include "tasks/magEncoder/magEncoder.h"

AS5600 as5600;

const gpio_num_t I2C_SDA_PIN = GPIO_NUM_1;
const gpio_num_t I2C_SCL_PIN = GPIO_NUM_2;
//const uint32_t I2C_FREQ = 100000UL;

const gpio_num_t AS5600_DIRECTION_PIN = GPIO_NUM_4;


void magEncoderTask(void *pvParameters)
{
	Serial.println("[Mag Encoder] Task started");

	Serial.println("[Mag Encoder] Initializing I2C comms channel");
	Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
	//if(Wire.available())
	//	Wire.setClock(I2C_FREQ);

	initEncoder();
    
	TickType_t xLastWakeTime;
	TickType_t xLastConnectionCheckTime;
	const TickType_t xTimeInterval = pdMS_TO_TICKS(2000);
	const TickType_t xConnectionCheckTimeInterval = pdMS_TO_TICKS(5000);

	// Initialise the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();
	xLastConnectionCheckTime = xTaskGetTickCount();

	/*
		Just to keep the task alive
		Cant just delete the task, the lost references would break the system
	*/
	while (true)
	{
		if (xTaskGetTickCount() - xLastConnectionCheckTime >= xConnectionCheckTimeInterval)
		{	
			if (!as5600.isConnected())
			{
				Serial.println("[Mag Encoder] AS5600 encoder connection failure !!!");
				Serial.println("[Mag Encoder] Trying to reconnect AS5600 encoder...");
				initEncoder();
			}
				
			xLastConnectionCheckTime = xTaskGetTickCount();
		}

		if (xTaskGetTickCount() - xLastWakeTime >= xTimeInterval)
		{	
			// Encoder motor control
				
			xLastWakeTime = xTaskGetTickCount();
		}

		

		vTaskDelay(pdTICKS_TO_MS(10));
	}
}

void initEncoder()
{
	Serial.println("[Mag Encoder] Initializing AS5600 encoder");
	as5600.begin(AS5600_DIRECTION_PIN);	// Software controlled direction

	if (as5600.isConnected())
	{
		Serial.println("[Mag Encoder] AS5600 encoder connection succesful");
	}
	else
	{
		Serial.println("[Mag Encoder] Failed connection to AS5600 encoder !!!");
		return;
	}
	

	as5600.setDirection(AS5600_CLOCK_WISE);	// Increment angle value when moving clock wise

	Serial.println("[Mag Encoder] Reading initial AS5600 status");

	Serial.print("[Mag Encoder] \tMagnet detection: ");
	if (as5600.detectMagnet()) // 
	{
		Serial.print("MAGNET DETECTED \n");

		Serial.print("[Mag Encoder] \tMagnet strength: ");
		if (as5600.magnetTooStrong()) // 
		{
			Serial.print("TOO STRONG \n");
		}
		else if (as5600.magnetTooWeak())
		{
			Serial.print("TOO WEAK \n");
		}
	}
	else
	{
		Serial.print("MAGNET NOT DETECTED \n");
		return;
	}

	/*
		The signals coming from the Hall sensors are first amplified and filtered
		before being converted by the analog-to-digital converter (ADC).

		The output of the ADC is processed by the hardwired CORDIC block 
		(Coordinate Rotation Digital Computer) to compute the angle and magnitude 
		of the magnetic field vector.
	*/
	Serial.print("[Mag Encoder] \tCORDIC ((Coordinate Rotation Digital Computer) magnitude: ");
	Serial.println(as5600.readMagnitude());

	/*
		The intensity of the magnetic field is used by the automatic gain control 
		(AGC) to adjust the amplification level to compensate for temperature and
		magnetic field variations.

		For the most robust performance, the gain value should be in the
		center of its range. The airgap of the physical system can be
		adjusted to achieve this value.
	*/
	Serial.print("[Mag Encoder] \tAGC (auto gain control) gain: ");
	Serial.println(as5600.readAGC());

	/*
		Just set the initial position for the cumulative position, the first read for 
		the cumulative position is irrelevant.
	*/
	as5600.getCumulativePosition();
}

void getEncoderData(JsonObject &payload)
{
	// Main object
	JsonObject as5600_ = payload["as5600"].to<JsonObject>();

	JsonObject status = as5600_["status"].to<JsonObject>();
	status["connected"] = as5600.isConnected();

	// If not connected, theres no point in trying to read the data
	if (!as5600.isConnected())
		return;

	status["magnetDetected"] = as5600.detectMagnet();
	status["magnetTooStrong"] = as5600.magnetTooStrong();
	status["magnetTooWeak"] = as5600.magnetTooWeak();

	/*
	if (as5600.isConnected())
		status["connected"] = "True";
	else
		status["connected"] = "False";
	if (as5600.detectMagnet())
		status["magnetDetected"] = "True";
	else
		status["magnetDetected"] = "False";
	
	if (as5600.magnetTooStrong())
		status["magnetTooStrong"] = "True";
	else
		status["magnetTooStrong"] = "False";
	
	if (as5600.magnetTooWeak())
		status["magnetTooWeak"] = "True";
	else
		status["magnetTooWeak"] = "False";
	*/

	status["cordicMagnitude"] = as5600.readMagnitude();
	status["agcGain"] = as5600.readAGC();

    JsonObject telemetry = as5600_["telemetry"].to<JsonObject>();
    telemetry["angle"] = as5600.readAngle();
	telemetry["rawAngle"] = as5600.rawAngle();
	telemetry["cumulativeAngle"] = as5600.getCumulativePosition();
	telemetry["revolutions"] = as5600.getRevolutions();
	telemetry["speedDeg"] = as5600.getAngularSpeed(AS5600_MODE_DEGREES, false);
	telemetry["speedRad"] = as5600.getAngularSpeed(AS5600_MODE_RADIANS, false);
	telemetry["speedRPM"] = as5600.getAngularSpeed(AS5600_MODE_RPM, false);

	JsonObject configuration = as5600_["configuration"].to<JsonObject>();
	//configuration["powerMode"] = as5600.getPowerMode();
	configuration["powerMode"] = bitmaskToPowerMode(as5600.getPowerMode());
	configuration["hysteresis"] = bitmaskToHysteresis(as5600.getHysteresis());
	configuration["slowFilter"] = bitmaskToSlowFilter(as5600.getSlowFilter());
	configuration["fastFilter"] = bitmaskToFastFilter(as5600.getFastFilter());
}

void handleEncoderCmd(JsonObject encoderCmd)
{
    bool updateNeeded = false;

    if (encoderCmd["configuration"].is<JsonObject>())
    {
        JsonObject configuration = encoderCmd["configuration"].as<JsonObject>();

		// To check that a key exists regardless of its type, you can use is<JsonVariant>()
		if (configuration["powerMode"].is<JsonVariant>())
		{
			const char* powerModeStr = configuration["powerMode"];
			uint8_t powerModeBitmask = powerModeToBitmask(powerModeStr);
			//uint8_t powerMode = configuration["powerMode"];
			if (powerModeBitmask != static_cast<uint8_t>(PowerMode::UNKNOWN))
			{
				as5600.setPowerMode(powerModeBitmask);
				Serial.printf("[Mag Encoder] Set powerMode: %s (mode: %u)\n", powerModeStr, powerModeBitmask);
				updateNeeded = true;
			}
		}

		if (configuration["hysteresis"].is<JsonVariant>())
		{
			const char* hysteresisStr = configuration["hysteresis"];
			uint8_t hysteresisBitmask = hysteresisToBitmask(hysteresisStr);
			//uint8_t hysteresis = configuration["hysteresis"];
			if (hysteresisBitmask != static_cast<uint8_t>(Hysteresis::UNKNOWN))
			{
				as5600.setHysteresis(hysteresisBitmask);
				Serial.printf("[Mag Encoder] Set hysteresis: %s (mode: %u)\n", hysteresisStr, hysteresisBitmask);
				updateNeeded = true;
			}
		}

		if (configuration["slowFilter"].is<JsonVariant>())
		{
			const char* slowFilterStr = configuration["slowFilter"];
			uint8_t slowFilterBitmask = slowFilterToBitmask(slowFilterStr);
			//uint8_t slowFilter = configuration["slowFilter"];
			if (slowFilterBitmask != static_cast<uint8_t>(SlowFilter::UNKNOWN))
			{
				as5600.setSlowFilter(slowFilterBitmask);
				Serial.printf("[Mag Encoder] Set slowFilter: %s (mode: %u)\n", slowFilterStr, slowFilterBitmask);
				updateNeeded = true;
			}
		}

		if (configuration["fastFilter"].is<JsonVariant>())
		{
			const char* fastFilterStr = configuration["fastFilter"];
			uint8_t fastFilterBitmask = fastFilterToBitmask(fastFilterStr);
			//uint8_t fastFilter = configuration["fastFilter"];
			if (fastFilterBitmask != static_cast<uint8_t>(FastFilter::UNKNOWN))
			{
				as5600.setFastFilter(fastFilterBitmask);
				Serial.printf("[Mag Encoder] Set fastFilter: %s (mode: %u)\n", fastFilterStr, fastFilterBitmask);
				updateNeeded = true;
			}
		}
    }

    // Only update if any changes incurred
    if (updateNeeded)
    {
        // Save config into non volatile memory?
    }
}

uint8_t powerModeToBitmask(const char* keyValue)
{
	if (strcmp(keyValue, "NORMAL") == 0)
		return static_cast<uint8_t>(PowerMode::NORMAL);

	if (strcmp(keyValue, "LOW_POWER_MODE_1") == 0)
		return static_cast<uint8_t>(PowerMode::LOW_POWER_MODE_1);

	if (strcmp(keyValue, "LOW_POWER_MODE_2") == 0)
		return static_cast<uint8_t>(PowerMode::LOW_POWER_MODE_2);

	if (strcmp(keyValue, "LOW_POWER_MODE_3") == 0)
		return static_cast<uint8_t>(PowerMode::LOW_POWER_MODE_3);

	return static_cast<uint8_t>(PowerMode::UNKNOWN); // Default return value if no match is found
}

uint8_t hysteresisToBitmask(const char* keyValue)
{
	if (strcmp(keyValue, "OFF") == 0)
		return static_cast<uint8_t>(Hysteresis::OFF);

	if (strcmp(keyValue, "1_LSB") == 0)
		return static_cast<uint8_t>(Hysteresis::LSB_1);

	if (strcmp(keyValue, "2_LSB") == 0)
		return static_cast<uint8_t>(Hysteresis::LSB_2);

	if (strcmp(keyValue, "3_LSB") == 0)
		return static_cast<uint8_t>(Hysteresis::LSB_3);

	return static_cast<uint8_t>(Hysteresis::UNKNOWN); // Default return value if no match is found
}

uint8_t slowFilterToBitmask(const char* keyValue)
{
	if ( (strcmp(keyValue, "16x") == 0) || (strcmp(keyValue, "16X") == 0) )
		return static_cast<uint8_t>(SlowFilter::_16X);

	if ( (strcmp(keyValue, "8x") == 0) || (strcmp(keyValue, "8X") == 0 ) )
		return static_cast<uint8_t>(SlowFilter::_8X);

	if ( (strcmp(keyValue, "4x") == 0) || (strcmp(keyValue, "4X") == 0 ) )
		return static_cast<uint8_t>(SlowFilter::_4X);

	if ( (strcmp(keyValue, "2x") == 0) || (strcmp(keyValue, "2X") == 0 ) )
		return static_cast<uint8_t>(SlowFilter::_2X);

	return static_cast<uint8_t>(SlowFilter::UNKNOWN); // Default return value if no match is found
}

uint8_t fastFilterToBitmask(const char* keyValue)
{
	if (strcmp(keyValue, "SLOW_FILTER_ONLY") == 0)
		return static_cast<uint8_t>(FastFilter::SLOW_FILTER_ONLY);

	if (strcmp(keyValue, "6_LSB") == 0)
		return static_cast<uint8_t>(FastFilter::LSB_6);

	if (strcmp(keyValue, "7_LSB") == 0)
		return static_cast<uint8_t>(FastFilter::LSB_7);

	if (strcmp(keyValue, "9_LSB") == 0)
		return static_cast<uint8_t>(FastFilter::LSB_9);

	if (strcmp(keyValue, "18_LSB") == 0)
		return static_cast<uint8_t>(FastFilter::LSB_18);

	if (strcmp(keyValue, "21_LSB") == 0)
		return static_cast<uint8_t>(FastFilter::LSB_21);

	if (strcmp(keyValue, "24_LSB") == 0)
		return static_cast<uint8_t>(FastFilter::LSB_24);

	if (strcmp(keyValue, "10_LSB") == 0)
		return static_cast<uint8_t>(FastFilter::LSB_10);

	return static_cast<uint8_t>(FastFilter::UNKNOWN); // Default return value if no match is found
}

const char* bitmaskToPowerMode(uint8_t bitmask)
{
	switch (bitmask)
	{
		case static_cast<uint8_t>(PowerMode::NORMAL): return "NORMAL";
		case static_cast<uint8_t>(PowerMode::LOW_POWER_MODE_1): return "LOW_POWER_MODE_1";
		case static_cast<uint8_t>(PowerMode::LOW_POWER_MODE_2): return "LOW_POWER_MODE_2";
		case static_cast<uint8_t>(PowerMode::LOW_POWER_MODE_3): return "LOW_POWER_MODE_3";
        default: return "UNKNOWN";
	}
}

const char* bitmaskToHysteresis(uint8_t bitmask)
{
	switch (bitmask)
	{
		case static_cast<uint8_t>(Hysteresis::OFF): return "OFF";
		case static_cast<uint8_t>(Hysteresis::LSB_1): return "1_LSB";
		case static_cast<uint8_t>(Hysteresis::LSB_2): return "2_LSB";
		case static_cast<uint8_t>(Hysteresis::LSB_3): return "3_LSB";
        default: return "UNKNOWN";
	}
}

const char* bitmaskToSlowFilter(uint8_t bitmask)
{
	switch (bitmask)
	{
		case static_cast<uint8_t>(SlowFilter::_16X): return "16X";
		case static_cast<uint8_t>(SlowFilter::_8X): return "8X";
		case static_cast<uint8_t>(SlowFilter::_4X): return "4X";
		case static_cast<uint8_t>(SlowFilter::_2X): return "2X";
        default: return "UNKNOWN";
	}
}

const char* bitmaskToFastFilter(uint8_t bitmask)
{
	switch (bitmask)
	{
		case static_cast<uint8_t>(FastFilter::SLOW_FILTER_ONLY): return "SLOW_FILTER_ONLY";
		case static_cast<uint8_t>(FastFilter::LSB_6): return "6_LSB";
		case static_cast<uint8_t>(FastFilter::LSB_7): return "7_LSB";
		case static_cast<uint8_t>(FastFilter::LSB_9): return "9_LSB";
		case static_cast<uint8_t>(FastFilter::LSB_18): return "18_LSB";
		case static_cast<uint8_t>(FastFilter::LSB_21): return "21_LSB";
		case static_cast<uint8_t>(FastFilter::LSB_24): return "24_LSB";
		case static_cast<uint8_t>(FastFilter::LSB_10): return "10_LSB";
        default: return "UNKNOWN";
	}
}