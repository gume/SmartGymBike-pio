#include "defines.h"
#include "Credentials.h"
#include "dynamicParams.h"

#include "FS.h"
#include <LittleFS.h>

#include <PubSubClient.h>
#include <Adafruit_SSD1306.h>
#include <BfButton.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoOTA.h>

#include "BikeStat.h"
#include "BikeDisplay.h"
#include "BikeLED.h"
#include <WS2812FX.h>

#include "SmartGymBike.h"

#define FORMAT_LITTLEFS_IF_FAILED true

ESPAsync_WiFiManager_Lite* ESPAsync_WiFiManager;
WiFiClient espClient;
PubSubClient client(espClient);

BikeStat* BikeStat::instance = nullptr;
String BikeStat::bikeRikeVersion = "BikeRike 0.12";

BikeLED bikeLED;
WS2812FX *BikeLED::ws2812fx = new WS2812FX(3, 25, NEO_GRB + NEO_KHZ800);

BikeDisplay bikeDisplay;

SmartGymBike smartGymBike;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 *BikeDisplay::display = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define PIN_BTNQ 15
#define PIN_BTNP 16
#define PIN_BTNS 17
#define PIN_BTNE 23
#define PIN_BTND 19
#define PIN_BTNU 18

BfButton buttonUp(BfButton::STANDALONE_DIGITAL, PIN_BTNU);
BfButton buttonDown(BfButton::STANDALONE_DIGITAL, PIN_BTND); 
BfButton buttonSS(BfButton::STANDALONE_DIGITAL, PIN_BTNS);
BfButton buttonUser(BfButton::STANDALONE_DIGITAL, PIN_BTNQ); 
BfButton buttonEnter(BfButton::STANDALONE_DIGITAL, PIN_BTNE); 
BfButton buttonPulse(BfButton::STANDALONE_DIGITAL, PIN_BTNP);

// Initialize the static member outside the class

// Interrupt for cadence sensor
void IRAM_ATTR cadenceIRQ() {
  //digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  SmartGymBike::pedalPush = true;
}

void pressHandler(BfButton *btn, BfButton::press_pattern_t pattern) {
  if (btn->getPin() == PIN_BTNE) {
    bikeDisplay.screenNext();
  }
  else if (btn->getPin() == PIN_BTNQ) {
    bikeDisplay.screenNext(true);
  }
  //digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void setup() {
  Serial.begin(115200);

  Serial.println("Starting SmartGymBike");

  Serial.println("FileSystem start");
  LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED);

  Serial.println("WiFiManager start");
  ESPAsync_WiFiManager = new ESPAsync_WiFiManager_Lite();
  ESPAsync_WiFiManager->setConfigPortal("SmarDzsimBajk", "sportolj123");
  ESPAsync_WiFiManager->setConfigPortalChannel(0);
  ESPAsync_WiFiManager->begin("smartgymbike");

  bikeLED.setup();
  bikeLED.breath(0x0000ff); // Blue

  bikeDisplay.setup();
  bikeDisplay.toast("Welc\nome!", 3000); 

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  // Setup cadence sensor
  pinMode(PIN_SPEED, INPUT_PULLUP);
  attachInterrupt(PIN_SPEED, cadenceIRQ, FALLING);

  buttonEnter.onPress(pressHandler).onDoublePress(pressHandler);
  buttonUser.onPress(pressHandler).onDoublePress(pressHandler);
}

void loop() {
  ESPAsync_WiFiManager->run();

  bikeLED.loop();
  bikeDisplay.loop();

  buttonUp.read();
  buttonDown.read();
  buttonSS.read();
  buttonUser.read();
  buttonPulse.read();
  buttonEnter.read();

  ArduinoOTA.handle();

  smartGymBike.loop();
 
  //digitalWrite(LED_BUILTIN, !digitalRead(PIN_BTND));
}