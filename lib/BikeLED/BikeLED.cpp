#include "BikeLED.h"

BikeLED::BikeLED() : bikeLED_mode(BIKELED_NONE) {
}

void BikeLED::setup() {
    ws2812fx->init();
    ws2812fx->setBrightness(64);
  
    ws2812fx->setSpeed(3);
    ws2812fx->setMode(FX_MODE_RAINBOW_CYCLE);
    ws2812fx->start();
}

void BikeLED::loop() {
    //digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    ws2812fx->service();
}

void BikeLED::breath(int color) {
    ws2812fx->setSegment(0, 0, 2, FX_MODE_BREATH, color,  1000, NO_OPTIONS);
    ws2812fx->start();
    bikeLED_mode = BIKELED_BREATH;
}

void BikeLED::running(int cadenceP, int levelP) {
    if (bikeLED_mode != BIKELED_RUNNING) {
        ws2812fx->setSegment(0, 0, 2, FX_MODE_SCAN, BLACK,  10000, NO_OPTIONS);
        ws2812fx->start();
        bikeLED_mode = BIKELED_RUNNING;
    }

    byte r, g, b;
    levelP = levelP * 254 / 200;
    if (levelP > 254) levelP = 254;

    b = max(127-levelP,0)*2;
    g = 255-abs(127-levelP)*2;
    r = max(levelP-127,0)*2;
    ws2812fx->setColor(((uint32_t)r << 16) + ((uint32_t)g << 8) + (uint32_t)b);

    cadenceP = cadenceP * 70 / 100;
    if (cadenceP > 140) cadenceP = 140;
    int ls = 6000 - cadenceP*40;
    if (ls < 10) ls = 10;
    ws2812fx->setSpeed(ls);
}
