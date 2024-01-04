#pragma once

//#include <Adafruit_NeoPixel.h>
#include <WS2812FX.h>

#define BIKELED_NONE    0
#define BIKELED_STOP    1
#define BIKELED_BIKE    2

class BikeLED {
public:
    BikeLED();
    void setup();
    void loop();
    void setMode(int mode);

    static WS2812FX *ws2812fx;

private:
    int bikeLED_mode;
    bool change;

    bool okWiFi;
    bool okMQTT;
    bool openCP;
};