#include "BikeLED.h"
#include "BikeStat.h"


BikeLED::BikeLED() : bikeLED_mode(BIKELED_NONE) {
    change = false;
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

    BikeStat& bikeStat = BikeStat::getInstance();
    if (okWiFi != bikeStat.okWiFi) {
        change = true;
        okWiFi = bikeStat.okWiFi;
    }
    if (okMQTT != bikeStat.okMQTT) {
        change = true;
        okMQTT = bikeStat.okMQTT;
    }
    if (openCP != bikeStat.openCP) {
        change = true;
        openCP = bikeStat.openCP;
    }

    if (change) {
        digitalWrite(LED_BUILTIN, HIGH);
        if (bikeLED_mode == BIKELED_NONE) {
            ws2812fx->clear();
            ws2812fx->stop();
        }
        else if (bikeLED_mode == BIKELED_STOP) {
            if (openCP) {
                ws2812fx->setSegment(0, 0, 2, FX_MODE_BREATH, YELLOW,  50, GAMMA);
            } else if (!okWiFi) {
                ws2812fx->setSegment(0, 0, 2, FX_MODE_BREATH, RED,  50, GAMMA);
            } else if (!okMQTT) {
                ws2812fx->setSegment(0, 0, 2, FX_MODE_BREATH, ORANGE,  50, GAMMA);
            } else {
                ws2812fx->setSegment(0, 0, 2, FX_MODE_BREATH, BLUE,  100, GAMMA);
            }
            ws2812fx->start();
        }
        else if (bikeLED_mode == BIKELED_BIKE) {
            //
        }

        change = false;
    } else {
        digitalWrite(LED_BUILTIN, LOW);
    }

    ws2812fx->service();
}

void BikeLED::setMode(int mode) {
    if (bikeLED_mode == mode) return;
    
    bikeLED_mode = mode;
    change = true;
}

/*
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
*/