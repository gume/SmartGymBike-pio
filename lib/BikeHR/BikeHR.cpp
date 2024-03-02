#include "BikeHR.h"

BLEAddress* BikeHR::peripheralAddress = nullptr;
BLEAdvertisedDevice* BikeHR::myAdvertisedDevice = nullptr;

BikeHR::BikeHR() : bikeStat(BikeStat::getInstance())/*,  peripheralAddress(nullptr)*/ {
    scanStarted = false;
}

void BikeHR::setup() {
    BLEDevice::init("ESP32");
    bikeStat.bikeHR = 0;
}

bool BikeHR::connectSensor() {
    
    if (waitFor > millis()) return false;

    if (!scanStarted) {
        //Serial.println("Start scan");
        // Find the device address

        BLEScan* pBLEScan = BLEDevice::getScan();
        pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
        pBLEScan->setInterval(80);
        pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks(), true, true);
        pBLEScan->setWindow(30);  // less or equal setInterval value
        pBLEScan->start(0, nullptr, true); // Async scan
        scanStarted = true;
    }

    if (!myAdvertisedDevice) return false;

    if (!pClient) {
        Serial.println("Create client");
        // Create client
        pClient = BLEDevice::createClient();
    }
    
    if (!pClient->isConnected()) {
        //pClient->connect(*peripheralAddress); // This is wrong..
        pClient->connect(myAdvertisedDevice);
        pRemoteService = nullptr;
        pRemoteCharacteristic = nullptr;
        waitFor = millis() + 1000;
        return false;
    }

    if (!pRemoteCharacteristic) {
        //Serial.println("get Service");
        pRemoteService = pClient->getService(BLEUUID((uint16_t)0x180D));
        if (pRemoteService == nullptr) {
            pClient->disconnect();
            myAdvertisedDevice = nullptr;
            scanStarted = false;
            return false;
        }
        //Serial.println("get Characteristic");
        pRemoteCharacteristic = pRemoteService->getCharacteristic(BLEUUID((uint16_t)0x2A37));
        if (pRemoteCharacteristic == nullptr) {
            pClient->disconnect();
            myAdvertisedDevice = nullptr;
            scanStarted = false;
            return false;
        }
        pRemoteCharacteristic->registerForNotify(BikeHR::hRCallback);

    }

    waitFor = 0;
    //Serial.println("Connected");
    return true;
}

void BikeHR::loop() {
    if (!pClient || !pRemoteCharacteristic) {
        connectSensor();
        return;
    }
    
    if (millis() < waitFor) return;

    if (millis() > bikeStat.bikeHRUpdate + 2000) {
        bikeStat.bikeHR = 0;
    }

    bikeStat.bikeHRConnected = pClient->isConnected();
    if (!pClient->isConnected()) {

        //Serial.println("Not connected");
        myAdvertisedDevice = nullptr;
        scanStarted = false;

        pRemoteCharacteristic->registerForNotify(nullptr);
        pRemoteCharacteristic = nullptr;

        return;
    }
}