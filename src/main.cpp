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
#include "EspMQTTClient.h"
#include <HAMqtt.h>

#include "BikeStat.h"
#include "BikeDisplay.h"
#include "BikeLED.h"
#include <WS2812FX.h>

#include "SmartGymBike.h"

#define FORMAT_LITTLEFS_IF_FAILED true

ESPAsync_WiFiManager_Lite* ESPAsync_WiFiManager;
//WiFiClient espClient;
//PubSubClient client(espClient);
EspMQTTClient *client;
HAMqttDevice *haDevice;
HAMqttEntity *haEntity;

BikeStat* BikeStat::instance = nullptr;
String BikeStat::bikeRikeVersion = "BikeRike 0.24";

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
  else if (btn->getPin() == PIN_BTNU) {
    if (pattern == BfButton::SINGLE_PRESS) smartGymBike.resistanceUp();
    else if (pattern == BfButton::DOUBLE_PRESS) smartGymBike.resistanceLevelUp();
    else smartGymBike.resistanceSet(SmartGymBike::RES_MAX);
  }
  else if (btn->getPin() == PIN_BTND) {
    if (pattern == BfButton::SINGLE_PRESS) smartGymBike.resistanceDown();
    else if (pattern == BfButton::DOUBLE_PRESS) smartGymBike.resistanceLevelDown();
    else smartGymBike.resistanceSet(SmartGymBike::RES_MIN);
  }
  //digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void onConnectionEstablished() {
  // sends the configuration payload on the configuration topic.
  client->publish(haEntity->getDiscoveryTopic(), haEntity->getConfigPayload(), true);
}

void setup() {
  Serial.begin(115200);

  Serial.println("Starting SmartGymBike");

  bikeLED.setup();
  bikeLED.setMode(BIKELED_BOOT1);

  Serial.println("FileSystem start");
  LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED);

  // Check Service button, if it is down, erase the config
  if (digitalRead(PIN_BTNS) == 0) {
    LittleFS.format();
  }

  bikeLED.setMode(BIKELED_BOOT2);

  Serial.println("WiFiManager start");
  ESPAsync_WiFiManager = new ESPAsync_WiFiManager_Lite();
  ESPAsync_WiFiManager->setConfigPortal("SmarDzsimBajk", "sportolj123");
  ESPAsync_WiFiManager->setConfigPortalChannel(0);
  ESPAsync_WiFiManager->begin("smartgymbike");

  BikeStat& bikeStat = BikeStat::getInstance();
  bikeStat.mqttBroker = myMenuItems[0].pdata;
  bikeStat.mqttBrokerPort = String(myMenuItems[1].pdata).toInt();
  bikeStat.mqttUser = myMenuItems[2].pdata;
  bikeStat.mqttPassword = myMenuItems[3].pdata;

  bikeLED.setMode(BIKELED_STOP);

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

  buttonEnter.onPress(pressHandler).onDoublePress(pressHandler).onPressFor(pressHandler, 1000);
  buttonUser.onPress(pressHandler).onDoublePress(pressHandler).onPressFor(pressHandler, 1000);
  buttonUp.onPress(pressHandler).onDoublePress(pressHandler).onPressFor(pressHandler, 1000);
  buttonDown.onPress(pressHandler).onDoublePress(pressHandler).onPressFor(pressHandler, 1000);

  // Start MQTT
  //client = new EspMQTTClient(bikeStat.mqttBroker.c_str(), "BikeRike", bikeStat.mqttBrokerPort, bikeStat.mqttUser.c_str(), bikeStat.mqttPassword.c_str());
  client = new EspMQTTClient(bikeStat.mqttBroker.c_str(), bikeStat.mqttBrokerPort, "BikeRike");

  haDevice = new HAMqttDevice("smartgymbike", *client);
  haEntity = new HAMqttEntity(*haDevice, "Exercise duration", HAMqttEntity::SENSOR);

  haDevice->addConfig("sw_version", bikeStat.bikeRikeVersion);
  haDevice->addConfig("manufactures", "GumeSoft");

  client->setKeepAlive(MQTT_KEEPALIVE);
  client->setMaxPacketSize(1024);

  haEntity->addStateTopic();
  haEntity->addConfig("device_class", "duration");
  haEntity->addConfig("state_class", "total_increasing");
  haEntity->addConfig("unit_of_measurement", "s");
}

uint32_t lastSysCheck = 0;
uint32_t lastMQTTReport = 0;

void loop() {
  uint32_t now = millis();

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

  BikeStat& bikeStat = BikeStat::getInstance();

  // Check Systen status
  if (now - lastSysCheck > 500) {
    // Chceck WiFi
    bikeStat.okWiFi = (WiFi.status() == WL_CONNECTED);
    // Check MQTT
    bikeStat.okMQTT = client->isMqttConnected();
    // Check Config Portal
    bikeStat.openCP = ESPAsync_WiFiManager->isConfigMode();

    // Set LED according to cadence
    if (bikeStat.bikeCadence > 0) bikeLED.setMode(BIKELED_BIKE);
    else bikeLED.setMode(BIKELED_STOP);

    lastSysCheck = now;
  }
 
  client->loop();
  haDevice->manageAvailability(MQTT_KEEPALIVE);

  // Time to send MQTT messages
  if (now - lastMQTTReport > 5000) {
    client->publish(haEntity->getStateTopic(), String(bikeStat.bikeTime / 1000));
    lastMQTTReport = now;
  }

  //digitalWrite(LED_BUILTIN, !digitalRead(PIN_BTND));
}