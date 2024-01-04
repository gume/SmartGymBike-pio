#pragma once

//#include <Adafruit_NeoPixel.h>
#include <WS2812FX.h>

#define BIKELED_NONE    0
#define BIKELED_BREATH  1
#define BIKELED_RUNNING 2

class BikeLED {
public:
    BikeLED();
    void setup();
    void loop();

    void breath(int color);
    void running(int cadenceP, int levelP);

    static WS2812FX *ws2812fx;

private:
    int bikeLED_mode;
};