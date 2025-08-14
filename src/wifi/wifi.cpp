#include "wifi/wifi.h"

void initWifi()
{
    WiFi.onEvent(WiFiEvent);

	WiFi.mode(WIFI_STA);    // Set to station mode (mcu to local wifi connection)
	WiFi.disconnect();  // Disconnect from access point if previously connected
	
    Serial.println("[WiFi] Connecting to wifi...");
	WiFi.begin(ssid, password);
	
    while (WiFi.status() != WL_CONNECTED)
	{
		vTaskDelay(pdMS_TO_TICKS(500));
		Serial.println("[WiFi] ...");
	}

    /*
	Serial.print("[WiFi] Connected to ");
	Serial.println(ssid);
	Serial.print("[WiFi] IP address: ");
	Serial.println(WiFi.localIP());
    */
}

/*
* WiFi Events

0  ARDUINO_EVENT_WIFI_READY               < ESP32 WiFi ready
1  ARDUINO_EVENT_WIFI_SCAN_DONE                < ESP32 finish scanning AP
2  ARDUINO_EVENT_WIFI_STA_START                < ESP32 station start
3  ARDUINO_EVENT_WIFI_STA_STOP                 < ESP32 station stop
4  ARDUINO_EVENT_WIFI_STA_CONNECTED            < ESP32 station connected to AP
5  ARDUINO_EVENT_WIFI_STA_DISCONNECTED         < ESP32 station disconnected from AP
6  ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE      < the auth mode of AP connected by ESP32 station changed
7  ARDUINO_EVENT_WIFI_STA_GOT_IP               < ESP32 station got IP from connected AP
8  ARDUINO_EVENT_WIFI_STA_LOST_IP              < ESP32 station lost IP and the IP is reset to 0
9  ARDUINO_EVENT_WPS_ER_SUCCESS       < ESP32 station wps succeeds in enrollee mode
10 ARDUINO_EVENT_WPS_ER_FAILED        < ESP32 station wps fails in enrollee mode
11 ARDUINO_EVENT_WPS_ER_TIMEOUT       < ESP32 station wps timeout in enrollee mode
12 ARDUINO_EVENT_WPS_ER_PIN           < ESP32 station wps pin code in enrollee mode
13 ARDUINO_EVENT_WIFI_AP_START                 < ESP32 soft-AP start
14 ARDUINO_EVENT_WIFI_AP_STOP                  < ESP32 soft-AP stop
15 ARDUINO_EVENT_WIFI_AP_STACONNECTED          < a station connected to ESP32 soft-AP
16 ARDUINO_EVENT_WIFI_AP_STADISCONNECTED       < a station disconnected from ESP32 soft-AP
17 ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED         < ESP32 soft-AP assign an IP to a connected station
18 ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED        < Receive probe request packet in soft-AP interface
19 ARDUINO_EVENT_WIFI_AP_GOT_IP6               < ESP32 ap interface v6IP addr is preferred
19 ARDUINO_EVENT_WIFI_STA_GOT_IP6              < ESP32 station interface v6IP addr is preferred
20 ARDUINO_EVENT_ETH_START                < ESP32 ethernet start
21 ARDUINO_EVENT_ETH_STOP                 < ESP32 ethernet stop
22 ARDUINO_EVENT_ETH_CONNECTED            < ESP32 ethernet phy link up
23 ARDUINO_EVENT_ETH_DISCONNECTED         < ESP32 ethernet phy link down
24 ARDUINO_EVENT_ETH_GOT_IP               < ESP32 ethernet got IP from connected AP
19 ARDUINO_EVENT_ETH_GOT_IP6              < ESP32 ethernet interface v6IP addr is preferred
25 ARDUINO_EVENT_MAX
*/

void WiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info)
{
    switch (event)
    {
        case ARDUINO_EVENT_WIFI_READY:
            Serial.println("[WiFi] Wifi interface ready");
            break;
        
        case ARDUINO_EVENT_WIFI_STA_START:
            Serial.println("[WiFi] Wifi STA mode initialized");
            break;

        case ARDUINO_EVENT_WIFI_STA_STOP:
            Serial.println("[WiFi] Wifi STA mode stopped");
            break;

        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        {
            //Serial.print("[WiFi] Connected to ");
            //Serial.println((char*) info.wifi_sta_connected.ssid);
            Serial.printf("[WiFi] Connected to AP: %s (BSSID: %s) \n",
                //info.wifi_sta_connected.ssid, 
                WiFi.SSID().c_str(),    // Casted to char str as it directly returned a string
                //info.wifi_sta_connected.bssid
                WiFi.BSSIDstr().c_str()
            );
            break;
        }
            
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        {
            Serial.printf("[WiFi] Lost connection!. Reason: %u (%s) \n",
                info.wifi_sta_disconnected.reason, 
                /*
                Had to cast this as the compiler would bitch about the damn type (even the docs 
                dont say that the type is no longer uint8_t, it's wifi_err_reason_t, which isnt
                anywhere else)
                */
               WiFi.disconnectReasonName(static_cast<wifi_err_reason_t>(info.wifi_sta_disconnected.reason))
            );
            Serial.println("[WiFi] Trying to reconnect...");
            
            // Try and connect wifi again
            WiFi.reconnect();
            //WiFi.begin(ssid, password);
            break;
        }

        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        {
            Serial.printf("[WiFi] Obtained IP address: %s, Gateway: %s, RSSI: %d dBm \n",
                //IPAddress(info.got_ip.ip_info.ip.addr).toString().c_str(),
                WiFi.localIP().toString().c_str(),
                //IPAddress(info.got_ip.ip_info.gw.addr).toString().c_str(),
                WiFi.gatewayIP().toString().c_str(),
                WiFi.RSSI()
            );
            break;
        }

        case ARDUINO_EVENT_WIFI_STA_LOST_IP:
            Serial.println("IP lost - waiting for DHCP renewal");
            break;

        default:
            Serial.printf("[WiFi] Unhandled event: %d \n", event);
            break;
    }
}