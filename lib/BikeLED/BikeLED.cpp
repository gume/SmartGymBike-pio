#include "BikeLED.h"
#include "BikeStat.h"


uint16_t BikeLED::myFadeOutEffect(void) { // Fade out and remain
  WS2812FX::Segment* seg = ws2812fx->getSegment(); // get the current segment
  WS2812FX::Segment_runtime* seg_rt = ws2812fx->getSegmentRuntime(); // get the current segment

  int lum = seg_rt->counter_mode_step;
  if(lum > 255) {
    lum = 255; // lum = 0 -> 255
    seg_rt->aux_param2 |=  CYCLE;
  }

  uint32_t color = ws2812fx->color_blend(seg->colors[0], BLACK, lum);
  ws2812fx->fill(color, seg->start, (uint16_t)(seg->stop - seg->start + 1));

  seg_rt->counter_mode_step += 4;
  return (seg->speed / 128);
}


BikeLED::BikeLED() : bikeLED_mode(BIKELED_NONE) {
    change = false;
}

void BikeLED::setup() {
    ws2812fx->init();
    ws2812fx->setBrightness(64);
  
    ws2812fx->setSpeed(3);
    ws2812fx->setMode(FX_MODE_RAINBOW_CYCLE);
    ws2812fx->start();

    ws2812fx->setCustomMode(myFadeOutEffect);

    lastHB = 0;
    lastRev = 0;
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
        if (bikeLED_mode == BIKELED_STOP) {
            if (openCP) {
                ws2812fx->setColor(YELLOW);
            } else if (!okWiFi) {
                ws2812fx->setColor(RED);
            } else if (!okMQTT) {
                ws2812fx->setColor(ORANGE);
            } else {
                ws2812fx->setColor(BLUE);
            }
        }
        change = false;
    }
    
    if (bikeLED_mode == BIKELED_BIKE) {
        if (revs != bikeStat.bikeRevs) {
            auto c = PURPLE;
            if (bikeStat.bikeCadence >= 50) c = CYAN;
            if (bikeStat.bikeCadence >= 70) c = GREEN;

            ws2812fx->setSegment(1, 1, 2, FX_MODE_CUSTOM, c, 100, GAMMA);
            ws2812fx->resetSegmentRuntime(1);
            //ws2812fx->start();
            revs = bikeStat.bikeRevs;
            lastRev = millis();
        }
    
        if (bikeStat.bikeHR > 0) {
            if (millis() > lastHB + (long) (60.0 / (float) bikeStat.bikeHR * 1000.0)) {
                ws2812fx->resetSegmentRuntime(0);
                lastHB = millis();
            }
        }
    }

    ws2812fx->service();
}

void BikeLED::setMode(int mode) {
    if (bikeLED_mode == mode) return;
    
    bikeLED_mode = mode;
    change = true;

    if (mode == BIKELED_BOOT1) {
        ws2812fx->resetSegments();
        ws2812fx->setPixelColor(0, WHITE);
        ws2812fx->setBrightness(64);
        ws2812fx->show();
    }
    else if (mode == BIKELED_BOOT2) {
        ws2812fx->resetSegments();
        ws2812fx->setPixelColor(0, RED);
        ws2812fx->setPixelColor(1, WHITE);
        ws2812fx->setBrightness(64);
        ws2812fx->show();
    }
    else if (mode == BIKELED_SAFEMODE) {
        ws2812fx->resetSegments();
        ws2812fx->setSegment(0, 0, 2, FX_MODE_BREATH, YELLOW,  50, GAMMA);
        ws2812fx->setBrightness(64);
        ws2812fx->start();
    }
    else if (mode == BIKELED_NONE) {
        ws2812fx->stop();
        ws2812fx->resetSegments();
        ws2812fx->clear();
    }
    else if (mode == BIKELED_STOP) {
        ws2812fx->resetSegments();
        ws2812fx->setSegment(0, 0, 2, FX_MODE_BREATH, BLUE,  50, GAMMA);
        ws2812fx->setBrightness(64);
        ws2812fx->start();
    }
    else if (mode == BIKELED_BIKE) {
        ws2812fx->resetSegments();
        ws2812fx->setSegment(0, 0, 0, FX_MODE_CUSTOM, RED,  50, GAMMA);
        ws2812fx->setSegment(1, 1, 2, FX_MODE_CUSTOM, PURPLE,  50, GAMMA);
        ws2812fx->setBrightness(32);
        ws2812fx->start();
    }

}
