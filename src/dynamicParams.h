/****************************************************************************************************************************
  dynamicParams.h
  For ESP8266 / ESP32 boards

  ESPAsync_WiFiManager_Lite (https://github.com/khoih-prog/ESPAsync_WiFiManager_Lite) is a library
  for the ESP32/ESP8266 boards to enable store Credentials in EEPROM/SPIFFS/LittleFS for easy
  configuration/reconfiguration and autoconnect/autoreconnect of WiFi and other services without Hardcoding.

  Built by Khoi Hoang https://github.com/khoih-prog/ESPAsync_WiFiManager_Lite
  Licensed under MIT license
 *****************************************************************************************************************************/

#ifndef dynamicParams_h
#define dynamicParams_h

#include "defines.h"

// USE_DYNAMIC_PARAMETERS defined in defined.h

/////////////// Start dynamic Credentials ///////////////

//Defined in <ESPAsync_WiFiManager_Lite.h>
/**************************************
  #define MAX_ID_LEN                5
  #define MAX_DISPLAY_NAME_LEN      16

  typedef struct
  {
  char id             [MAX_ID_LEN + 1];
  char displayName    [MAX_DISPLAY_NAME_LEN + 1];
  char *pdata;
  uint8_t maxlen;
  } MenuItem;
**************************************/

#if USE_DYNAMIC_PARAMETERS

#define MQTT_SERVER_LEN       32
#define MQTT_SERVERPORT_LEN   6
#define MQTT_USERNAME_LEN     20
#define MQTT_PASSWORD_LEN     40


char MQTT_SERVER     [MQTT_SERVER_LEN + 1]        = "mqtt.home";
char MQTT_SERVERPORT [MQTT_SERVERPORT_LEN + 1]    = "1883";     //1883, or 8883 for SSL
char MQTT_USERNAME   [MQTT_USERNAME_LEN + 1]      = "";
char MQTT_PASSWORD   [MQTT_PASSWORD_LEN + 1]      = "";

MenuItem myMenuItems [] =
{
  { "svr", "MQTT_SERVER",      MQTT_SERVER,     MQTT_SERVER_LEN },
  { "prt", "MQTT_SERVERPORT",  MQTT_SERVERPORT, MQTT_SERVERPORT_LEN },
  { "usr", "MQTT_USERNAME",    MQTT_USERNAME,   MQTT_USERNAME_LEN },
  { "pwd", "MQTT_PASSWORD",    MQTT_PASSWORD,   MQTT_PASSWORD_LEN },
};

uint16_t NUM_MENU_ITEMS = sizeof(myMenuItems) / sizeof(MenuItem);  //MenuItemSize;

#else

MenuItem myMenuItems [] = {};

uint16_t NUM_MENU_ITEMS = 0;

#endif    //USE_DYNAMIC_PARAMETERS


#endif      //dynamicParams_h
