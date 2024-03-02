#pragma once

#include <Arduino.h>
#include "BikeStat.h"

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

class BikeHR {
    public:
    BikeHR();

    void setup();
    void loop();

    private:
    bool connectSensor();

    uint32_t waitFor;
    bool scanStarted;

    static BLEAddress* peripheralAddress;
    static BLEAdvertisedDevice* myAdvertisedDevice;

    BLEClient* pClient = nullptr;
    BLERemoteService* pRemoteService = nullptr;
    BLERemoteCharacteristic* pRemoteCharacteristic = nullptr;

    BikeStat& bikeStat;

    static void hRCallback( BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
        //Serial.println("Notify");
        /*Serial.print(length);
        Serial.print(": ");
        for (int i = 0; i < length; i++) {
            Serial.print(pData[i], HEX);
            Serial.print(" ");
        }
        Serial.println();*/
        int rpm = 0;
        if ((pData[0] & 1) == 0) {
            // 8 bit data
            rpm = pData[1];
        } else {
            // 16 bit data
            rpm = pData[1] * 256 + pData[2];
        }
        bool contactSupport = ((pData[0] & 4) > 0);
        bool contactStatus = ((pData[0] & 2) > 0);

        //Serial.println(rpm);
        BikeStat& bs = BikeStat::getInstance();
        if (!(contactSupport && !contactStatus)) {
            bs.bikeHR = rpm;
        }
        if (contactSupport && !contactStatus) {
            bs.bikeHR = 0;
        }
        bs.bikeHRUpdate = millis();
    }

    class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
        void onResult(BLEAdvertisedDevice advertisedDevice) {
            //Serial.println(advertisedDevice.toString().c_str());
            BikeStat& bs = BikeStat::getInstance();
            if (advertisedDevice.getName().compare(bs.bikeHRSensorName.c_str()) == 0) {
                bs.bikeHRSensorAddress = String(advertisedDevice.getAddress().toString().c_str());
                BikeHR::peripheralAddress = new BLEAddress(bs.bikeHRSensorAddress.c_str());
                //Serial.println(BikeHR::peripheralAddress->toString().c_str());
                BikeHR::myAdvertisedDevice = new BLEAdvertisedDevice(advertisedDevice);

                // Stop scanning
                //Serial.println("Stop scan");
                BLEScan* pBLEScan = BLEDevice::getScan();
                pBLEScan->stop();
            }
        }
    };
};