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
    if (revs != bikeStat.bikeRevs) {
        change = true;
    }

    if (change) {
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
            ws2812fx->setBrightness(64);
            ws2812fx->start();
        }
        else if (bikeLED_mode == BIKELED_BIKE) {
            if (revs != bikeStat.bikeRevs) {
                auto c = PURPLE;
                if (bikeStat.bikeCadence >= 50) c = CYAN;
                if (bikeStat.bikeCadence >= 70) c = GREEN;

                ws2812fx->setSegment(0, 0, 2, FX_MODE_STATIC, c,  50, GAMMA);
                ws2812fx->setBrightness(32);
                ws2812fx->start();
                revs = bikeStat.bikeRevs;
                lastRev = millis();
            }
        }
        change = false;
    } else {
        if (bikeLED_mode == BIKELED_BIKE) {
            int br = 32 - (millis() - lastRev) / 20;
            if (br < 0) br = 0;
            ws2812fx->setBrightness(br);
        }
    }

    ws2812fx->service();
}

void BikeLED::setMode(int mode) {
    if (bikeLED_mode == mode) return;
    
    bikeLED_mode = mode;
    change = true;

    if (mode == BIKELED_BOOT1) {
        ws2812fx->setSegment(0, 0, 2, FX_MODE_STATIC, BLACK,  50, GAMMA);
        ws2812fx->setBrightness(64);
        ws2812fx->start();
        ws2812fx->setPixelColor(0, WHITE);
        ws2812fx->service();
    }
    else if (mode == BIKELED_BOOT2) {
        ws2812fx->setSegment(0, 0, 2, FX_MODE_STATIC, BLACK,  50, GAMMA);
        ws2812fx->setBrightness(64);
        ws2812fx->start();
        ws2812fx->setPixelColor(0, RED);
        ws2812fx->setPixelColor(1, WHITE);
        ws2812fx->service();
    } 
}
