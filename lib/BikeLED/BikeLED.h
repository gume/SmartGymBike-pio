#pragma once

//#include <Adafruit_NeoPixel.h>
#include <WS2812FX.h>

#define BIKELED_NONE    0
#define BIKELED_STOP    1
#define BIKELED_BIKE    2
#define BIKELED_BOOT1   3
#define BIKELED_BOOT2   4
#define BIKELED_SAFEMODE    5


class BikeLED {
public:
    BikeLED();
    void setup();
    void loop();
    void setMode(int mode);

    static uint16_t myFadeOutEffect(void);

    static WS2812FX *ws2812fx;

private:
    int bikeLED_mode;
    bool change;

    bool okWiFi;
    bool okMQTT;
    bool openCP;

    int revs;
    uint32_t lastRev;
    uint32_t lastHB;
};